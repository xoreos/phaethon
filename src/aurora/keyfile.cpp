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

/* See BioWare's own specs released for Neverwinter Nights modding
 * (<https://github.com/xoreos/xoreos-docs/tree/master/specs/bioware>)
 */

#include <memory>
#include <cstddef>

#include "src/common/util.h"
#include "src/common/strutil.h"
#include "src/common/error.h"
#include "src/common/readstream.h"
#include "src/common/encoding.h"

#include "src/aurora/keyfile.h"
#include "src/aurora/keydatafile.h"

static const uint32_t kKEYID     = MKTAG('K', 'E', 'Y', ' ');
static const uint32_t kVersion1  = MKTAG('V', '1', ' ', ' ');
static const uint32_t kVersion11 = MKTAG('V', '1', '.', '1');

namespace Aurora {

KEYFile::KEYFile(Common::SeekableReadStream *key) {
	std::unique_ptr<Common::SeekableReadStream> keyStream(key);

	load(*keyStream);
}

KEYFile::~KEYFile() {
}

bool KEYFile::haveDataFile(uint32_t index) const {
	return getIResource(index).dataFile != 0;
}

void KEYFile::addDataFile(uint32_t dataFileIndex, KEYDataFile *dataFile) {
	if (!dataFile)
		throw Common::Exception("KEYFile::addDataFile(): dataFile == 0");

	ResourceList::iterator   res = _resources.begin();
	IResourceList::iterator iRes = _iResources.begin();
	for (; (res != _resources.end()) && (iRes != _iResources.end()); ++res, ++iRes) {
		// Merge information for all resources where the data file index matches
		if (iRes->dataFileIndex != dataFileIndex)
			continue;

		if (res->type != dataFile->getResourceType(iRes->resIndex))
			throw Common::Exception("Resource type doesn't match in data file (%d, %d, %d, %d, %d)",
			                        res->index, iRes->dataFileIndex, iRes->resIndex,
			                        res->type, dataFile->getResourceType(iRes->resIndex));

		iRes->dataFile = dataFile;
	}
}

const std::vector<Common::UString> &KEYFile::getDataFileList() const {
	return _dataFiles;
}

void KEYFile::load(Common::SeekableReadStream &key) {
	readHeader(key);

	if (_id != kKEYID)
		throw Common::Exception("Not a KEY file (%s)", Common::debugTag(_id).c_str());

	if ((_version != kVersion1) && (_version != kVersion11))
		throw Common::Exception("Unsupported KEY file version %s", Common::debugTag(_version).c_str());

	uint32_t dataFileCount = key.readUint32LE();
	uint32_t resCount      = key.readUint32LE();

	// Version 1.1 has some NULL bytes here
	if (_version == kVersion11)
		key.skip(4);

	uint32_t offFileTable     = key.readUint32LE();
	uint32_t offResTable      = key.readUint32LE();

	key.skip( 8); // Build year and day
	key.skip(32); // Reserved

	try {

		_dataFiles.resize(dataFileCount);
		readDataFileList(key, offFileTable);

		_resources.resize(resCount);
		_iResources.resize(resCount);
		readResList(key, offResTable);

	} catch (Common::Exception &e) {
		e.add("Failed reading KEY file");
		throw;
	}

}

void KEYFile::readDataFileList(Common::SeekableReadStream &key, uint32_t offset) {
	key.seek(offset);

	for (std::vector<Common::UString>::iterator d = _dataFiles.begin(); d != _dataFiles.end(); ++d) {
		key.skip(4); // File size of the data file

		ptrdiff_t nameOffset = key.readUint32LE();
		size_t    nameSize   = 0;

		// nameSize is expanded to 4 bytes in 1.1 and the location is dropped
		if (_version == kVersion11) {
			nameSize = key.readUint32LE();
		} else {
			nameSize = key.readUint16LE();
			key.skip(2); // Location of the data file (HD, CD, ...)
		}

		size_t curPos = key.seek(nameOffset);

		*d = Common::readStringFixed(key, Common::kEncodingASCII, nameSize);

		key.seek(curPos);

		d->replaceAll('\\', '/');
		if (d->beginsWith("/"))
			d->erase(d->begin());
	}
}

void KEYFile::readResList(Common::SeekableReadStream &key, uint32_t offset) {
	key.seek(offset);

	uint32_t index = 0;
	ResourceList::iterator   res = _resources.begin();
	IResourceList::iterator iRes = _iResources.begin();
	for (; (res != _resources.end()) && (iRes != _iResources.end()); ++index, ++res, ++iRes) {
		iRes->dataFile = 0;

		res->name  = Common::readStringFixed(key, Common::kEncodingASCII, 16);
		res->type  = (FileType) key.readUint16LE();
		res->index = index;

		uint32_t id = key.readUint32LE();

		// The new flags field holds the data file index now. The rest contains fixed
		// resource info.
		if (_version == kVersion11) {
			uint32_t flags = key.readUint32LE();
			iRes->dataFileIndex = (flags & 0xFFF00000) >> 20;
		} else
			iRes->dataFileIndex = id >> 20;

		// TODO: Fixed resources?
		iRes->resIndex = id & 0xFFFFF;
	}
}

const Archive::ResourceList &KEYFile::getResources() const {
	return _resources;
}

const KEYFile::IResource &KEYFile::getIResource(uint32_t index) const {
	if (index >= _iResources.size())
		throw Common::Exception("Resource index out of range (%u/%u)", index, (uint)_iResources.size());

	return _iResources[index];
}

uint32_t KEYFile::getResourceSize(uint32_t index) const {
	const IResource &iRes = getIResource(index);
	if (!iRes.dataFile)
		return 0xFFFFFFFF;

	return iRes.dataFile->getResourceSize(iRes.resIndex);
}

Common::SeekableReadStream *KEYFile::getResource(uint32_t index, bool UNUSED(tryNoCopy)) const {
	const IResource &iRes = getIResource(index);
	if (!iRes.dataFile)
		throw Common::Exception("Data files for resource %d (\"%s\") missing", index,
		                        _dataFiles[iRes.dataFileIndex].c_str());

	return iRes.dataFile->getResource(iRes.resIndex);
}

std::vector<const Archive::Resource *> KEYFile::getResourceListForDataFile(const Common::UString &dataFile) const {
	std::vector<const Archive::Resource *> list;

	uint32_t i = 0;
	for (auto it = _resources.begin(); it != _resources.end(); ++it, i++) {
		if (_dataFiles[getIResource(i).dataFileIndex] != dataFile)
			continue;

		list.push_back(&(*it));
	}

	return list;
}

} // End of namespace Aurora
