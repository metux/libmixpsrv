
DESCRIPTION="A small 9P server library"
HOMEPAGE="http://j.metux.de/index.php?option=com_content&task=view&id=51"
SRC_URI="http://releases.metux.de/${PN}/${PN}-${PV}.tar.bz2"

LICENSE="LGPL"
SLOT="0"
KEYWORDS="~alpha ~amd64 ~hppa ~ia64 ~mips ~ppc ~ppc64 ~s390 ~sparc x86"
IUSE=""

DEPEND="net-libs/libmixp"

src_compile() {
	emake || die "build failed"
}

src_install() {
	emake DESTDIR="${D}" install || die
	dodoc ChangeLog README
}
