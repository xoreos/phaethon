AC_PREREQ([2.65])

AC_INIT([Phaethon],[0.0.0])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([build-aux])

AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE([1.11 -Wall -Wno-portability no-dist-gzip dist-xz subdir-objects])
# Support silent build rules. Disable
# by either passing --disable-silent-rules to configure or passing V=1
# to make
AM_SILENT_RULES([yes])

dnl If AM_PROG_AR exists, call it, to shut up a libtool warning (and make it more portable, I guess?)
m4_ifdef([AM_PROG_AR], [AM_PROG_AR])

dnl libtool
LT_PREREQ([2.2])
LT_INIT([disable-shared])

dnl We're C++
AC_PROG_CXX
AM_PROG_CC_C_O
dnl We want a make install
AC_PROG_INSTALL

dnl --with-werror
AC_ARG_WITH([werror], [AS_HELP_STRING([--with-werror], [Compile with -Werror @<:@default=no@:>@])], [], [with_werror=no])
if test "x$with_werror" = "xyes"; then
	WERROR="-Werror -Werror=unused-but-set-variable"
fi

dnl Standard C, C++
AC_C_CONST
AC_HEADER_STDC

dnl Endianness
AC_C_BIGENDIAN()

dnl Special variables of the size of pointers
AC_TYPE_INTPTR_T
AC_TYPE_UINTPTR_T

dnl math library and special float functions
AC_CHECK_LIB([m], [cos])
AC_CHECK_FUNCS([fabs])
AC_CHECK_FUNCS([fmin])
AC_CHECK_FUNCS([fmax])
AC_CHECK_FUNCS([fabsf])
AC_CHECK_FUNCS([fminf])
AC_CHECK_FUNCS([fmaxf])

dnl General purpose libraries
AX_CHECK_ICONV( , AC_MSG_ERROR([No useable iconv() function found!]))
AX_CHECK_ZLIB(1, 2, 3, 4, , AC_MSG_ERROR([zlib(>= 1.2.3.4) is required and could not be found!]))
AX_CHECK_LZMA(5, 0, 5, 2, , AC_MSG_ERROR([liblzma(>= 5.0.5) is required and could not be found!]))

dnl Graphic libraries
AX_CHECK_WX(3, 0, 0, , AC_MSG_ERROR([wxWidgets (>= 3.0.0) is required and could not be found!]))

dnl Sound libraries
AX_CHECK_AL( , AC_MSG_ERROR([OpenAL or OpenAL Soft (>= 1.12) is required and could not be found!]))
AX_CHECK_MAD(0, 15, 1, , AC_MSG_ERROR([libMAD (>= 0.15.1) is required and could not be found!]))
AX_CHECK_OGG(, AC_MSG_ERROR([libOgg (>= 1.2.0) is required and could not be found!]))
AX_CHECK_VORBIS(, AC_MSG_ERROR([libVorbisFile (>= 1.3.1) is required and could not be found!]))

dnl Boost
BOOST_REQUIRE(1.48.0)
BOOST_SYSTEM
BOOST_FILESYSTEM
BOOST_STRING_ALGO
BOOST_REGEX
BOOST_UNORDERED
BOOST_HASH
BOOST_DATE_TIME
BOOST_CHRONO
BOOST_ATOMIC
BOOST_THREAD

dnl Extra flags
case "$target" in
	*darwin*)
		PHAETHON_CFLAGS="-DUNIX -DMACOSX"
		PHAETHON_LIBS=""
		;;
	*mingw*)
		PHAETHON_CFLAGS=""
		PHAETHON_LIBS="-static-libgcc -static-libstdc++"
		;;
	*)
		PHAETHON_CFLAGS="-DUNIX"
		PHAETHON_LIBS=""
		;;
esac;

AC_SUBST(PHAETHON_CFLAGS)
AC_SUBST(PHAETHON_LIBS)

AC_SUBST(WERROR)

AC_CONFIG_FILES([gitstamp/Makefile])
AC_CONFIG_FILES([utf8cpp/Makefile])
AC_CONFIG_FILES([src/common/Makefile])
AC_CONFIG_FILES([src/sound/Makefile])
AC_CONFIG_FILES([src/sound/decoders/Makefile])
AC_CONFIG_FILES([src/aurora/Makefile])
AC_CONFIG_FILES([src/Makefile])
AC_CONFIG_FILES([Makefile])

AC_OUTPUT
