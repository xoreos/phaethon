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

/** @file aurora/keyfile.h
 *  Handling BioWare's KEYs (resource index files).
 */

#ifndef AURORA_KEYFILE_H
#define AURORA_KEYFILE_H

#include <vector>

#include "common/types.h"
#include "common/ustring.h"

#include "aurora/types.h"
#include "aurora/aurorafile.h"
#include "aurora/archive.h"

namespace Common {
	class SeekableReadStream;
}

namespace Aurora {

class KEYFile : public Archive, public AuroraBase {
public:
	KEYFile(const Common::UString &fileName);
	~KEYFile();

	/** Clear the resource list. */
	void clear();

	/** Return the list of resources. */
	const ResourceList &getResources() const;

	/** Return the size of a resource. */
	uint32 getResourceSize(uint32 index) const;

	/** Return a stream of the resource's contents. */
	Common::SeekableReadStream *getResource(uint32 index) const;

private:
	/** Internal resource information. */
	struct IResource {
		uint32 bifIndex; ///< Index into the bif list.
		uint32 resIndex; ///< Index into the bif's resource table.
	};

	typedef std::vector<IResource> IResourceList;

	typedef std::vector<Common::UString> BIFList;

	/** External list of resource names and types. */
	ResourceList _resources;

	/** Internal list of resource BIF indices. */
	IResourceList _iResources;

	/** All managed bifs. */
	BIFList _bifs;

	void load(Common::SeekableReadStream &key);

	void readBIFList(Common::SeekableReadStream &key, uint32 offset);
	void readResList(Common::SeekableReadStream &key, uint32 offset);
};

} // End of namespace Aurora

#endif // AURORA_KEYFILE_H
