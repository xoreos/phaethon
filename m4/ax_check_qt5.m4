#serial 1000
dnl @synopsis AX_CHECK_QT5(version, action-if, action-if-not)
dnl
dnl @summary check for Qt5 of sufficient version.
dnl
dnl Defines QT5_LIBS, QT5_CFLAGS
dnl
dnl @category InstalledPackages
dnl @author Sven Hesse <drmccoy@drmccoy.de>
dnl @version 2017-02-02
dnl @license Creative Commons CC0 1.0 Universal Public Domain Dedication

AC_DEFUN([AX_CHECK_QT5], [
	PKG_CHECK_MODULES([QT5], [Qt5Core >= $1 Qt5Gui >= $1 Qt5Widgets >= $1 Qt5Multimedia >= $1 Qt5Concurrent >= $1], [$2], [$3])

	# Qt5 needs -fPIC...
	QT5_CFLAGS="$QT5_CFLAGS -fPIC"

	AC_SUBST(QT5_CFLAGS)
	AC_SUBST(QT5_LIBS)
])
