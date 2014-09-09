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

// Largely based on the AudioStream implementation found in ScummVM.

/** @file sound/audiostream.cpp
 *  Streaming audio.
 */

#include "sound/audiostream.h"

namespace Sound {

LoopingAudioStream::LoopingAudioStream(RewindableAudioStream *stream, uint loops, bool disposeAfterUse)
    : _parent(stream), _disposeAfterUse(disposeAfterUse), _loops(loops), _completeIterations(0) {
}

LoopingAudioStream::~LoopingAudioStream() {
	if (_disposeAfterUse)
		delete _parent;
}

int LoopingAudioStream::readBuffer(int16 *buffer, const int numSamples) {
	if ((_loops && _completeIterations == _loops) || !numSamples)
		return 0;

	int samplesRead = _parent->readBuffer(buffer, numSamples);

	if (_parent->endOfStream()) {
		++_completeIterations;
		if (_completeIterations == _loops)
			return samplesRead;

		const int remainingSamples = numSamples - samplesRead;

		if (!_parent->rewind()) {
			// TODO: Properly indicate error
			_loops = _completeIterations = 1;
			return samplesRead;
		}

		return samplesRead + readBuffer(buffer + samplesRead, remainingSamples);
	}

	return samplesRead;
}

bool LoopingAudioStream::endOfData() const {
	return (_loops != 0 && (_completeIterations == _loops));
}

bool LoopingAudioStream::rewind() {
	if (!_parent->rewind())
		return false;

	_completeIterations = 0;
	return true;
}

uint64 LoopingAudioStream::getLength() const {
	if (!_loops)
		return kInvalidLength;

	uint64 length = _parent->getLength();
	if (length == kInvalidLength)
		return kInvalidLength;

	return _loops * length;
}

uint64 LoopingAudioStream::getDuration() const {
	if (!_loops)
		return kInvalidLength;

	uint64 duration = _parent->getDuration();
	if (duration == kInvalidLength)
		return kInvalidLength;

	return _loops * duration;
}

uint64 LoopingAudioStream::getLengthOnce() const {
	return _parent->getLength();
}

} // End of namespace Sound
