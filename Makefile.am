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

# The main "controller" automake recipe. We're setting up all our
# global build variables here, then recursively include all other
# build recipe, creating a flat non-recursive automake setup.

# Use the m4 directory for extra m4 autoconf scripts.
ACLOCAL_AMFLAGS = -I m4 --install

# Empty element to make the source lists easier editable.
EMPTY =

# Initialize the global lists.

noinst_HEADERS     =
noinst_LTLIBRARIES =

bin_PROGRAMS =

check_LTLIBRARIES =
check_PROGRAMS    =
TESTS             =

CLEANFILES =

EXTRA_DIST     =
dist_doc_DATA  =
dist_man1_MANS =

# Library compile flags

LIBSF_PHAETHON = $(PHAETHON_CFLAGS)
LIBSF_GENERAL  = $(PTHREAD_CFLAGS) $(ZLIB_CFLAGS) $(LZMA_FLAGS)
LIBSF_GRAPHIC  = $(QT5_CFLAGS)
LIBSF_SOUND    = $(AL_CFLAGS) $(MAD_CFLAGS) $(OGG_CFLAGS) $(VORBIS_CFLAGS)
LIBSF_BOOST    = $(BOOST_CPPFLAGS)

LIBSF          = $(LIBSF_PHAETHON) $(LIBSF_GENERAL) $(LIBSF_GRAPHIC) \
                 $(LIBSF_SOUND) $(LIBSF_BOOST)

# Library linking flags

LIBSL_PHAETHON = $(PHAETHON_LIBS)
LIBSL_GENERAL  = $(PTHREAD_LIBS) $(LTLIBICONV) $(ZLIB_LIBS) $(LZMA_LIBS)
LIBSL_GRAPHIC  = $(QT5_LIBS)
LIBSL_SOUND    = $(AL_LIBS) $(MAD_LIBS) $(OGG_LIBS) $(VORBIS_LIBS)
LIBSL_BOOST    = $(BOOST_SYSTEM_LDFLAGS) $(BOOST_SYSTEM_LIBS) \
                 $(BOOST_FILESYSTEM_LDFLAGS) $(BOOST_FILESYSTEM_LIBS) \
                 $(BOOST_DATE_TIME_LDFLAGS) $(BOOST_DATE_TIME_LIBS) \
                 $(BOOST_CHRONO_LDFLAGS) $(BOOST_CHRONO_LIBS) \
                 $(BOOST_LOCALE_LDFLAGS) $(BOOST_LOCALE_LIBS) \
                 $(BOOST_THREAD_LDFLAGS) $(BOOST_THREAD_LIBS)
LIBSL          = $(LIBSL_PHAETHON) $(LIBSL_GENERAL) $(LIBSL_GRAPHIC) \
                 $(LIBSL_SOUND) $(LIBSL_BOOST)

# Other compiler flags

FLAGS_DIR      = -I$(srcdir)
FLAGS_DEBUG    = $(GGDB)
FLAGS_STD      = $(STD)
FLAGS_OPT      = $(LTO) $(NATIVE)
FLAGS_WARN     = $(WARN) $(WERROR)

FLAGS          = $(FLAGS_DIR) $(FLAGS_DEBUG) $(FLAGS_STD) \
                 $(FLAGS_OPT) $(FLAGS_WARN)

# Putting it all together...

AM_CXXFLAGS   = $(FLAGS) $(LIBSF)
AM_LDFLAGS    = $(FLAGS_OPT)
LDADD         = $(LIBSL)
LIBS          =

# Now include the top-level recipe file and through it, all the
# other build recipes.

include rules.mk
