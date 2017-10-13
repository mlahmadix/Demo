SUMMARY  = "Full Cmake/Boost Embedded Demo"
HOMEPAGE = "https://github.com/mlahmadix/Demo"
SECTION  = "examples"
DEPENDS  = "boost"
LICENSE  = "MIT"

LIC_FILES_CHKSUM = "file://LICENSE;md5=d65c2cc1dd6bf52773770354f45cfcca"

SRCREV = "1c0d28591f29736dace401d4e1a42df7a733998d"
SRC_URI = "git://github.com/mlahmadix/Demo.git;protocol=https"

S = "${WORKDIR}/git"

inherit cmake

do_install() {
	install -d ${D}${bindir}
	install -m 0755 HelloWorld ${D}${bindir}/HelloWorld
}
