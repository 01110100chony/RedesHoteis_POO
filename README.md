# Sistema de Reservas - Rede de Hotéis Paradise

Aplicação desktop em C++17 e Qt Widgets para gerenciamento de reservas telefônicas das unidades **Jericoacoara**, **Canoa Quebrada** e **Cumbuco**.

## Estrutura do Projeto

- `model/`: classes de domínio (`Pessoa`, `Atendente`, `Cliente`, `TiposHotel`, `EstrategiaDesconto`, `Reserva`)
- `core/`: regras centralizadas e persistência (`GerenciadorReservas` como Singleton e `PersistenciaArquivo` em CSV/TXT)
- `ui/`: janelas Qt (`TelaLogin`, `TelaPrincipal`, `TelaReserva`, `TelaResumoReserva`, `TelaListagemReservas`)
- `tests/`: testes automatizados das regras de negócio (`teste_regras.cpp`)

## Padrões Utilizados

- **Herança e Polimorfismo**: `Atendente` e `Cliente` herdam da classe base abstrata `Pessoa`.
- **Singleton (obrigatório)**: `GerenciadorReservas::getInstance()` centraliza login, clientes, disponibilidade e reservas.
- **Strategy (bônus)**: `EstrategiaDesconto` e suas subclasses (`SemDesconto`, `DescontoClienteVip`, `DescontoBaixaTemporada`, `DescontoPromocaoFeriados`).

## Credenciais de Teste

| Usuário | Senha |
|---|---|
| `atendente1` | `senha1` |
| `atendente2` | `senha2` |
| `atendente3` | `senha3` |
| `atendente4` | `senha4` |

## Como Compilar e Executar

### Qt Creator
Abra o arquivo `HotelParadise.pro` ou `CMakeLists.txt` diretamente no Qt Creator e clique em **Run**.

### Linha de comando (CMake + Ninja)
```powershell
cmake -S . -B build -G Ninja
cmake --build build
.\build\HotelParadise.exe
```

Para executar os testes das regras de negócio:
```powershell
.\build\teste_regras.exe
```

## Releases para Windows e Ubuntu

As releases são montadas pelo GitHub Actions e incluem os runtimes do Qt e do compilador. Quem só vai usar o programa não precisa instalar compilador nem Qt.

- **Windows 11 x64:** baixe `HotelParadise-Windows-x64.zip`, extraia a pasta e abra `HotelParadise.exe`.
- **Ubuntu 22.04 ou mais recente, x86_64:** baixe `HotelParadise-Ubuntu-x86_64.AppImage`, marque como executável (`chmod +x HotelParadise-Ubuntu-x86_64.AppImage`) e abra pelo gerenciador de arquivos ou execute `./HotelParadise-Ubuntu-x86_64.AppImage`.

Para gerar os dois arquivos, envie uma tag de versão ao GitHub, por exemplo:

```bash
git tag v1.0.0
git push origin v1.0.0
```

O workflow `.github/workflows/release.yml` compila cada sistema separadamente e publica os arquivos na página da release. O Ubuntu precisa ser x86_64; o AppImage inclui Qt, mas usa bibliotecas centrais do próprio sistema e pode precisar do FUSE 2 para montagem.

Os dados de clientes e reservas ficam na pasta de dados do usuário (`%APPDATA%/HotelParadise` no Windows e `~/.local/share/HotelParadise` no Ubuntu). Dados que já existam na pasta `dados` ao lado do executável são copiados para esse local na primeira execução.
