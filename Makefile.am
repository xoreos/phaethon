# xoreos - A reimplementation of BioWare's Aurora engine
#
# xoreos is the legal property of its developers, whose names
# can be found in the AUTHORS file distributed with this source
# distribution.
#
# xoreos is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 3
# of the License, or (at your option) any later version.
#
# xoreos is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with xoreos. If not, see <http://www.gnu.org/licenses/>.

ACLOCAL_AMFLAGS = -I m4 --install

EXTRA_DIST = \
             autogen.sh \
             BUILDDEP.md \
             Doxyfile \
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
