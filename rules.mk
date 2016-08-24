# Phaethon - A FLOSS resource explorer for BioWare's Aurora engine games
#
# Phaethon is the legal property of its developers, whose names
# can be found in the AUTHORS file distributed with this source
# distribution.
#
# Phaethon is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 3
# of the License, or (at your option) any later version.
#
# Phaethon is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Phaethon. If not, see <http://www.gnu.org/licenses/>.

# Top-level automake build recipe.

# Extra files to help with the sources
EXTRA_DIST += \
    BUILDDEP.md \
    .uncrustifyrc \
    $(EMPTY)

# Extra autotools files
EXTRA_DIST += \
    autogen.sh \
    $(EMPTY)

# Files for CMake, our alternative build system
EXTRA_DIST += \
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

# Licenses and credits
dist_doc_DATA += \
    COPYING \
    COPYING.GPLv2 \
    COPYING.LGPLv2.1 \
    AUTHORS \
    $(EMPTY)

# User documentation
dist_doc_DATA += \
    ChangeLog \
    TODO \
    README.md \
    $(EMPTY)

# Documents for contributors
dist_doc_DATA += \
    CONTRIBUTING.md \
    CODE_OF_CONDUCT.md \
    $(EMPTY)

# Doxygen

EXTRA_DIST += \
    Doxyfile \
    doc/doxygen/README \
    $(EMPTY)

doxygen:
	doxygen

doxygen-clean:
	rm -rf doc/doxygen/html/
	rm -rf doc/doxygen/latex/
	rm -rf doc/doxygen/man/
	rm -rf doc/doxygen/*.tmp
	rm -rf doc/doxygen/*.db

# Subdirectories

include dists/rules.mk

include utf8cpp/rules.mk

include src/rules.mk

include tests/rules.mk
