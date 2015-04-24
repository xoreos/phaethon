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
 *  A tree structure of files in directories.
 */

#ifndef COMMON_FILETREE_H
#define COMMON_FILETREE_H

#include <boost/filesystem.hpp>

#include <list>

#include "common/ustring.h"

namespace Common {

/** A tree structure of files in directories. */
class FileTree {
public:
	/** An entry in the file tree. */
	struct Entry {
		/** The name of a file or directory. */
		UString name;
		/** The full normalized path of the file or directory. */
		boost::filesystem::path path;

		/** The files and directories inside this directory entry. */
		std::list<Entry> children;

		Entry();
		Entry(const boost::filesystem::path &p);

		bool isDirectory() const;
	};

	FileTree();
	~FileTree();

	/** Clear the tree. */
	void clear();

	/** Is the tree empty? */
	bool isEmpty() const;

	/** Return the root node. */
	const Entry &getRoot() const;

	/** Fill the tree with this path.
	 *
	 *  @param  p The path to read.
	 *  @param  recurseDepth The number of levels to recurse into subdirectories.
	 *                       If 0, only one entry, this path, is added.
	 *                       If -1, the recursion is limitless.
	 *  @return true if the path was successfully read, false otherwise.
	 */
	void readPath(boost::filesystem::path path, int recurseDepth = 0);

	/** Fill the tree with this path.
	 *
	 *  @param  p The path to read.
	 *  @param  recurseDepth The number of levels to recurse into subdirectories.
	 *                       If 0, only one entry, this path, is added.
	 *                       If -1, the recursion is limitless.
	 *  @return true if the path was successfully read, false otherwise.
	 */
	void readPath(const Common::UString &path, int recurseDepth = 0);

private:
	Entry _root;

	void addPath(Entry &entry, const boost::filesystem::path &path, int recurseDepth);
};

} // End of namespace Common

#endif // COMMON_FILETREE_H
