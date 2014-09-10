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

/** @file images/dumptga.h
 *  A simple TGA image dumper.
 */

#ifndef IMAGES_DUMPTGA_H
#define IMAGES_DUMPTGA_H

#include "common/types.h"

#include "images/types.h"

namespace Common {
	class UString;
}

namespace Images {

class Decoder;

/** Dump raw image data into a TGA file. */
void dumpTGA(const Common::UString &fileName, const byte *data, int width, int height, PixelFormat format);
/** Dump image into a TGA file. */
void dumpTGA(const Common::UString &fileName, const Decoder *image);

} // End of namespace Images

#endif // IMAGES_DUMPTGA_H
