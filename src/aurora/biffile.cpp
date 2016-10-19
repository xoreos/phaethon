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
 *  Handling BioWare's BIFs (resource data files).
 */

/* See BioWare's own specs released for Neverwinter Nights modding
 * (<https://github.com/xoreos/xoreos-docs/tree/master/specs/bioware>)
 */

#include <cassert>

#include "src/common/util.h"
#include "src/common/strutil.h"
#include "src/common/error.h"
#include "src/common/memreadstream.h"

#include "src/aurora/biffile.h"

static const uint32 kBIFID     = MKTAG('B', 'I', 'F', 'F');
static const uint32 kVersion1  = MKTAG('V', '1', ' ', ' ');
static const uint32 kVersion11 = MKTAG('V', '1', '.', '1');

namespace Aurora {

BIFFile::BIFFile(Common::SeekableReadStream *bif) : _bif(bif) {
	assert(_bif);

	load(*_bif);
}

BIFFile::~BIFFile() {
}

void BIFFile::load(Common::SeekableReadStream &bif) {
	readHeader(bif);

	if (_id != kBIFID)
		throw Common::Exception("Not a BIF file (%s)", Common::debugTag(_id).c_str());

	if ((_version != kVersion1) && (_version != kVersion11))
		throw Common::Exception("Unsupported BIF file version %s", Common::debugTag(_version).c_str());

	uint32 varResCount = bif.readUint32LE();
	uint32 fixResCount = bif.readUint32LE();

	if (fixResCount != 0)
		throw Common::Exception("TODO: Fixed BIF resources");

	uint32 offVarResTable = bif.readUint32LE();

	try {

		_resources.resize(varResCount);
		readVarResTable(bif, offVarResTable);

	} catch (Common::Exception &e) {
		e.add("Failed reading BIF file");
		throw;
	}

}

void BIFFile::readVarResTable(Common::SeekableReadStream &bif, uint32 offset) {
	bif.seek(offset);

	for (ResourceList::iterator res = _resources.begin(); res != _resources.end(); ++res) {
		bif.skip(4); // ID

		if (_version == kVersion11)
			bif.skip(4); // Flags

		res->offset = bif.readUint32LE();
		res->size   = bif.readUint32LE();
		res->type   = (FileType) bif.readUint32LE();

		res->packedSize = res->size;
	}
}

Common::SeekableReadStream *BIFFile::getResource(uint32 index) const {
	const Resource &res = getRes(index);
	if (res.size == 0)
		return new Common::MemoryReadStream(0, 0);

	_bif->seek(res.offset);

	Common::ScopedPtr<Common::SeekableReadStream> resStream(_bif->readStream(res.size));

	if (!resStream || (((uint32) resStream->size()) != res.size))
		throw Common::Exception(Common::kReadError);

	return resStream.release();
}

} // End of namespace Aurora
