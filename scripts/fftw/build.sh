#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)
DEPS_DIR="${ROOT_DIR}/.deps/fftw"
DOWNLOADS_DIR="${DEPS_DIR}/downloads"
SRC_PARENT="${DEPS_DIR}/src"
BUILD_DIR="${DEPS_DIR}/build"
INSTALL_DIR="${DEPS_DIR}/install"
BIN_DIR="${DEPS_DIR}/bin"

VERSION="3.3.10"
TARBALL="fftw-${VERSION}.tar.gz"
URL="https://www.fftw.org/${TARBALL}"
SHA256="56c932549852cddcfafdab3820b0200c7742675be92179e59e6215b340e26467"
SRC_DIR="${SRC_PARENT}/fftw-${VERSION}"

mkdir -p "${DOWNLOADS_DIR}" "${SRC_PARENT}" "${BUILD_DIR}" "${INSTALL_DIR}" "${BIN_DIR}"

if [[ ! -f "${DOWNLOADS_DIR}/${TARBALL}" ]]; then
  echo "Downloading FFTW ${VERSION}..."
  curl -L -o "${DOWNLOADS_DIR}/${TARBALL}" "${URL}"
fi

if command -v shasum >/dev/null 2>&1; then
  ACTUAL_SHA=$(shasum -a 256 "${DOWNLOADS_DIR}/${TARBALL}" | awk '{print $1}')
elif command -v sha256sum >/dev/null 2>&1; then
  ACTUAL_SHA=$(sha256sum "${DOWNLOADS_DIR}/${TARBALL}" | awk '{print $1}')
else
  echo "Missing sha256 tool (shasum/sha256sum)." >&2
  exit 1
fi

if [[ "${ACTUAL_SHA}" != "${SHA256}" ]]; then
  echo "SHA256 mismatch for ${TARBALL}" >&2
  echo "Expected: ${SHA256}" >&2
  echo "Actual:   ${ACTUAL_SHA}" >&2
  exit 1
fi

if [[ ! -d "${SRC_DIR}" ]]; then
  echo "Extracting ${TARBALL}..."
  tar -xzf "${DOWNLOADS_DIR}/${TARBALL}" -C "${SRC_PARENT}"
fi

if [[ ! -f "${BUILD_DIR}/Makefile" ]]; then
  echo "Configuring FFTW..."
  (cd "${BUILD_DIR}" && "${SRC_DIR}/configure" \
    --prefix="${INSTALL_DIR}" \
    --enable-shared \
    --disable-dependency-tracking \
    --disable-fortran)
fi

JOBS=1
if command -v nproc >/dev/null 2>&1; then
  JOBS=$(nproc)
elif command -v sysctl >/dev/null 2>&1; then
  JOBS=$(sysctl -n hw.ncpu)
fi

echo "Building FFTW (jobs=${JOBS})..."
make -C "${BUILD_DIR}" -j"${JOBS}"
make -C "${BUILD_DIR}" install

echo "Building FFTW oracle..."
cc -O2 -std=c11 -Wall -Wextra \
  -I"${INSTALL_DIR}/include" \
  "${ROOT_DIR}/scripts/fftw/oracle.c" \
  -L"${INSTALL_DIR}/lib" -lfftw3 \
  -Wl,-rpath,"${INSTALL_DIR}/lib" \
  -o "${BIN_DIR}/fftw-oracle"

echo "Done. Oracle: ${BIN_DIR}/fftw-oracle"
