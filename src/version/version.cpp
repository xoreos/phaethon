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
 *  Basic Phaethon version information.
 */

#include "src/version/version.h"

#if defined(HAVE_CONFIG_H)
	#include "config.h"
#endif

// Define default values if the real ones can't be determined

#ifndef PACKAGE_NAME
	#define PACKAGE_NAME "Phaethon"
#endif

#ifndef PACKAGE_VERSION
	#define PACKAGE_VERSION "0.0.5"
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

namespace Version {

static const char *kProjectName            = PACKAGE_NAME;
static const char *kProjectVersion         = PACKAGE_VERSION;
static const char *kProjectNameVersion     = PACKAGE_NAME " " PACKAGE_VERSION PHAETHON_REVSEP PHAETHON_REV;
static const char *kProjectNameVersionFull = PACKAGE_NAME " " PACKAGE_VERSION PHAETHON_REVSEP PHAETHON_REV " [" PHAETHON_REVDESC "] (" PHAETHON_BUILDDATE ")";

static const char *kProjectURL = "https://xoreos.org/";

static const char *kProjectAuthors =
	"Copyright (c) 2014-2020 by the xoreos team.\n"
	"Please see the AUTHORS file for details.\n"
	"\n"
	"This is free software; see the source for copying conditions.  There is NO\n"
	"warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.";

const char *getProjectName() {
	return kProjectName;
}

const char *getProjectVersion() {
	return kProjectVersion;
}

const char *getProjectNameVersion() {
	return kProjectNameVersion;
}

const char *getProjectNameVersionFull() {
	return kProjectNameVersionFull;
}

const char *getProjectURL() {
	return kProjectURL;
}

const char *getProjectAuthors() {
	return kProjectAuthors;
}

} // End of namespace Version
