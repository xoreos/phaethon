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
 *  Unit tests for our LZMA decompressor (which uses lzma).
 */

#include "gtest/gtest.h"

#include "src/common/lzma.h"
#include "src/common/memreadstream.h"
#include "src/common/error.h"

// Percy Bysshe Shelley's "Ozymandias"
static const char *kDataUncompressed =
	"I met a traveller from an antique land\n"
	"Who said: Two vast and trunkless legs of stone\n"
	"Stand in the desert. Near them, on the sand,\n"
	"Half sunk, a shattered visage lies, whose frown,\n"
	"And wrinkled lip, and sneer of cold command,\n"
	"Tell that its sculptor well those passions read\n"
	"Which yet survive, stamped on these lifeless things,\n"
	"The hand that mocked them and the heart that fed:\n"
	"And on the pedestal these words appear:\n"
	"'My name is Ozymandias, king of kings:\n"
	"Look on my works, ye Mighty, and despair!'\n"
	"Nothing beside remains. Round the decay\n"
	"Of that colossal wreck, boundless and bare\n"
	"The lone and level sands stretch far away.";

// Percy Bysshe Shelley's "Ozymandias", compressed using LZMA
static const byte kDataCompressed[] = {
	0x5D,0x00,0x00,0x00,0x01,0x00,0x24,0x88,0x09,0xA6,0x53,0xD8,0x61,0x0F,0xAE,0x55,
	0xA9,0x63,0x39,0x41,0xFC,0x9F,0xD4,0xA8,0x78,0x14,0xAC,0xE4,0x2B,0xCC,0x0A,0x14,
	0xAF,0x12,0x42,0x25,0xBC,0xEF,0xA4,0x3C,0x92,0x2C,0xC3,0x41,0x1E,0x0C,0x7E,0xAB,
	0x97,0x95,0x4B,0xE0,0x18,0xFC,0x32,0xE0,0x7E,0xAC,0x35,0xED,0x0D,0xE8,0x9D,0xFA,
	0xB0,0x7D,0xCD,0x26,0x2B,0x71,0xF2,0xC4,0xCF,0x31,0x2C,0xF9,0xD8,0xEC,0xFB,0xFC,
	0xF5,0x7E,0x9E,0x1C,0x36,0xB0,0x30,0x72,0xE7,0x43,0xD8,0x64,0xA9,0x22,0x29,0xDD,
	0x62,0xB2,0x19,0xB5,0x01,0xBD,0xC0,0x21,0xF2,0xEC,0x1C,0xF3,0x58,0xF0,0xBD,0x95,
	0xBA,0xA0,0xBA,0xC4,0x6E,0xB2,0xDC,0x30,0x18,0x5E,0xA0,0x6F,0x72,0x03,0x57,0x1B,
	0x8B,0xE6,0x63,0x39,0x72,0x58,0xE3,0xE8,0x70,0xE3,0x91,0xD1,0x0A,0x95,0x8E,0xC3,
	0x34,0x79,0x10,0x27,0x5F,0x5A,0xFE,0xAA,0x27,0xCA,0xF2,0x15,0x1C,0x6C,0x72,0x86,
	0xE1,0xE1,0x4A,0x57,0x1C,0xA3,0x76,0x66,0xF6,0x6A,0xC5,0xD8,0x7E,0xEE,0x04,0x0C,
	0x98,0x6E,0x4D,0x70,0xBB,0x98,0xD9,0x59,0xD4,0xD0,0x25,0x34,0x7D,0x76,0xCF,0x02,
	0x40,0xD7,0x78,0x47,0xC0,0xE0,0x4E,0xD2,0xF7,0x05,0x45,0x16,0x3F,0x2E,0xDD,0xAC,
	0x68,0x60,0xE3,0x49,0x96,0x36,0xA7,0x52,0x22,0xEE,0x42,0xC8,0x6E,0x9A,0x14,0x20,
	0xD7,0x03,0x35,0x25,0xF7,0xAB,0x8A,0x8B,0x38,0x9F,0xBF,0x79,0x81,0x0B,0x3A,0x7B,
	0xA1,0x55,0xF2,0xF5,0xF6,0x7E,0xA5,0x47,0x34,0xAF,0x22,0x82,0x9A,0xFF,0xB1,0x93,
	0xCF,0x47,0x98,0x63,0xF4,0x11,0xC8,0xD0,0x48,0x3F,0xC5,0xC9,0x1E,0xAD,0x4F,0x88,
	0xBF,0x57,0x40,0xB0,0x7E,0xA2,0xB5,0xC8,0xA7,0x0B,0x64,0x83,0xD7,0xAB,0x8A,0x33,
	0xA6,0x64,0xEA,0x2B,0xCF,0x41,0x96,0x92,0xF5,0x7B,0x66,0x48,0xA3,0x53,0x9D,0x01,
	0x4F,0xC3,0xDF,0xA3,0x85,0x54,0x45,0x65,0xA9,0x3C,0x20,0x31,0x02,0x55,0xDB,0x64,
	0x33,0x50,0x19,0x7A,0x58,0x64,0x87,0x72,0xF6,0x12,0x05,0xA3,0x83,0xFC,0xB4,0x0E,
	0x28,0x5B,0x5C,0x17,0x57,0xB3,0xD8,0xF7,0xBE,0x1D,0xDF,0x96,0x32,0x36,0xA0,0xFE,
	0x51,0x56,0x2D,0x84,0x2B,0xCA,0x2B,0x85,0x53,0x71,0xC1,0x33,0x3B,0xD2,0x77,0x2F,
	0xB3,0x9E,0x87,0x71,0x8A,0x01,0x94,0x26,0x53,0x11,0x73,0x21,0xB5,0xD4,0x15,0xFB,
	0xAC,0xC3,0xE7,0xA5,0x0A,0x09,0xF4,0x36,0x5B,0x88,0x25,0x51,0x0C,0x12,0xB5,0x09,
	0x8A,0x78,0x57,0x5A,0xCC,0x20,0x13,0xC3,0xFD,0xC2,0x1E,0xF9,0xA6,0xF4,0xA1,0x77,
	0xB2,0xAD,0xD3,0x6B,0xEF,0xB9,0xF7,0xA1,0x28,0x8A,0xB4,0x3D,0xCE,0x54,0xDA,0x78,
	0xFF,0xF8,0x6E,0xCB,0x5F
};

GTEST_TEST(LZMA1, decompressBuf) {
	static const size_t kSizeCompressed   = sizeof(kDataCompressed);
	static const size_t kSizeDecompressed = strlen(kDataUncompressed);

	const byte *decompressed =
		Common::decompressLZMA1(kDataCompressed, kSizeCompressed, kSizeDecompressed);
	ASSERT_NE(decompressed, static_cast<const byte *>(0));

	for (size_t i = 0; i < kSizeDecompressed; i++)
		EXPECT_EQ(decompressed[i], kDataUncompressed[i]) << "At index " << i;

	delete[] decompressed;
}

GTEST_TEST(LZMA1, decompressStream) {
	static const size_t kSizeCompressed   = sizeof(kDataCompressed);
	static const size_t kSizeDecompressed = strlen(kDataUncompressed);

	Common::MemoryReadStream compressed(kDataCompressed);

	Common::SeekableReadStream *decompressed =
		Common::decompressLZMA1(compressed, kSizeCompressed, kSizeDecompressed);
	ASSERT_NE(decompressed, static_cast<Common::SeekableReadStream *>(0));

	ASSERT_EQ(decompressed->size(), kSizeDecompressed);

	for (size_t i = 0; i < kSizeDecompressed; i++)
		EXPECT_EQ(decompressed->readByte(), kDataUncompressed[i]) << "At index " << i;

	delete decompressed;
}

GTEST_TEST(LZMA1, decompressFailOutputSmall) {
	static const size_t kSizeCompressed   = sizeof(kDataCompressed);
	static const size_t kSizeDecompressed = strlen(kDataUncompressed) / 2;

	EXPECT_THROW(Common::decompressLZMA1(kDataCompressed, kSizeCompressed, kSizeDecompressed),
	             Common::Exception);
}

GTEST_TEST(LZMA1, decompressFailOutputBig) {
	static const size_t kSizeCompressed   = sizeof(kDataCompressed);
	static const size_t kSizeDecompressed = strlen(kDataUncompressed) * 2;

	EXPECT_THROW(Common::decompressLZMA1(kDataCompressed, kSizeCompressed, kSizeDecompressed),
	             Common::Exception);
}

GTEST_TEST(LZMA1, decompressFailInputCut) {
	static const size_t kSizeCompressed   = sizeof(kDataCompressed) / 2;
	static const size_t kSizeDecompressed = strlen(kDataUncompressed);

	EXPECT_THROW(Common::decompressLZMA1(kDataCompressed, kSizeCompressed, kSizeDecompressed),
	             Common::Exception);
}
