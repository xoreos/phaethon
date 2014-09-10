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

/** @file soundcontrols.cpp
 *  Controls for handling sound playback / preview.
 */

#include "common/util.h"

#include "sound/sound.h"
#include "sound/audiostream.h"

#include "soundcontrols.h"

SoundTimer::SoundTimer(SoundControls &soundCtrl) :
	_duration(Sound::RewindableAudioStream::kInvalidLength), _soundCtrl(&soundCtrl) {

}

SoundTimer::~SoundTimer() {
}

void SoundTimer::setDuration(uint64 duration) {
	_duration = duration;
}

void SoundTimer::Notify() {
	uint64 t = SoundMan.getChannelDurationPlayed(_soundCtrl->sound);

	Common::UString played  = formatTime(t);
	Common::UString total   = formatTime(_duration);
	Common::UString percent = formatPercent(_duration, t);

	_soundCtrl->textPosition->SetLabelMarkup(Common::UString::sprintf("<tt>%s</tt>", played.c_str()));
	_soundCtrl->textPercent->SetLabelMarkup(Common::UString::sprintf("<tt>%s</tt>", percent.c_str()));
	_soundCtrl->textDuration->SetLabelMarkup(Common::UString::sprintf("<tt>%s</tt>", total.c_str()));

	_soundCtrl->sliderPosition->SetValue(getSliderPos(_duration, t));

	bool isPlaying = SoundMan.isPlaying(_soundCtrl->sound);
	bool isPaused  = SoundMan.isPaused(_soundCtrl->sound);

	setButtons(!isPlaying || isPaused, isPlaying && !isPaused, isPlaying);
}

void SoundTimer::setButtons(bool enablePlay, bool enablePause, bool enableStop) {
	_soundCtrl->buttonPlay->Enable(enablePlay);
	_soundCtrl->buttonPause->Enable(enablePause);
	_soundCtrl->buttonStop->Enable(enableStop);
}

Common::UString SoundTimer::formatTime(uint64 t) {
	if (t == Sound::RewindableAudioStream::kInvalidLength)
		return "??:??:??.???";

	uint32 ms = t % 1000;

	t /= 1000;

	uint32 s = t % 60;

	t /= 60;

	uint32 m = t % 60;

	t /= 60;

	uint32 h = t;

	return Common::UString::sprintf("%02u:%02u:%02u.%03u", h, m, s, ms);
}

Common::UString SoundTimer::formatPercent(uint64 total, uint64 t) {
	if ((total == Sound::RewindableAudioStream::kInvalidLength) ||
	    (t == Sound::RewindableAudioStream::kInvalidLength))
		return "???%";

	if (t == 0)
		return "  0%";

	uint percent = CLIP<uint>((t * 100) / total, 0, 100);

	return Common::UString::sprintf("%3u%%", percent);
}

int SoundTimer::getSliderPos(uint64 total, uint64 t) {
	if ((total == Sound::RewindableAudioStream::kInvalidLength) ||
	    (t == Sound::RewindableAudioStream::kInvalidLength))
		return 0;

	if (t == 0)
		return 0;

	return CLIP<int>((t * 10000) / total, 0, 10000);
}
