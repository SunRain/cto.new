#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"

PREFIX=${PREFIX:-/usr}
LIBEXEC_DIR=${LIBEXEC_DIR:-"${PREFIX}/libexec"}
SYSTEMD_DIR=${SYSTEMD_DIR:-/etc/systemd/system}
POLKIT_DIR=${POLKIT_DIR:-/usr/share/polkit-1/actions}
UDEV_DIR=${UDEV_DIR:-/etc/udev/rules.d}
BUILD_DIR=${BUILD_DIR:-"${PROJECT_ROOT}/build"}
ENABLE_SERVICE=${ENABLE_SERVICE:-1}

DAEMON_BINARY_SOURCE="${BUILD_DIR}/libexec/gameperf-daemon"
SERVICE_SOURCE="${PROJECT_ROOT}/systemd/gameperf-daemon.service"
POLICY_SOURCE="${PROJECT_ROOT}/polkit/org.gameperf.daemon.policy"
UDEV_SOURCE="${PROJECT_ROOT}/udev/99-gameperf.rules"

if [[ ! -x "${DAEMON_BINARY_SOURCE}" ]]; then
    echo "Daemon binary not found at ${DAEMON_BINARY_SOURCE}." >&2
    echo "Please build the project (cmake --build ${BUILD_DIR}) before installing." >&2
    exit 1
fi

sudo install -Dm755 "${DAEMON_BINARY_SOURCE}" "${LIBEXEC_DIR}/gameperf-daemon"
sudo install -Dm644 "${SERVICE_SOURCE}" "${SYSTEMD_DIR}/gameperf-daemon.service"
sudo install -Dm644 "${POLICY_SOURCE}" "${POLKIT_DIR}/org.gameperf.daemon.policy"
sudo install -Dm644 "${UDEV_SOURCE}" "${UDEV_DIR}/99-gameperf.rules"

# Update daemon path in service if a custom LIBEXEC_DIR is used
sudo sed -i "s|ExecStart=.*|ExecStart=${LIBEXEC_DIR}/gameperf-daemon|" "${SYSTEMD_DIR}/gameperf-daemon.service"

sudo udevadm control --reload-rules || true
sudo udevadm trigger || true

sudo systemctl daemon-reload

if [[ "${ENABLE_SERVICE}" -eq 1 ]]; then
    sudo systemctl enable --now gameperf-daemon.service || true
    echo "GamePerf daemon service enabled and started."
else
    echo "GamePerf daemon installed. Enable it with: sudo systemctl enable --now gameperf-daemon.service"
fi
