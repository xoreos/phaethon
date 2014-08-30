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

/** @file common/filetree.cpp
 *  A tree structure of files in directories.
 */

#include "common/filetree.h"
#include "common/filepath.h"
#include "common/error.h"

namespace Common {

FileTree::Entry::Entry() {
}

FileTree::Entry::Entry(const boost::filesystem::path &p) : name(p.filename().c_str()), path(p) {
}


FileTree::FileTree() {
}

FileTree::~FileTree() {
}

void FileTree::clear() {
	_root.name.clear();
	_root.path.clear();
	_root.children.clear();
}

bool FileTree::isEmpty() const {
	return _root.name.empty() && _root.path.empty();
}

const FileTree::Entry &FileTree::getRoot() const {
	return _root;
}

void FileTree::readPath(const Common::UString &path, int recurseDepth) {
	return readPath(boost::filesystem::path(path.c_str()), recurseDepth);
}

void FileTree::readPath(boost::filesystem::path path, int recurseDepth) {
	clear();

	// The path needs to exist
	if (!boost::filesystem::exists(path))
		throw Common::Exception("Path \"%s\" does not exist", path.c_str());

	path = FilePath::normalize(path);

	_root.name = path.filename().c_str();
	_root.path = path;

	// If we can't or shouldn't recurse, we're done
	if (boost::filesystem::is_regular_file(path) || (recurseDepth == 0))
		return;

	addPath(_root, path, (recurseDepth == -1) ? -1 : (recurseDepth - 1));
}

void FileTree::addPath(Entry &entry, const boost::filesystem::path &path, int recurseDepth) {
	try {
		// Iterator over the directory's contents
		boost::filesystem::directory_iterator itEnd;
		for (boost::filesystem::directory_iterator itDir(path); itDir != itEnd; ++itDir) {
			// Add the file/directory to the entry's children
			entry.children.push_back(Entry(itDir->path()));

			// Recurse into directory until the depth limit is reached
			if (is_directory(itDir->status()))
				if (recurseDepth != 0)
					addPath(entry.children.back(), itDir->path(), (recurseDepth == -1) ? -1 : (recurseDepth - 1));
		}
	} catch (Exception &e) {
		e.add("Failed to read path \"%s\"", path.c_str());

		throw;
	} catch (std::exception &e) {
		Exception se(e);

		se.add("Failed to read path \"%s\"", path.c_str());
		throw se;
	}
}

} // End of namespace Common
