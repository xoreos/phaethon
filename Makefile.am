ACLOCAL_AMFLAGS = -I m4 --install

EXTRA_DIST = \
             autogen.sh \
             BUILDDEP.md \
             Doxyfile \
             README.dox \
             doc/doxygen/README \
             dists/arch/PKGBUILD \
             dists/debian/changelog \
             dists/debian/compat \
             dists/debian/control \
             dists/debian/copyright \
             dists/debian/phaethon.lintian-overrides \
             dists/debian/rules \
             dists/debian/source/format \
             .uncrustifyrc \
             CMakeLists.txt \
             cmake/CMakeAM.cmake \
             cmake/FindFaad.cmake \
             cmake/FindIconv.cmake \
             cmake/FindLibLZMA.cmake \
             cmake/FindMad.cmake \
             cmake/FindOgg.cmake \
             cmake/FindVorbis.cmake \
             cmake/SetCheckCompilerFlag.cmake \
             cmake/toolchain/i686-windows-mingw.cmake \
             cmake/toolchain/x86_64-windows-mingw.cmake \
             $(EMPTY)

dist_doc_DATA = \
                COPYING \
                AUTHORS \
                ChangeLog \
                TODO \
                README.md \
                CONTRIBUTING.md \
                CODE_OF_CONDUCT.md \
                $(EMPTY)

SUBDIRS = \
          gitstamp \
          utf8cpp \
          src \
          $(EMPTY)

doxygen:
	doxygen

doxygen-clean:
	rm -rf doc/doxygen/html/
	rm -rf doc/doxygen/latex/
	rm -rf doc/doxygen/man/
	rm -rf doc/doxygen/*.tmp
	rm -rf doc/doxygen/*.db
