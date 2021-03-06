#!/usr/bin/env -S bash ../.port_include.sh
port=libogg
version=1.3.4
useconfigure=true
files="https://github.com/xiph/ogg/releases/download/v${version}/libogg-${version}.tar.gz libogg-${version}.tar.gz b9a66c80bdf45363605e4aa75fa951a8"
auth_type=md5

install() {
    run make DESTDIR=$DESTDIR $installopts install
    ${CC} -shared -o $DESTDIR/usr/local/lib/libogg.so -Wl,--whole-archive $DESTDIR/usr/local/lib/libogg.a -Wl,--no-whole-archive
    rm -f $DESTDIR/usr/local/lib/libogg.la
}
