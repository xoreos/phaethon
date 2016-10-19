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

#ifndef AURORA_BZFFILE_H
#define AURORA_BZFFILE_H

#include "src/common/types.h"
#include "src/common/scopedptr.h"

#include "src/aurora/types.h"
#include "src/aurora/aurorafile.h"
#include "src/aurora/keydatafile.h"

namespace Aurora {

/** Class to hold resource data information of a BZF file.
 *
 *  A BZF is a compressed variation of a BIF file, found exclusively
 *  in the iOS version of Knights of the Old Republic.
 *
 *  See also classes KEYFile in keyfile.h and BIFFile in biffile.h.
 */
class BZFFile : public AuroraFile, public KEYDataFile {
public:
	/** Take over this stream and read a BZF file out of it. */
	BZFFile(Common::SeekableReadStream *bzf);
	~BZFFile();

	/** Return a stream of the resource's contents. */
	Common::SeekableReadStream *getResource(uint32 index) const;

private:
	Common::ScopedPtr<Common::SeekableReadStream> _bzf;

	void load(Common::SeekableReadStream &bzf);

	void readVarResTable(Common::SeekableReadStream &bzf, uint32 offset);

	Common::SeekableReadStream *decompress(byte *compressedData, uint32 packedSize, uint32 unpackedSize) const;
};

} // End of namespace Aurora

#endif // AURORA_BZFFILE_H
