#!/usr/bin/env -S bash ../.port_include.sh
port=ninja
version=1.8.2
files="https://github.com/ninja-build/ninja/archive/v${version}.tar.gz ninja-v${version}.tar.gz 5fdb04461cc7f5d02536b3bfc0300166"
auth_type=md5

build() {
    CXXFLAGS="--sysroot=${SERENITY_BUILD_DIR}/Root" \
    LDFLAGS="--sysroot=${SERENITY_BUILD_DIR}/Root" \
    # platform=linux is close enough.
    run ./configure.py --bootstrap --platform=linux
    run strip ninja
}

install() {
    run mkdir -p "${SERENITY_BUILD_DIR}/Root/usr/local/bin"
    run cp ninja "${SERENITY_BUILD_DIR}/Root/usr/local/bin/ninja"
}
