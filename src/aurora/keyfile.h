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
 *  Handling BioWare's KEYs (resource index files).
 */

#ifndef AURORA_KEYFILE_H
#define AURORA_KEYFILE_H

#include <vector>

#include "src/common/types.h"
#include "src/common/ustring.h"

#include "src/aurora/types.h"
#include "src/aurora/aurorafile.h"
#include "src/aurora/archive.h"

namespace Common {
	class SeekableReadStream;
}

namespace Aurora {

class KEYDataFile;

class KEYFile : public Archive, public AuroraBase {
public:
	KEYFile(const Common::UString &fileName);
	~KEYFile();

	/** Clear the resource list. */
	void clear();

	/** Add a data file that's managed by this KEY file. */
	void addDataFile(uint32 dataFileIndex, KEYDataFile *dataFile);

	/** Return the list of data files (BIF/BZF) this KEY file indexes. */
	const std::vector<Common::UString> &getDataFileList() const;

	/** Do we have a data file associated for this resource? */
	bool haveDataFile(uint32 index) const;

	/** Return the list of resources. */
	const ResourceList &getResources() const;

	/** Return the size of a resource.
	 *
	 *  Note: The size of the resource is stored in data file.
	 *        If the data files containing this resource's data
	 *        was not added first with addDataFile(), this method
	 *        will return 0xFFFFFFFF.
	 */
	uint32 getResourceSize(uint32 index) const;

	/** Return a stream of the resource's contents.
	 *
	 *  Note: Since the resource's data is stored in data files,
	 *        this method will throw an error if the respective
	 *        data file was not added first with addDataFile().
	 */
	Common::SeekableReadStream *getResource(uint32 index) const;

private:
	/** Internal resource information. */
	struct IResource {
		uint32 dataFileIndex; ///< Index into the data file list.
		uint32 resIndex;      ///< Index into the data file's resource table.

		KEYDataFile *dataFile; ///< The actual data file containing this resource.
	};

	typedef std::vector<IResource> IResourceList;

	/** External list of resource names and types. */
	ResourceList _resources;

	/** Internal list of resource data file indices. */
	IResourceList _iResources;

	/** All managed data files (BIF/BZF). */
	std::vector<Common::UString> _dataFiles;

	void load(Common::SeekableReadStream &key);

	void readDataFileList(Common::SeekableReadStream &key, uint32 offset);
	void readResList(Common::SeekableReadStream &key, uint32 offset);

	const IResource &getIResource(uint32 index) const;
};

} // End of namespace Aurora

#endif // AURORA_KEYFILE_H
