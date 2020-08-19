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
 *  Handling BioWare's localized strings.
 */

/* See BioWare's own specs released for Neverwinter Nights modding
 * (<https://github.com/xoreos/xoreos-docs/tree/master/specs/bioware>)
 */

#include <memory>

#include "src/common/util.h"
#include "src/common/memreadstream.h"
#include "src/common/encoding.h"

#include "src/aurora/locstring.h"
#include "src/aurora/aurorafile.h"

namespace Aurora {

void LocString::clear() {
	_id = kStrRefInvalid;

	_strings.clear();
}

bool LocString::empty() const {
	return (_id == kStrRefInvalid) && (_strings.empty() || getString().empty());
}

void LocString::swap(LocString &str) {
	std::swap(_id, str._id);

	_strings.swap(str._strings);
}

uint32_t LocString::getID() const {
	return _id;
}

void LocString::setID(uint32_t id) {
	_id = id;
}

bool LocString::hasString(Language language, LanguageGender gender) const {
	return hasString(LangMan.getLanguageID(language, gender));
}

bool LocString::hasString(uint32_t languageID) const {
	return _strings.find(languageID) != _strings.end();
}

const Common::UString &LocString::getString(Language language, LanguageGender gender) const {
	return getString(LangMan.getLanguageID(language, gender));
}

static const Common::UString kEmpty;
const Common::UString &LocString::getString(uint32_t languageID) const {
	StringMap::const_iterator s = _strings.find(languageID);
	if (s == _strings.end())
		return kEmpty;

	return s->second;
}

void LocString::setString(Language language, LanguageGender gender, const Common::UString &str) {
	return setString(LangMan.getLanguageID(language, gender), str);
}

void LocString::setString(Language language, const Common::UString &str) {
	setString(language, kLanguageGenderMale  , str);
	setString(language, kLanguageGenderFemale, str);
}

void LocString::setString(uint32_t languageID, const Common::UString &str) {
	_strings[languageID] = str;
}

const Common::UString &LocString::getStrRefString() const {
	if (_id == kStrRefInvalid)
		return kEmpty;

	// TODO: Return numerical ID?
	return kEmpty;
}

const Common::UString &LocString::getFirstString() const {
	if (_strings.empty())
		return getStrRefString();

	return _strings.begin()->second;
}

const Common::UString &LocString::getString() const {
	uint32_t languageID = LangMan.getLanguageID(LangMan.getCurrentLanguageText(), LangMan.getCurrentGender());

	// Look whether we have an internal localized string
	if (hasString(languageID))
		return getString(languageID);

	// Try the differently gendered internal string
	if (hasString(LangMan.swapLanguageGender(languageID)))
		return getString(LangMan.swapLanguageGender(languageID));

	// Next, try the external localized one
	const Common::UString &refString = getStrRefString();
	if (!refString.empty())
		return refString;

	// If all else fails, just get the first one available
	return getFirstString();
}

void LocString::getStrings(std::vector<SubLocString> &str) const {
	for (StringMap::const_iterator s = _strings.begin(); s != _strings.end(); ++s)
		str.push_back(SubLocString(s->first, s->second));
}

void LocString::readString(uint32_t languageID, Common::SeekableReadStream &stream) {
	uint32_t length = stream.readUint32LE();

	std::pair<StringMap::iterator, bool> s = _strings.insert(std::make_pair(languageID, ""));
	if (length == 0)
		return;

	s.first->second = "[???]";

	std::unique_ptr<Common::MemoryReadStream> data(stream.readStream(length));
	std::unique_ptr<Common::MemoryReadStream> parsed(LangMan.preParseColorCodes(*data));

	Common::Encoding encoding = LangMan.getEncodingLocString(LangMan.getLanguageGendered(languageID));
	if (encoding == Common::kEncodingInvalid)
		encoding = Common::kEncodingUTF8;

	try {
		s.first->second = Common::readString(*parsed, encoding);
	} catch (...) {
		parsed->seek(0);
		s.first->second = Common::readString(*parsed, Common::kEncodingCP1252);
	}
}

void LocString::readLocSubString(Common::SeekableReadStream &stream) {
	uint32_t languageID = stream.readUint32LE();

	readString(languageID, stream);
}

void LocString::readLocString(Common::SeekableReadStream &stream, uint32_t id, uint32_t count) {
	_id = id;

	while (count-- > 0)
		readLocSubString(stream);
}

void LocString::readLocString(Common::SeekableReadStream &stream) {
	uint32_t id    = stream.readUint32LE();
	uint32_t count = stream.readUint32LE();

	readLocString(stream, id, count);
}

} // End of namespace Aurora
