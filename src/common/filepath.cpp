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

/** @file common/filepath.cpp
 *  Utility class for manipulating file paths.
 */

#include <list>

#include <boost/algorithm/string.hpp>
#include <boost/system/config.hpp>
#include <boost/regex.hpp>

#include "common/filepath.h"
#include "common/util.h"

// boost-filesystem stuff
using boost::filesystem::path;
using boost::filesystem::exists;
using boost::filesystem::is_regular_file;
using boost::filesystem::is_directory;
using boost::filesystem::file_size;
using boost::filesystem::directory_iterator;
using boost::filesystem::absolute;
using boost::filesystem::canonical;

// boost-string_algo
using boost::equals;
using boost::iequals;

namespace Common {

bool FilePath::isRegularFile(const UString &p) {
	return (exists(p.c_str()) && is_regular_file(p.c_str()));
}

bool FilePath::isDirectory(const UString &p) {
	return (exists(p.c_str()) && is_directory(p.c_str()));
}

uint32 FilePath::getFileSize(const UString &p) {
	uintmax_t size = file_size(p.c_str());

	if (size == ((uintmax_t) -1))
		return kFileInvalid;

	return size;
}

UString FilePath::getFile(const UString &p) {
	path file(p.c_str());

	return file.filename().c_str();
}

UString FilePath::getStem(const UString &p) {
	path file(p.c_str());

	return file.stem().c_str();
}

UString FilePath::getExtension(const UString &p) {
	path file(p.c_str());

	return file.extension().c_str();
}

UString FilePath::changeExtension(const UString &p, const UString &ext) {
	path file(p.c_str());

	file.replace_extension(ext.c_str());

	return file.string();
}

path FilePath::normalize(const boost::filesystem::path &p) {
	if (!exists(p.c_str()))
		return path("");

	UString norm = canonical(p).c_str();

	norm.replaceAll('\\', '/');

	return path(norm.c_str());
}

UString FilePath::normalize(const UString &p) {
	return normalize(path(p.c_str())).c_str();
}

bool FilePath::isAbsolute(const UString &p) {
	return path(p.c_str()).has_root_directory();
}

UString FilePath::makeRelative(UString basePath, const UString &path) {
	if (basePath.empty())
		return path;

	if (!basePath.endsWith("/"))
		basePath += '/';

	if (!path.beginsWith(basePath))
		return "";

	return path.substr(path.getPosition(basePath.size()), path.end());
}

void FilePath::getSubDirectories(const UString &directory, std::list<UString> &subDirectories) {

	path dirPath(directory.c_str());

	// Iterator over the directory's contents
	directory_iterator itEnd;
	for (directory_iterator itDir(dirPath); itDir != itEnd; ++itDir) {
		if (is_directory(itDir->status())) {
			subDirectories.push_back(itDir->path().generic_string());
		}
	}
}

static void splitDirectories(const UString &directory, std::list<UString> &dirs) {
	UString curDir;

	for (UString::iterator it = directory.begin(); it != directory.end(); ++it) {
		uint32 c = *it;

		if (c == '/') {
			// Found a directory separator, split here

			// Got a real directory, add it to our list
			if (!curDir.empty())
				dirs.push_back(curDir);

			curDir.clear();

		} else
			// Otherwise, just append the current character
			curDir += c;

	}

	// Got trailing data, add it to our list
	if (!curDir.empty())
		dirs.push_back(curDir);
}

static UString findSubDirectory_internal(const UString &directory, const UString &subDirectory,
		bool caseInsensitive) {

	try {
		path dirPath(directory.c_str());
		path subDirPath(subDirectory.c_str());

		// Iterator over the directory's contents
		directory_iterator itEnd;
		for (directory_iterator itDir(dirPath); itDir != itEnd; ++itDir) {
			if (is_directory(itDir->status())) {
				// It's a directory. Check if it's the one we're looking for

				if (caseInsensitive) {
					if (iequals(itDir->path().filename().c_str(), subDirectory.c_str()))
						return itDir->path().generic_string();
				} else {
					if (equals(itDir->path().filename().c_str(), subDirectory.c_str()))
						return itDir->path().generic_string();
				}
			}
		}
	} catch (...) {
	}

	return "";
}

UString FilePath::findSubDirectory(const UString &directory, const UString &subDirectory,
		bool caseInsensitive) {

	if (!exists(directory.c_str()) || !is_directory(directory.c_str()))
		// Path is either no directory or doesn't exist
		return "";

	if (subDirectory.empty())
		// Subdirectory to look for is empty, return the directory instead
		return directory;

	// Split the subDirectory string into actual directories
	std::list<UString> dirs;
	splitDirectories(subDirectory, dirs);

	// Iterate over the directory list to find each successive subdirectory
	UString curDir = directory;
	for (std::list<UString>::iterator it = dirs.begin(); it != dirs.end(); ++it)
		if ((curDir = findSubDirectory_internal(curDir, *it, caseInsensitive)).empty())
			return "";

	return curDir;
}

UString FilePath::escapeStringLiteral(const UString &str) {
	const boost::regex esc("[\\^\\.\\$\\|\\(\\)\\[\\]\\*\\+\\?\\/\\\\]");
	const std::string  rep("\\\\\\1&");

	return boost::regex_replace(std::string(str.c_str()), esc, rep, boost::match_default | boost::format_sed);
}

UString FilePath::getHumanReadableSize(uint32 size) {
	static const char *sizes[] = {"B", "K", "M", "G"};

	double s = size;
	int n = 0;

	while ((s >= 1024) && ((n + 1) < ARRAYSIZE(sizes))) {
		n++;
		s /= 1024;
	}

	return UString::sprintf("%.2lf%s", s, sizes[n]);
}

} // End of namespace Common
