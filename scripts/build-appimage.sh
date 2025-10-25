#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"

BUILD_DIR="${PROJECT_ROOT}/build"
APPDIR="${BUILD_DIR}/AppDir"
APPIMAGE_OUTPUT="${BUILD_DIR}/packaging/appimage"

if [[ -f "${PROJECT_ROOT}/VERSION" ]]; then
    VERSION=$(cat "${PROJECT_ROOT}/VERSION")
else
    VERSION=$(grep -oP 'project\(.*VERSION\s+\K[0-9.]+' "${PROJECT_ROOT}/CMakeLists.txt" || echo "1.0.0")
fi

echo "Building AppImage for GamePerf Manager version ${VERSION}..."

if [[ ! -d "${BUILD_DIR}" ]]; then
    echo "Error: Build directory not found. Please build the project first."
    exit 1
fi

mkdir -p "${APPIMAGE_OUTPUT}"
rm -rf "${APPDIR}"
mkdir -p "${APPDIR}"

cd "${BUILD_DIR}"
DESTDIR="${APPDIR}" cmake --install .

mkdir -p "${APPDIR}/usr/share/metainfo"
cat > "${APPDIR}/usr/share/metainfo/gameperf-manager.appdata.xml" <<'EOF'
<?xml version="1.0" encoding="UTF-8"?>
<component type="desktop">
  <id>com.gameperf.Manager</id>
  <metadata_license>CC0-1.0</metadata_license>
  <project_license>MIT</project_license>
  <name>GamePerf Manager</name>
  <summary>Gaming performance management and library integration</summary>
  <description>
    <p>GamePerf Manager provides a comprehensive interface for managing your game libraries across Steam, Epic Games Store (via Legendary CLI), and Wine/Proton, along with real-time performance monitoring.</p>
  </description>
  <url type="homepage">https://gameperf.example.com</url>
  <provides>
    <binary>gameperf-manager</binary>
  </provides>
  <launchable type="desktop-id">gameperf-manager.desktop</launchable>
</component>
EOF

cd "${APPDIR}/usr" || cd "${APPDIR}"

APPRUN="${APPDIR}/AppRun"
cat > "${APPRUN}" <<'APPRUN_EOF'
#!/bin/sh
SELF=$(readlink -f "$0")
HERE=${SELF%/*}

export LD_LIBRARY_PATH="${HERE}/usr/lib:${HERE}/usr/lib/x86_64-linux-gnu:${LD_LIBRARY_PATH}"
export QT_PLUGIN_PATH="${HERE}/usr/lib/qt6/plugins:${QT_PLUGIN_PATH}"
export QML_IMPORT_PATH="${HERE}/usr/lib/qt6/qml:${QML_IMPORT_PATH}"
export QML2_IMPORT_PATH="${HERE}/usr/lib/qt6/qml:${QML2_IMPORT_PATH}"
export PATH="${HERE}/usr/bin:${PATH}"

exec "${HERE}/usr/bin/gameperf-manager" "$@"
APPRUN_EOF

chmod +x "${APPRUN}"

LINUXDEPLOY_URL="https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage"
LINUXDEPLOY_QT_URL="https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-x86_64.AppImage"

LINUXDEPLOY="${BUILD_DIR}/linuxdeploy-x86_64.AppImage"
LINUXDEPLOY_QT="${BUILD_DIR}/linuxdeploy-plugin-qt-x86_64.AppImage"

if [[ ! -x "${LINUXDEPLOY}" ]]; then
    echo "Downloading linuxdeploy..."
    curl -L "${LINUXDEPLOY_URL}" -o "${LINUXDEPLOY}"
    chmod +x "${LINUXDEPLOY}"
fi

if [[ ! -x "${LINUXDEPLOY_QT}" ]]; then
    echo "Downloading linuxdeploy-plugin-qt..."
    curl -L "${LINUXDEPLOY_QT_URL}" -o "${LINUXDEPLOY_QT}"
    chmod +x "${LINUXDEPLOY_QT}"
fi

cd "${BUILD_DIR}"

export QMAKE=/usr/bin/qmake6
export QML_SOURCES_PATHS="${PROJECT_ROOT}/qml"
export VERSION="${VERSION}"

"${LINUXDEPLOY}" --appdir "${APPDIR}" \
    --plugin qt \
    --output appimage \
    --executable "${APPDIR}/usr/bin/gameperf-manager" \
    --desktop-file "${APPDIR}/usr/share/applications/gameperf-manager.desktop" \
    --icon-file "${APPDIR}/usr/share/icons/hicolor/scalable/apps/gameperf-manager.svg"

APPIMAGE_FILE=$(ls GamePerf*Manager*.AppImage 2>/dev/null || echo "")
if [[ -n "${APPIMAGE_FILE}" ]]; then
    mv "${APPIMAGE_FILE}" "${APPIMAGE_OUTPUT}/GamePerf-Manager-${VERSION}-x86_64.AppImage"
    echo "AppImage created: ${APPIMAGE_OUTPUT}/GamePerf-Manager-${VERSION}-x86_64.AppImage"
else
    echo "Warning: AppImage file not found in expected location"
    exit 1
fi
