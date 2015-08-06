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

#include "src/common/error.h"

#include "src/aurora/keydatafile.h"

namespace Aurora {

KEYDataFile::KEYDataFile() {
}

KEYDataFile::~KEYDataFile() {
}

FileType KEYDataFile::getResourceType(uint32 index) const {
	return getRes(index).type;
}

uint32 KEYDataFile::getResourceSize(uint32 index) const {
	return getRes(index).size;
}

const KEYDataFile::Resource &KEYDataFile::getRes(uint32 index) const {
	if (index >= _resources.size())
		throw Common::Exception("Resource index out of range (%u/%u)", index, (uint)_resources.size());

	return _resources[index];
}

} // End of namespace Aurora
