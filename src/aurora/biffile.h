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

#ifndef AURORA_BIFFILE_H
#define AURORA_BIFFILE_H

#include "common/ustring.h"

#include "aurora/types.h"
#include "aurora/aurorafile.h"
#include "aurora/keydatafile.h"

namespace Common {
	class File;
}

namespace Aurora {

/** Class to hold resource data information of a bif file. */
class BIFFile : public AuroraBase, public KEYDataFile {
public:
	BIFFile(const Common::UString &fileName);
	~BIFFile();

	/** Return a stream of the resource's contents. */
	Common::SeekableReadStream *getResource(uint32 index) const;

private:
	Common::UString _fileName;

	void open(Common::File &file) const;
	void load();

	void readVarResTable(Common::SeekableReadStream &bif, uint32 offset);
};

} // End of namespace Aurora

#endif // AURORA_BIFFILE_H
