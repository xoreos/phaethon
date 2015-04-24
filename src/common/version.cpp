/* Phaethon - A FLOSS resource explorer for BioWare's Aurora engine games
 *
 * Phaethon is the legal property of its developers, whose names
 * can be found in the AUTHORS file distributed with this source
 * distribution.
 *
 * Phaethon is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * Phaethon is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Phaethon. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file
 *  Basic Phaethon version information
 */

#include "src/common/version.h"

#if defined(HAVE_CONFIG_H)
	#include "config.h"
#endif

// Define default values if the real ones can't be determined

#ifndef PACKAGE_NAME
	#define PACKAGE_NAME "Phaethon"
#endif

#ifndef PACKAGE_VERSION
	#define PACKAGE_VERSION "0.0.0"
#endif

#ifndef PHAETHON_REVDESC
	#define PHAETHON_REVDESC "unknown"
#endif

#ifndef PHAETHON_REV
	#define PHAETHON_REV "unk"
#endif

#ifndef PHAETHON_BUILDDATE
	#define PHAETHON_BUILDDATE __DATE__ " " __TIME__
#endif

// If we're in full release mode, the revision is ignored
#ifdef PHAETHON_RELEASE
	#undef PHAETHON_REV
	#define PHAETHON_REV ""
	#define PHAETHON_REVSEP ""
#else
	#define PHAETHON_REVSEP "+"
#endif

// Distributions may append an extra version string
#ifdef PHAETHON_DISTRO
	#undef PHAETHON_REV
	#define PHAETHON_REV PHAETHON_DISTRO
#endif

const char *PHAETHON_NAME            = PACKAGE_NAME;
const char *PHAETHON_VERSION         = PACKAGE_VERSION;
const char *PHAETHON_NAMEVERSION     = PACKAGE_NAME " " PACKAGE_VERSION PHAETHON_REVSEP PHAETHON_REV;
const char *PHAETHON_NAMEVERSIONFULL = PACKAGE_NAME " " PACKAGE_VERSION PHAETHON_REVSEP PHAETHON_REV " [" PHAETHON_REVDESC "] (" PHAETHON_BUILDDATE ")";
const char *PHAETHON_COPYRIGHTYEAR   = "2014";
const char *PHAETHON_COPYRIGHTAUTHOR = "Sven Hesse (DrMcCoy) <drmccoy@drmccoy.de>\n"
                                       "Matthew Hoops (clone2727) <clone2727@gmail.com>";
const char *PHAETHON_URL             = "https://xoreos.org/";
