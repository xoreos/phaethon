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
 *  Data files containing resources indexed in BioWare's KEY files.
 */

#ifndef AURORA_KEYDATAFILE_H
#define AURORA_KEYDATAFILE_H

#include <vector>

#include "common/types.h"

#include "aurora/types.h"

namespace Common {
	class SeekableReadStream;
}

namespace Aurora {

/** Class holding resource data indexed in BioWare KEY files. */
class KEYDataFile {
public:
	KEYDataFile();
	virtual ~KEYDataFile();

	/** Return the type of a resource. */
	FileType getResourceType(uint32 index) const;

	/** Return the size of a resource. */
	uint32 getResourceSize(uint32 index) const;

	/** Return a stream of the resource's contents. */
	virtual Common::SeekableReadStream *getResource(uint32 index) const = 0;

protected:
	/** Resource information. */
	struct Resource {
		FileType type; ///< The resource's type.

		uint32 offset; ///< The offset of the resource within the BIF.
		uint32 size;   ///< The resource's size.

		uint32 packedSize; ///< Raw, uncompressed data size.
	};

	typedef std::vector<Resource> ResourceList;

	/** External list of resource names and types. */
	ResourceList _resources;

	const Resource &getRes(uint32 index) const;
};

} // End of namespace Aurora

#endif // AURORA_KEYDATAFILE_H
