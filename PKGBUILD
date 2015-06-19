# Maintainer: Michael Baer <derbaer@clockwork-origins.de>
pkgname=clockutils
pkgver=0.3.0
pkgrel=1
pkgdesc="A lightweight c++ library for commonly needed tasks. Optimized for simplicity and speed."
arch=('i686', 'x86_64')
url="https://clockwork-origins.de/main.php"
license=('LGPL2.1')
depends=('gcc-libs>=4.8.2' 'glibc')
makedepends=('cmake')
source=(http://clockwork-origins.de/downloads/clockUtils/$pkgname-$pkgver-src.zip)
sha256sums=('SKIP') #autofill using updpkgsums

build() {
	cd "$pkgname-$pkgver-src"

	cmake -DCMAKE_INSTALLPREFIX=/usr .

	make
}

package() {
	cd "$pkgname-$pkgver-src"

	make DESTDIR="$pkgdir/" install
}
