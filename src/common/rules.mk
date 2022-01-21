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

# Common support code used all over the codebase.

noinst_LTLIBRARIES += src/common/libcommon.la
src_common_libcommon_la_SOURCES =

src_common_libcommon_la_SOURCES += \
    src/common/endianness.h \
    src/common/deallocator.h \
    src/common/disposableptr.h \
    src/common/system.h \
    src/common/fallthrough.h \
    src/common/types.h \
    src/common/util.h \
    src/common/strutil.h \
    src/common/encoding.h \
    src/common/platform.h \
    src/common/readstream.h \
    src/common/memreadstream.h \
    src/common/writestream.h \
    src/common/memwritestream.h \
    src/common/maths.h \
    src/common/singleton.h \
    src/common/error.h \
    src/common/ustring.h \
    src/common/hash.h \
    src/common/md5.h \
    src/common/blowfish.h \
    src/common/deflate.h \
    src/common/lzma.h \
    src/common/readfile.h \
    src/common/writefile.h \
    src/common/filepath.h \
    src/common/filelist.h \
    src/common/filetree.h \
    src/common/zipfile.h \
    src/common/bitstream.h \
    src/common/huffman.h \
    src/common/sinewindows.h \
    src/common/cosinetables.h \
    src/common/fft.h \
    src/common/mdct.h \
    src/common/mutex.h \
    src/common/thread.h \
    src/common/binsearch.h \
    src/common/streamtokenizer.h \
    src/common/string.h \
    src/common/lzx.h \
    $(EMPTY)

src_common_libcommon_la_SOURCES += \
    src/common/util.cpp \
    src/common/strutil.cpp \
    src/common/encoding.cpp \
    src/common/platform.cpp \
    src/common/readstream.cpp \
    src/common/memreadstream.cpp \
    src/common/writestream.cpp \
    src/common/memwritestream.cpp \
    src/common/maths.cpp \
    src/common/md5.cpp \
    src/common/blowfish.cpp \
    src/common/deflate.cpp \
    src/common/lzma.cpp \
    src/common/error.cpp \
    src/common/ustring.cpp \
    src/common/readfile.cpp \
    src/common/writefile.cpp \
    src/common/filepath.cpp \
    src/common/filelist.cpp \
    src/common/filetree.cpp \
    src/common/zipfile.cpp \
    src/common/huffman.cpp \
    src/common/sinewindows.cpp \
    src/common/cosinetables.cpp \
    src/common/fft.cpp \
    src/common/mdct.cpp \
    src/common/thread.cpp \
    src/common/streamtokenizer.cpp \
    src/common/string.cpp \
    src/common/lzx.cpp \
    $(EMPTY)

src_common_libcommon_la_LIBADD = \
    external/mspack/libmspack.la \
    $(EMPTY)
