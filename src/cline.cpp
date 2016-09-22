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
 *  Command line handling.
 */

#include "src/version/version.h"

#include "src/cline.h"

Job parseCommandLine(const std::vector<Common::UString> &argv) {
	Job job;

	// No options at all means we operate on an empty path
	job.operation = kOperationPath;

	// Go through all arguments
	for (size_t i = 1; i < argv.size(); i++) {
		// Find --help and --version
		if        ((argv[i] == Common::UString("-h")) || (argv[i] == Common::UString("--help"))) {
			job.operation = kOperationHelp;
			break;
		} else if ((argv[i] == Common::UString("-v")) || (argv[i] == Common::UString("--version"))) {
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

Common::UString createVersionText() {
	Common::UString text;

	text += Common::UString::format("%s\n", Version::getProjectNameVersionFull());
	text += Common::UString::format("%s\n", Version::getProjectURL());
	text += Common::UString::format("\n");
	text += Common::UString::format("%s", Version::getProjectAuthors());

	return text;
}

Common::UString createHelpText(const Common::UString &name) {
	Common::UString text;

	text += Common::UString::format("%s - A FLOSS resource explorer for BioWare's Aurora engine games\n",
	                                Version::getProjectName());
	text += Common::UString::format("Usage: %s [options] [<path>]\n", name.c_str());
	text += Common::UString::format("  -h      --help              Display this text and exit.\n");
	text += Common::UString::format("  -v      --version           Display version information and exit.");

	return text;
}
