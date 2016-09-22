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

# Main Phaethon entry point.

bin_PROGRAMS += src/phaethon
src_phaethon_SOURCES =

src_phaethon_SOURCES += \
    src/cline.h \
    $(EMPTY)

src_phaethon_SOURCES += \
    src/cline.cpp \
    src/phaethon.cpp \
    $(EMPTY)

src_phaethon_LDADD = \
    src/gui/libgui.la \
    src/sound/libsound.la \
    src/images/libimages.la \
    src/aurora/libaurora.la \
    src/common/libcommon.la \
    src/version/libversion.la \
    $(LDADD) \
    $(EMPTY)

# Subdirectories

include src/version/rules.mk
include src/common/rules.mk
include src/aurora/rules.mk
include src/images/rules.mk
include src/sound/rules.mk
include src/gui/rules.mk
