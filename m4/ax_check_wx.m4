#serial 1000
dnl @synopsis AX_CHECK_WX(major, minor, release, action-if, action-if-not)
dnl
dnl @summary check for wxWidgets of sufficient major, minor and release version
dnl
dnl Defines WX_LIBS, WX_CXXFLAGS.
dnl
dnl @category InstalledPackages
dnl @author Sven Hesse <drmccoy@drmccoy.de>
dnl @version 2013-12-14
dnl @license Creative Commons CC0 1.0 Universal Public Domain Dedication

AC_DEFUN([AX_CHECK_WX], [
	AC_ARG_VAR(WX_CONFIG, [wx-config utility])
	AC_ARG_VAR(WX_CPPFLAGS, [C++ preprocessor flags for wxWidgets])
	AC_ARG_VAR(WX_CXXFLAGS, [C++ compiler flags for wxWidgets])
	AC_ARG_VAR(WX_LIBS, [libraries to pass to the linker for wxWidgets])

	AC_CHECK_PROG([WX_CONFIG], [wx-config], [wx-config])

	if test -n "$WX_CONFIG"; then
		if test -z "$WX_CPPFLAGS"; then
			WX_CPPFLAGS=`$WX_CONFIG --cppflags`
		fi
		if test -z "$WX_CXXFLAGS"; then
			WX_CXXFLAGS=`$WX_CONFIG --cxxflags`
		fi
		if test -z "$WX_LIBS"; then
			WX_LIBS=`$WX_CONFIG --libs`
		fi
	fi

	save_CPPFLAGS="$CPPFLAGS"
	save_CXXFLAGS="$CXXFLAGS"
	save_LIBS="$LIBS"
	CPPFLAGS="$CPPFLAGS $WX_CPPFLAGS"
	CXXFLAGS="$CXXFLAGS $WX_CXXFLAGS"
	LIBS="$LIBS $WX_LIBS"

	AC_LANG_PUSH([C++])

	AC_CHECK_HEADER([wx/wx.h], AC_LINK_IFELSE([AC_LANG_SOURCE([[
#include <wx/wx.h>

int main(int argc, char **argv) {
#if !wxCHECK_VERSION($1, $2, $3)
	#error wxWidgets version too low
#endif

	return 0;
}
]])], nowx=0, nowx=1), nowx=1)

	AC_LANG_POP([C++])

	CPPFLAGS="$save_CPPFLAGS"
	CXXFLAGS="$save_CXXFLAGS"
	LIBS="$save_LIBS"

	if test "$nowx" = "1"; then
		ifelse([$5], , :, [$5])
	else
		ifelse([$4], , :, [$4])
	fi

	AC_SUBST(WX_CXXFLAGS)
	AC_SUBST(WX_LIBS)
])
