#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="$ROOT/build/release-ubuntu"
APPDIR="$ROOT/build/appimage/AppDir"
DIST_ROOT="$ROOT/dist"
APPIMAGE_PATH="$DIST_ROOT/HotelParadise-Ubuntu-x86_64.AppImage"

: "${LINUXDEPLOY:?Defina LINUXDEPLOY apontando para o AppImage do linuxdeploy}"
: "${LINUXDEPLOY_PLUGIN_QT:?Defina LINUXDEPLOY_PLUGIN_QT apontando para o plugin Qt}"

case "$APPDIR" in
  "$ROOT/build/"*) ;;
  *) echo "AppDir fora de build: $APPDIR" >&2; exit 1 ;;
esac

cmake -S "$ROOT" -B "$BUILD_DIR" -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX=/usr
cmake --build "$BUILD_DIR" --target HotelParadise --parallel

rm -rf "$APPDIR"
mkdir -p "$APPDIR"
DESTDIR="$APPDIR" cmake --install "$BUILD_DIR"
mkdir -p "$APPDIR/usr/share/applications"
cp "$ROOT/packaging/hotelparadise.desktop" "$APPDIR/usr/share/applications/HotelParadise.desktop"

mkdir -p "$DIST_ROOT"
rm -f "$APPIMAGE_PATH" "$DIST_ROOT"/HotelParadise-x86_64.AppImage
chmod +x "$LINUXDEPLOY" "$LINUXDEPLOY_PLUGIN_QT"
(
  cd "$DIST_ROOT"
  APPIMAGE_EXTRACT_AND_RUN=1 LDAI_OUTPUT=HotelParadise-x86_64.AppImage "$LINUXDEPLOY" \
    --appdir "$APPDIR" \
    --executable "$APPDIR/usr/bin/HotelParadise" \
    --desktop-file "$APPDIR/usr/share/applications/HotelParadise.desktop" \
    --icon-file "$ROOT/packaging/hotelparadise.svg" \
    --plugin qt \
    --output appimage
)

if [[ ! -f "$DIST_ROOT/HotelParadise-x86_64.AppImage" ]]; then
  echo "linuxdeploy não gerou HotelParadise-x86_64.AppImage" >&2
  exit 1
fi
mv "$DIST_ROOT/HotelParadise-x86_64.AppImage" "$APPIMAGE_PATH"
echo "Pacote Ubuntu criado: $APPIMAGE_PATH"
