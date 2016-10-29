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
 *  Unit tests for our KEY resource index reader.
 */

#include <vector>

#include "gtest/gtest.h"

#include "src/common/error.h"
#include "src/common/memreadstream.h"

#include "src/aurora/keyfile.h"

// --- KEY V1.0 ---

static const byte kKEY10File[] = {
	0x4B,0x45,0x59,0x20,0x56,0x31,0x20,0x20,0x01,0x00,0x00,0x00,0x01,0x00,0x00,0x00,
	0x40,0x00,0x00,0x00,0x5B,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x4C,0x00,0x00,0x00,0x0F,0x00,0x00,0x00,0x64,0x61,0x74,0x61,
	0x5C,0x78,0x6F,0x72,0x65,0x6F,0x73,0x2E,0x62,0x69,0x66,0x6F,0x7A,0x79,0x6D,0x61,
	0x6E,0x64,0x69,0x61,0x73,0x00,0x00,0x00,0x00,0x00,0x00,0x0A,0x00,0x01,0x00,0x00,
	0x00
};

GTEST_TEST(KEYFile10, getDataFileList) {
	Aurora::KEYFile key(new Common::MemoryReadStream(kKEY10File));

	const std::vector<Common::UString> &dataFiles = key.getDataFileList();
	ASSERT_EQ(dataFiles.size(), 1);

	EXPECT_STREQ(dataFiles[0].c_str(), "data/xoreos.bif");
}

GTEST_TEST(KEYFile10, haveDataFile) {
	Aurora::KEYFile key(new Common::MemoryReadStream(kKEY10File));

	EXPECT_FALSE(key.haveDataFile(0));

	EXPECT_THROW(key.haveDataFile(1), Common::Exception);
}

GTEST_TEST(KEYFile10, getResources) {
	Aurora::KEYFile key(new Common::MemoryReadStream(kKEY10File));

	const Aurora::KEYFile::ResourceList &res = key.getResources();
	ASSERT_EQ(res.size(), 1);

	EXPECT_STREQ(res.begin()->name.c_str(), "ozymandias");
	EXPECT_EQ(res.begin()->type, Aurora::kFileTypeTXT);
	EXPECT_EQ(res.begin()->index, 0);
	EXPECT_EQ(res.begin()->hash, 0);
}

GTEST_TEST(KEYFile10, getResourceSize) {
	Aurora::KEYFile key(new Common::MemoryReadStream(kKEY10File));

	EXPECT_EQ(key.getResourceSize(0), 0xFFFFFFFF);

	EXPECT_THROW(key.getResourceSize(1), Common::Exception);
}

GTEST_TEST(KEYFile10, getResource) {
	const Aurora::KEYFile key(new Common::MemoryReadStream(kKEY10File));

	EXPECT_THROW(key.getResource(0), Common::Exception);
	EXPECT_THROW(key.getResource(1), Common::Exception);
}

GTEST_TEST(KEYFile10, getNameHashAlgo) {
	const Aurora::KEYFile key(new Common::MemoryReadStream(kKEY10File));

	EXPECT_EQ(key.getNameHashAlgo(), Common::kHashNone);
}

GTEST_TEST(KEYFile10, findResourceHash) {
	const Aurora::KEYFile key(new Common::MemoryReadStream(kKEY10File));

	EXPECT_EQ(key.findResource(0), 0xFFFFFFFF);
}

GTEST_TEST(KEYFile10, findResourceName) {
	const Aurora::KEYFile key(new Common::MemoryReadStream(kKEY10File));

	EXPECT_EQ(key.findResource("ozymandias", Aurora::kFileTypeTXT), 0);

	EXPECT_EQ(key.findResource("ozymandias", Aurora::kFileTypeBMP), 0xFFFFFFFF);
	EXPECT_EQ(key.findResource("nope"      , Aurora::kFileTypeTXT), 0xFFFFFFFF);
	EXPECT_EQ(key.findResource("nope"      , Aurora::kFileTypeBMP), 0xFFFFFFFF);
}

// --- KEY V1.1 ---

static const byte kKEY11File[] = {
	0x4B,0x45,0x59,0x20,0x56,0x31,0x2E,0x31,0x01,0x00,0x00,0x00,0x01,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x44,0x00,0x00,0x00,0x5F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x50,0x00,0x00,0x00,0x0F,0x00,0x00,0x00,
	0x64,0x61,0x74,0x61,0x5C,0x78,0x6F,0x72,0x65,0x6F,0x73,0x2E,0x62,0x69,0x66,0x6F,
	0x7A,0x79,0x6D,0x61,0x6E,0x64,0x69,0x61,0x73,0x00,0x00,0x00,0x00,0x00,0x00,0x0A,
	0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

GTEST_TEST(KEYFile11, getDataFileList) {
	Aurora::KEYFile key(new Common::MemoryReadStream(kKEY11File));

	const std::vector<Common::UString> &dataFiles = key.getDataFileList();
	ASSERT_EQ(dataFiles.size(), 1);

	EXPECT_STREQ(dataFiles[0].c_str(), "data/xoreos.bif");
}

GTEST_TEST(KEYFile11, haveDataFile) {
	Aurora::KEYFile key(new Common::MemoryReadStream(kKEY11File));

	EXPECT_FALSE(key.haveDataFile(0));

	EXPECT_THROW(key.haveDataFile(1), Common::Exception);
}

GTEST_TEST(KEYFile11, getResources) {
	Aurora::KEYFile key(new Common::MemoryReadStream(kKEY11File));

	const Aurora::KEYFile::ResourceList &res = key.getResources();
	ASSERT_EQ(res.size(), 1);

	EXPECT_STREQ(res.begin()->name.c_str(), "ozymandias");
	EXPECT_EQ(res.begin()->type, Aurora::kFileTypeTXT);
	EXPECT_EQ(res.begin()->index, 0);
	EXPECT_EQ(res.begin()->hash, 0);
}

GTEST_TEST(KEYFile11, getResourceSize) {
	Aurora::KEYFile key(new Common::MemoryReadStream(kKEY11File));

	EXPECT_EQ(key.getResourceSize(0), 0xFFFFFFFF);

	EXPECT_THROW(key.getResourceSize(1), Common::Exception);
}

GTEST_TEST(KEYFile11, getResource) {
	Aurora::KEYFile key(new Common::MemoryReadStream(kKEY11File));

	EXPECT_THROW(key.getResource(0), Common::Exception);
	EXPECT_THROW(key.getResource(1), Common::Exception);
}

GTEST_TEST(KEYFile11, getNameHashAlgo) {
	const Aurora::KEYFile key(new Common::MemoryReadStream(kKEY11File));

	EXPECT_EQ(key.getNameHashAlgo(), Common::kHashNone);
}

GTEST_TEST(KEYFile11, findResourceHash) {
	const Aurora::KEYFile key(new Common::MemoryReadStream(kKEY11File));

	EXPECT_EQ(key.findResource(0), 0xFFFFFFFF);
}

GTEST_TEST(KEYFile11, findResourceName) {
	const Aurora::KEYFile key(new Common::MemoryReadStream(kKEY11File));

	EXPECT_EQ(key.findResource("ozymandias", Aurora::kFileTypeTXT), 0);

	EXPECT_EQ(key.findResource("ozymandias", Aurora::kFileTypeBMP), 0xFFFFFFFF);
	EXPECT_EQ(key.findResource("nope"      , Aurora::kFileTypeTXT), 0xFFFFFFFF);
	EXPECT_EQ(key.findResource("nope"      , Aurora::kFileTypeBMP), 0xFFFFFFFF);
}
