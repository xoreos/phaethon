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

# UTF8-CPP (<https://github.com/nemtrif/utfcpp>).

noinst_HEADERS += \
    external/utf8cpp/utf8.h \
    external/utf8cpp/utf8/checked.h \
    external/utf8cpp/utf8/core.h \
    external/utf8cpp/utf8/cpp11.h \
    external/utf8cpp/utf8/cpp17.h \
    external/utf8cpp/utf8/unchecked.h \
    $(EMPTY)

EXTRA_DIST += \
    external/utf8cpp/LICENSE \
    external/utf8cpp/README.Phaethon \
    $(EMPTY)
