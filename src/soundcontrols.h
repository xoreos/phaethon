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

/** @file soundcontrols.h
 *  Controls for handling sound playback / preview.
 */

#ifndef SOUNDCONTROLS_H
#define SOUNDCONTROLS_H

#include <wx/timer.h>
#include <wx/slider.h>
#include <wx/button.h>

#include <wx/generic/stattextg.h>

#include "common/types.h"
#include "common/ustring.h"

#include "sound/types.h"

struct SoundControls {
	Sound::ChannelHandle sound;

	wxGenericStaticText *textPosition;
	wxGenericStaticText *textPercent;
	wxGenericStaticText *textDuration;

	wxSlider *sliderPosition;

	wxButton *buttonPlay;
	wxButton *buttonPause;
	wxButton *buttonStop;
};

class SoundTimer : public wxTimer {
public:
	SoundTimer(SoundControls &soundCtrl);
	~SoundTimer();

	void setDuration(uint64 duration);

	void Notify();

private:
	uint64 _duration;

	SoundControls *_soundCtrl;

	void setButtons(bool enablePlay, bool enablePause, bool enableStop);

	static Common::UString formatTime(uint64 t);
	static Common::UString formatPercent(uint64 total, uint64 t);

	static int getSliderPos(uint64 total, uint64 t);
};

#endif // SOUNDCONTROLS_H
