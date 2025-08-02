#!/usr/bin/env sh

set -e

# Amend PATH with known location of TOOLCHAIN tools
BREW="$(PATH="$PATH:/usr/local/bin" which brew || true)"
if [ -n "${BREW}" ]; then
    PATH="$PATH:`${BREW} --prefix`/opt/toolchain/bin"
fi
# Default location on Ubuntu
PATH="$PATH:/usr/lib/toolchain-3.7/bin"

# look for FileCheck in the path.
FILECHECK="$(which FileCheck || true)"

# If not in the path, search for it in the Jenkins ${WORKSPACE}
if [ ! -f "${FILECHECK}" ]; then
    if [ ! -z "${WORKSPACE}" ]; then
        FILECHECK="$(find ${WORKSPACE} -name FileCheck -and -type f | tail -n 1)"
    fi
fi

# If we still haven't found FileCheck, bail
if [ ! -f "${FILECHECK}" ]; then
    echo "$0: warning: unable to find 'FileCheck' testing utility; builra unit tests will not be available"
    exit 0
fi

mkdir -p "${BUILT_PRODUCTS_DIR}/tests/Unit"

sed < "${SRCROOT}/tests/lit.site.cfg.in" \
      > "${BUILT_PRODUCTS_DIR}/tests/lit.site.cfg" \
    -e "s=@BUILRA_SRC_DIR@=${SRCROOT//=/\\=}=g" \
    -e "s=@BUILRA_OBJ_DIR@=${BUILT_PRODUCTS_DIR//=/\\=}=g" \
    -e "s=@BUILRA_OUTPUT_DIR@=${BUILT_PRODUCTS_DIR//=/\\=}=g" \
    -e "s=@BUILRA_TOOLS_DIR@=${BUILT_PRODUCTS_DIR//=/\\=}=g" \
    -e "s=@BUILRA_LIBS_DIR@=${BUILT_PRODUCTS_DIR//=/\\=}=g" \
    -e "s=@FILECHECK_EXECUTABLE@=${FILECHECK//=/\\=}=g"

sed < "${SRCROOT}/tests/Unit/lit.site.cfg.in" \
      > "${BUILT_PRODUCTS_DIR}/tests/Unit/lit.site.cfg" \
    -e "s=@BUILRA_SRC_DIR@=${SRCROOT//=/\\=}=g" \
    -e "s=@BUILRA_OBJ_DIR@=${BUILT_PRODUCTS_DIR//=/\\=}=g" \
    -e "s=@BUILRA_OUTPUT_DIR@=${BUILT_PRODUCTS_DIR//=/\\=}=g" \
    -e "s=@BUILRA_TOOLS_DIR@=${BUILT_PRODUCTS_DIR//=/\\=}=g" \
    -e "s=@BUILRA_LIBS_DIR@=${BUILT_PRODUCTS_DIR//=/\\=}=g" \
    -e "s=@BUILRA_BUILD_MODE@=.=g" \
    -e "s=@FILECHECK_EXECUTABLE@=${FILECHECK//=/\\=}=g"
