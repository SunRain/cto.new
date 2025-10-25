#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"

cd "${PROJECT_ROOT}"

TEMP_DEBIAN_DIR=0

if [[ ! -f "${PROJECT_ROOT}/debian/control" ]]; then
    echo "Copying Debian packaging files..."
    rm -rf "${PROJECT_ROOT}/debian"
    cp -r "${PROJECT_ROOT}/packaging/debian" "${PROJECT_ROOT}/debian"
    TEMP_DEBIAN_DIR=1
fi

echo "Building Debian package..."
dpkg-buildpackage -us -uc -b

DEB_FILES=("${PROJECT_ROOT}/../"*.deb)
if [[ -f "${DEB_FILES[0]}" ]]; then
    DEB_OUTPUT="${PROJECT_ROOT}/build/packaging/debian"
    mkdir -p "${DEB_OUTPUT}"
    mv "${PROJECT_ROOT}/../"*.deb "${DEB_OUTPUT}/" || true
    mv "${PROJECT_ROOT}/../"*.changes "${DEB_OUTPUT}/" 2>/dev/null || true
    mv "${PROJECT_ROOT}/../"*.buildinfo "${DEB_OUTPUT}/" 2>/dev/null || true
    echo "Debian packages created in ${DEB_OUTPUT}/"
    ls -lh "${DEB_OUTPUT}/"*.deb
else
    echo "Error: Debian package build failed."
    exit 1
fi

if [[ ${TEMP_DEBIAN_DIR} -eq 1 ]]; then
    rm -rf "${PROJECT_ROOT}/debian"
fi
