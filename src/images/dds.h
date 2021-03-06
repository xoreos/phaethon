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
 *  DDS (DirectDraw Surface) loading.
 */

#ifndef IMAGES_DDS_H
#define IMAGES_DDS_H

#include "src/images/decoder.h"

namespace Common {
	class SeekableReadStream;
}

namespace Images {

/** DirectDraw Surface.
 *
 *  Both standard DDS files and BioWare's own version are supported.
 */
class DDS : public Decoder {
public:
	DDS(Common::SeekableReadStream &dds);
	~DDS();

	/** Return true if the data within this stream is a DDS image. */
	static bool detect(Common::SeekableReadStream &dds);

private:
	enum DataType {
		kDataTypeDirect,
		kDataType4444
	};

	/** The specific pixel format of the included image data. */
	struct DDSPixelFormat {
		uint32_t size;     ///< The size of the image data in bytes.
		uint32_t flags;    ///< Features of the image data.
		uint32_t fourCC;   ///< The FourCC to detect the format by.
		uint32_t bitCount; ///< Number of bits per pixel.
		uint32_t rBitMask; ///< Bit mask for the red color component.
		uint32_t gBitMask; ///< Bit mask for the green color component.
		uint32_t bBitMask; ///< Bit mask for the blue color component.
		uint32_t aBitMask; ///< Bit mask for the alpha component.
	};

	// Loading helpers
	void load(Common::SeekableReadStream &dds);
	void readHeader(Common::SeekableReadStream &dds, DataType &dataType);
	void readStandardHeader(Common::SeekableReadStream &dds, DataType &dataType);
	void readBioWareHeader(Common::SeekableReadStream &dds, DataType &dataType);
	void readData(Common::SeekableReadStream &dds, DataType dataType);

	void detectFormat(const DDSPixelFormat &format, DataType &dataType);

	void setSize(MipMap &mipMap);
};

} // End of namespace Images

#endif // IMAGES_DDS_H
