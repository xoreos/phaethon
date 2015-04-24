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

#include "common/util.h"
#include "common/error.h"
#include "common/stream.h"
#include "common/file.h"

#include "aurora/keyfile.h"
#include "aurora/keydatafile.h"

static const uint32 kKEYID     = MKTAG('K', 'E', 'Y', ' ');
static const uint32 kVersion1  = MKTAG('V', '1', ' ', ' ');
static const uint32 kVersion11 = MKTAG('V', '1', '.', '1');

namespace Aurora {

KEYFile::KEYFile(const Common::UString &fileName) {
	Common::File key;
	if (!key.open(fileName))
		throw Common::Exception(Common::kOpenError);

	load(key);
}

KEYFile::~KEYFile() {
}

void KEYFile::clear() {
	_resources.clear();
	_iResources.clear();
	_dataFiles.clear();
}

bool KEYFile::haveDataFile(uint32 index) const {
	return getIResource(index).dataFile != 0;
}

void KEYFile::addDataFile(uint32 dataFileIndex, KEYDataFile *dataFile) {
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
		throw Common::Exception("Not a KEY file");

	if ((_version != kVersion1) && (_version != kVersion11))
		throw Common::Exception("Unsupported KEY file version %08X", _version);

	uint32 dataFileCount = key.readUint32LE();
	uint32 resCount      = key.readUint32LE();

	// Version 1.1 has some NULL bytes here
	if (_version == kVersion11)
		key.skip(4);

	uint32 offFileTable = key.readUint32LE();
	uint32 offResTable  = key.readUint32LE();

	key.skip( 8); // Build year and day
	key.skip(32); // Reserved

	try {

		_dataFiles.resize(dataFileCount);
		readDataFileList(key, offFileTable);

		_resources.resize(resCount);
		_iResources.resize(resCount);
		readResList(key, offResTable);

		if (key.err())
			throw Common::Exception(Common::kReadError);

	} catch (Common::Exception &e) {
		e.add("Failed reading KEY file");
		throw;
	}

}

void KEYFile::readDataFileList(Common::SeekableReadStream &key, uint32 offset) {
	if (!key.seek(offset))
		throw Common::Exception(Common::kSeekError);

	for (std::vector<Common::UString>::iterator d = _dataFiles.begin(); d != _dataFiles.end(); ++d) {
		key.skip(4); // File size of the data file

		uint32 nameOffset = key.readUint32LE();
		uint32 nameSize   = 0;

		// nameSize is expanded to 4 bytes in 1.1 and the location is dropped
		if (_version == kVersion11) {
			nameSize = key.readUint32LE();
		} else {
			nameSize = key.readUint16LE();
			key.skip(2); // Location of the data file (HD, CD, ...)
		}

		uint32 curPos = key.seekTo(nameOffset);
		d->readFixedASCII(key, nameSize);
		key.seekTo(curPos);

		AuroraFile::cleanupPath(*d);
	}
}

void KEYFile::readResList(Common::SeekableReadStream &key, uint32 offset) {
	if (!key.seek(offset))
		throw Common::Exception(Common::kSeekError);

	uint32 index = 0;
	ResourceList::iterator   res = _resources.begin();
	IResourceList::iterator iRes = _iResources.begin();
	for (; (res != _resources.end()) && (iRes != _iResources.end()); ++index, ++res, ++iRes) {
		iRes->dataFile = 0;

		res->name.readFixedASCII(key, 16);
		res->type  = (FileType) key.readUint16LE();
		res->index = index;

		uint32 id = key.readUint32LE();

		// The new flags field holds the data file index now. The rest contains fixed
		// resource info.
		if (_version == kVersion11) {
			uint32 flags = key.readUint32LE();
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

const KEYFile::IResource &KEYFile::getIResource(uint32 index) const {
	if (index >= _iResources.size())
		throw Common::Exception("Resource index out of range (%d/%d)", index, _iResources.size());

	return _iResources[index];
}

uint32 KEYFile::getResourceSize(uint32 index) const {
	const IResource &iRes = getIResource(index);
	if (!iRes.dataFile)
		return 0xFFFFFFFF;

	return iRes.dataFile->getResourceSize(iRes.resIndex);
}

Common::SeekableReadStream *KEYFile::getResource(uint32 index) const {
	const IResource &iRes = getIResource(index);
	if (!iRes.dataFile)
		throw Common::Exception("Data files for resource %d (\"%s\") missing", index,
		                        _dataFiles[iRes.dataFileIndex].c_str());

	return iRes.dataFile->getResource(iRes.resIndex);
}

} // End of namespace Aurora
