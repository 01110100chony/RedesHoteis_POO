$ErrorActionPreference = "Stop"

$root = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$msys2Root = if ($env:MSYS2_LOCATION) { $env:MSYS2_LOCATION } else { "C:\msys64" }
$qtBin = Join-Path $msys2Root "ucrt64\bin"
$buildDir = Join-Path $root "build\release-windows"
$distRoot = Join-Path $root "dist"
$stageRoot = Join-Path $distRoot "windows-x64"
$stage = Join-Path $stageRoot "HotelParadise"
$zipPath = Join-Path $distRoot "HotelParadise-Windows-x64.zip"

if (-not (Test-Path (Join-Path $qtBin "windeployqt.exe"))) {
    throw "windeployqt.exe não encontrado em $qtBin"
}

$env:Path = "$qtBin;$env:Path"
& cmake -S $root -B $buildDir -G Ninja -DCMAKE_BUILD_TYPE=Release "-DCMAKE_PREFIX_PATH=$($msys2Root -replace '\\','/')/ucrt64"
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

& cmake --build $buildDir --target HotelParadise --parallel 4
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

$distFull = [IO.Path]::GetFullPath($distRoot).TrimEnd([IO.Path]::DirectorySeparatorChar)
$stageFull = [IO.Path]::GetFullPath($stageRoot)
if (-not $stageFull.StartsWith($distFull + [IO.Path]::DirectorySeparatorChar, [StringComparison]::OrdinalIgnoreCase)) {
    throw "Staging fora de dist: $stageFull"
}

if (Test-Path -LiteralPath $stageRoot) {
    Remove-Item -LiteralPath $stageRoot -Recurse -Force
}
New-Item -ItemType Directory -Path $stage -Force | Out-Null
Copy-Item -LiteralPath (Join-Path $buildDir "HotelParadise.exe") -Destination (Join-Path $stage "HotelParadise.exe")

$windeployqt = Join-Path $qtBin "windeployqt.exe"
$qtpaths = Join-Path $qtBin "qtpaths.exe"
& $windeployqt --qtpaths $qtpaths --release --compiler-runtime --no-translations (Join-Path $stage "HotelParadise.exe")
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

# O windeployqt do MSYS2 não inclui todos os DLLs MinGW e de terceiros em algumas versões.
# Percorre imports PE dos executáveis, DLLs e plugins e copia o fechamento do runtime UCRT64.
$objdump = Join-Path $qtBin "objdump.exe"
$systemDirs = @(
    (Join-Path $env:SystemRoot "System32"),
    (Join-Path $env:SystemRoot "SysWOW64"),
    $env:SystemRoot
)
$queue = [Collections.Generic.Queue[string]]::new()
$visited = [Collections.Generic.HashSet[string]]::new([StringComparer]::OrdinalIgnoreCase)
Get-ChildItem -LiteralPath $stage -Recurse -File | Where-Object { $_.Extension -in @(".exe", ".dll") } | ForEach-Object {
    $queue.Enqueue($_.FullName)
}

while ($queue.Count -gt 0) {
    $binary = $queue.Dequeue()
    if (-not $visited.Add($binary)) { continue }

    $imports = & $objdump -p $binary 2>&1
    if ($LASTEXITCODE -ne 0) { throw "Não foi possível ler imports PE de $binary" }

    foreach ($line in $imports) {
        if ($line -notmatch '^\s*DLL Name:\s*(.+)$') { continue }
        $dependency = $Matches[1].Trim()
        if ($dependency -match '^(api-ms-win-|ext-ms-win-)') { continue }

        $bundledPath = Join-Path $stage $dependency
        if (Test-Path -LiteralPath $bundledPath) { continue }

        $runtimePath = Join-Path $qtBin $dependency
        if (Test-Path -LiteralPath $runtimePath) {
            Copy-Item -LiteralPath $runtimePath -Destination $bundledPath
            $queue.Enqueue($bundledPath)
            continue
        }

        $isWindowsLibrary = $false
        foreach ($systemDir in $systemDirs) {
            if ($systemDir -and (Test-Path -LiteralPath (Join-Path $systemDir $dependency))) {
                $isWindowsLibrary = $true
                break
            }
        }
        if (-not $isWindowsLibrary) {
            throw "Dependência PE não resolvida: $dependency (importada por $binary)"
        }
    }
}

foreach ($required in @(
    "Qt6Core.dll", "Qt6Gui.dll", "Qt6Widgets.dll",
    "libgcc_s_seh-1.dll", "libstdc++-6.dll", "libwinpthread-1.dll",
    "platforms\qwindows.dll"
)) {
    if (-not (Test-Path -LiteralPath (Join-Path $stage $required))) {
        throw "Dependência ausente no pacote Windows: $required"
    }
}

if (Test-Path -LiteralPath $zipPath) {
    Remove-Item -LiteralPath $zipPath -Force
}
Compress-Archive -Path $stage -DestinationPath $zipPath -CompressionLevel Optimal
Write-Host "Pacote Windows criado: $zipPath"
