SUMMARY  = "Full Cmake/Boost Embedded Automotive Demo"
HOMEPAGE = "https://github.com/mlahmadix/Demo"
SECTION  = "core"
DEPENDS  = "boost"
LICENSE  = "MIT"

LIC_FILES_CHKSUM = "file://LICENSE;md5=d65c2cc1dd6bf52773770354f45cfcca"


SRCREV = "27c5f1dc22cb09e6f164dccd4cb36936993f52af"
SRC_URI = "git://github.com/mlahmadix/Demo.git;protocol=https"

S = "${WORKDIR}/git"

inherit cmake

EXTRA_OECMAKE="-DLOGDEBUG=ON -DCANDATALOGGER=ON -DNMEAPDEBUG=ON"

do_install() {
	install -d ${D}${bindir}
	install -m 0755 HelloWorld ${D}${bindir}/HelloWorld
}
