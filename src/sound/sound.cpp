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

/** @file sound/sound.cpp
 *  The global sound manager, handling all sound output.
 */

#include "sound/sound.h"
#include "sound/audiostream.h"
#include "sound/decoders/asf.h"
#include "sound/decoders/mp3.h"
#include "sound/decoders/vorbis.h"
#include "sound/decoders/wave.h"

#include "common/util.h"
#include "common/error.h"
#include "common/stream.h"

DECLARE_SINGLETON(Sound::SoundManager)

namespace Sound {

SoundManager::SoundManager() {
}

SoundManager::~SoundManager() {
}

AudioStream *SoundManager::makeAudioStream(Common::SeekableReadStream *stream) {
	bool isMP3 = false;
	uint32 tag = stream->readUint32BE();

	if (tag == 0xfff360c4) {
		// Modified WAVE file (used in streamsounds folder, at least in KotOR 1/2)
		stream = new Common::SeekableSubReadStream(stream, 0x1D6, stream->size(), true);

	} else if (tag == MKTAG('R', 'I', 'F', 'F')) {
		stream->seek(12);
		tag = stream->readUint32BE();

		if (tag != MKTAG('f', 'm', 't', ' '))
			throw Common::Exception("Broken WAVE file");

		// Skip fmt chunk
		stream->skip(stream->readUint32LE());
		tag = stream->readUint32BE();

		while ((tag == MKTAG('f', 'a', 'c', 't')) || (tag == MKTAG('P', 'A', 'D', ' '))) {
			// Skip useless chunks
			stream->skip(stream->readUint32LE());
			tag = stream->readUint32BE();
		}

		if (tag != MKTAG('d', 'a', 't', 'a'))
			throw Common::Exception("Found invalid tag in WAVE file: %x", tag);

		uint32 dataSize = stream->readUint32LE();
		if (dataSize == 0) {
			isMP3 = true;
			stream = new Common::SeekableSubReadStream(stream, stream->pos(), stream->size(), true);
		} else
			// Just a regular WAVE
			stream->seek(0);

	} else if ((tag                    == MKTAG('B', 'M', 'U', ' ')) &&
	           (stream->readUint32BE() == MKTAG('V', '1', '.', '0'))) {

		// BMU files: MP3 with extra header
		isMP3 = true;
		stream = new Common::SeekableSubReadStream(stream, stream->pos(), stream->size(), true);

	} else if (tag == MKTAG('O', 'g', 'g', 'S')) {

		stream->seek(0);
		return makeVorbisStream(stream, true);

	} else if (tag == 0x3026B275) {

		// ASF (most probably with WMAv2)
		stream->seek(0);
		return makeASFStream(stream, true);

	} else if (((tag & 0xFFFFFF00) | 0x20) == MKTAG('I', 'D', '3', ' ')) {

		// ID3v2 tag found => Should be MP3.
		stream->seek(0);
		isMP3 = true;

	} else if ((tag & 0xFFFA0000) == 0xFFFA0000) {

		// MPEG sync + MPEG1 layer 3 bits found => Should be MP3.
		// NOTE: To decrease the chances of false positives, we could look at more than just the first frame.
		stream->seek(0);
		isMP3 = true;

	} else
		throw Common::Exception("Unknown sound format");

	if (isMP3)
		return makeMP3Stream(stream, true);

	return makeWAVStream(stream, true);
}

} // End of namespace Sound
