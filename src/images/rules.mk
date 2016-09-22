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

# Image loaders.

noinst_LTLIBRARIES += src/images/libimages.la
src_images_libimages_la_SOURCES =

src_images_libimages_la_SOURCES += \
    src/images/types.h \
    src/images/util.h \
    src/images/s3tc.h \
    src/images/decoder.h \
    src/images/dumptga.h \
    src/images/winiconimage.h \
    src/images/tga.h \
    src/images/dds.h \
    src/images/tpc.h \
    src/images/txb.h \
    src/images/sbm.h \
    $(EMPTY)

src_images_libimages_la_SOURCES += \
    src/images/s3tc.cpp \
    src/images/decoder.cpp \
    src/images/dumptga.cpp \
    src/images/winiconimage.cpp \
    src/images/tga.cpp \
    src/images/dds.cpp \
    src/images/tpc.cpp \
    src/images/txb.cpp \
    src/images/sbm.cpp \
    $(EMPTY)
