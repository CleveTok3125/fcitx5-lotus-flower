pkgname=fcitx5-lotus-flower
pkgver=3.1.0
pkgrel=1
pkgdesc="A fork of fcitx5-lotus - Vietnamese input method for fcitx5"
arch=('x86_64')
license=('GPL-3.0-or-later')
url="https://github.com/CleveTok3125/fcitx5-lotus-flower"

provides=(
    'fcitx5-lotus'
)

conflicts=(
    'fcitx5-lotus'
    'fcitx5-lotus-git'
    'fcitx5-lotus-bin'
    'fcitx5-lotus-openrc'
    'fcitx5-lotus-runit'
    'fcitx5-lotus-openrc-bin'
    'fcitx5-lotus-runit-bin'
)

depends=(
    'fcitx5'
    'libinput'
    'hicolor-icon-theme'
    'glibc'
    'libstdc++'
    'libgcc'
    'libudev.so'
    'python-qtpy'
    'python-dbus'
)

makedepends=(
    'cmake'
    'go'
    'extra-cmake-modules'
    'gcc'
    'python'
    'libx11'
)

build() {
    cd "$startdir"

    cmake -B build -S . \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=/usr \
        -DCMAKE_INSTALL_LIBDIR=/usr/lib \
        -DINSTALL_OPENRC=ON \
        -DINSTALL_RUNIT=ON \
        -DRUNIT_SV_DIR=/etc/runit/sv

    make -C build
}

package() {
    cd "$startdir"

    make -C build DESTDIR="$pkgdir" install
}
