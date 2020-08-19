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
 *  Handling BioWare's BZFs (resource data files), used in the iOS version of
 *  Knights of the Old Republic.
 *
 *  Essentially, they are BIF files with LZMA-compressed data.
 */

#include <cassert>

#include "src/common/util.h"
#include "src/common/strutil.h"
#include "src/common/error.h"
#include "src/common/memreadstream.h"
#include "src/common/lzma.h"

#include "src/aurora/bzffile.h"

static const uint32_t kBZFID     = MKTAG('B', 'I', 'F', 'F');
static const uint32_t kVersion1  = MKTAG('V', '1', ' ', ' ');

namespace Aurora {

BZFFile::BZFFile(Common::SeekableReadStream *bzf) : _bzf(bzf) {
	assert(_bzf);

	load(*_bzf);
}

BZFFile::~BZFFile() {
}

void BZFFile::load(Common::SeekableReadStream &bzf) {
	readHeader(bzf);

	if (_id != kBZFID)
		throw Common::Exception("Not a BZF file");

	if (_version != kVersion1)
		throw Common::Exception("Unsupported BZF file version %08X", _version);

	uint32_t varResCount = bzf.readUint32LE();
	uint32_t fixResCount = bzf.readUint32LE();

	if (fixResCount != 0)
		throw Common::Exception("TODO: Fixed BZF resources");

	uint32_t offVarResTable = bzf.readUint32LE();

	try {

		_resources.resize(varResCount);
		readVarResTable(bzf, offVarResTable);

	} catch (Common::Exception &e) {
		e.add("Failed reading BZF file");
		throw;
	}

}

void BZFFile::readVarResTable(Common::SeekableReadStream &bzf, uint32_t offset) {
	bzf.seek(offset);

	for (uint32_t i = 0; i < _resources.size(); i++) {
		bzf.skip(4); // ID

		_resources[i].offset = bzf.readUint32LE();
		_resources[i].size   = bzf.readUint32LE();
		_resources[i].type   = (FileType) bzf.readUint32LE();

		if (i > 0)
			_resources[i - 1].packedSize = _resources[i].offset - _resources[i - 1].offset;
	}

	if (!_resources.empty())
		_resources.back().packedSize = bzf.size() - _resources.back().offset;
}

Common::SeekableReadStream *BZFFile::getResource(uint32_t index) const {
	const Resource &res = getRes(index);
	if ((res.packedSize == 0) || (res.size == 0))
		return new Common::MemoryReadStream(static_cast<const byte *>(0), 0);

	_bzf->seek(res.offset);

	return Common::decompressLZMA1(*_bzf, res.packedSize, res.size);
}

} // End of namespace Aurora
