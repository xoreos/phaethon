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

/** @file phaethon.cpp
 *  The project's main entry point.
 */

#include <cstdio>

#include <wx/wx.h>

#include "common/util.h"
#include "common/version.h"
#include "common/error.h"
#include "common/ustring.h"

struct Job;

void printUsage(const char *name);
void printVersion();

Job parseCommandLine(int argc, char **argv);

void openGamePath(const Common::UString &path);


/** Type for all operations this tool can do. */
enum Operation {
	kOperationInvalid = 0, ///< Invalid command line.
	kOperationHelp       , ///< Show the help text.
	kOperationVersion    , ///< Show version information.
	kOperationPath         ///< Crawl through a game directory.
};

/** Full description of the job this tool will be doing. */
struct Job {
	Operation operation;  ///< The operation to perform.
	Common::UString path; ///< The game directory to look through.

	Job() : operation(kOperationInvalid) {
	}
};


int main(int argc, char **argv) {
	// Find out what we're supposed to do
	Job job = parseCommandLine(argc, argv);

	try {
		// Handle the job
		switch (job.operation) {
			case kOperationHelp:
				printUsage(argv[0]);
				break;

			case kOperationVersion:
				printVersion();
				break;

			case kOperationPath:
				openGamePath(job.path);
				break;

			case kOperationInvalid:
			default:
				printUsage(argv[0]);
				return -1;
		}
	} catch (Common::Exception &e) {
		Common::printException(e);
		return -2;
	} catch (std::exception &e) {
		Common::Exception se(e);

		Common::printException(se);
		return -2;
	}

	return 0;
}

/** Print usage/help text. */
void printUsage(const char *name) {
	std::printf("%s - A FLOSS resource explorer for BioWare's Aurora engine games\n", PHAETHON_NAME);
	std::printf("Usage: %s [options] [<path>]\n", name);
	std::printf("  -h      --help              Display this text and exit.\n");
	std::printf("  -v      --version           Display version information and exit.\n");
}

/** Print the tool's version. */
void printVersion() {
	std::printf("%s\n", PHAETHON_NAMEVERSIONFULL);
	std::printf("%s\n", PHAETHON_URL);
	std::printf("Copyright (c) %s by the following:\n", PHAETHON_COPYRIGHTYEAR);
	std::printf("\n");
	std::printf("%s\n", PHAETHON_COPYRIGHTAUTHOR);
	std::printf("\n");
	std::printf("%s is free software; you can redistribute it and/or\n", PHAETHON_NAME);
	std::printf("modify it under the terms of the GNU General Public License\n");
	std::printf("as published by the Free Software Foundation; either version 3\n");
	std::printf("of the License, or (at your option) any later version.\n");
	std::printf("\n");
	std::printf("%s is distributed in the hope that it will be useful,\n", PHAETHON_NAME);
	std::printf("but WITHOUT ANY WARRANTY; without even the implied warranty of\n");
	std::printf("MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the\n");
	std::printf("GNU General Public License for more details.\n");
	std::printf("\n");
	std::printf("You should have received a copy of the GNU General Public License\n");
	std::printf("along with %s. If not, see <http://www.gnu.org/licenses/>.\n", PHAETHON_NAME);
	std::printf("\n");
	std::printf("The Infinity engine, Aurora engine, Odyssey engine and Eclipse engine are\n");
	std::printf("registered trademarks of BioWare Corp., a subsidiary of Electronic Arts, Inc.\n");
	std::printf("The Electron engine and Lycium engine are trademarks of BioWare Corp., a\n");
	std::printf("subsidiary of Electronic Arts, Inc. Bink is a registered trademark of RAD Game\n");
	std::printf("Tools, Inc. Neverwinter Nights is a registered trademark of Wizards of the\n");
	std::printf("Coast, a subsidiary of Hasbro Inc. Star Wars, Knights of the Old Republic and\n");
	std::printf("The Sith Lords are registered trademarks of Lucasfilm Entertainment Company\n");
	std::printf("Ltd., a subsidiary of The Walt Disney Company. Jade Empire is a registered\n");
	std::printf("trademark of BioWare Corp., a subsidiary of Electronic Arts, Inc. Sonic the\n");
	std::printf("Hedgehog is a registered trademark of Sega Corporation. The Witcher is a\n");
	std::printf("registered trademark of CD Projekt S.A. Dragon Age is a registered trademark\n");
	std::printf("of Electronic Arts, Inc. All rights reserved.\n");
}

Job parseCommandLine(int argc, char **argv) {
	Job job;

	// No options at all means we operate on an empty path
	job.operation = kOperationPath;

	// Go through all arguments
	for (int i = 1; i < argc; i++) {
		// Find --help and --version
		if        (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")) {
			job.operation = kOperationHelp;
			break;
		} else if (!strcmp(argv[i], "-v") || !strcmp(argv[i], "--version")) {
			job.operation = kOperationVersion;
			break;
		}

		// We only allow one path, so a second one makes the command line invalid
		if (!job.path.empty()) {
			job.operation = kOperationInvalid;
			break;
		}

		job.path = argv[i];
	}

	return job;
}


class Phaethon : public wxApp {
public:
	Phaethon(const Common::UString &path = "");
	~Phaethon();

	bool OnInit();

private:
	Common::UString _path;
};

Phaethon::Phaethon(const Common::UString &path) : _path(path) {
}

Phaethon::~Phaethon() {
}

bool Phaethon::OnInit() {
	return true;
}


void openGamePath(const Common::UString &path) {
	wxApp *phaethon = new Phaethon(path);

	wxApp::SetInstance(phaethon);

	int      argc = 0;
	wxChar **argv = 0;
	wxEntry(argc, argv);
}
