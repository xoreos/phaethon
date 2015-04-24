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
 *  Preview panel for sound resources.
 */

#ifndef PANELPREVIEWSOUND_H
#define PANELPREVIEWSOUND_H

#include <wx/panel.h>

#include "common/types.h"
#include "common/ustring.h"

#include "sound/types.h"

class wxGenericStaticText;
class wxSlider;
class wxButton;
class wxTimer;

class wxCommandEvent;
class wxTimerEvent;
class wxScrollEvent;

namespace GUI {

class ResourceTreeItem;

class PanelPreviewSound : public wxPanel {
public:
	PanelPreviewSound(wxWindow *parent, const Common::UString &title);
	~PanelPreviewSound();

	void setCurrentItem(const ResourceTreeItem *item);

	bool play();
	void pause();
	void stop();

private:
	Sound::ChannelHandle _sound;

	const ResourceTreeItem *_currentItem;
	uint64 _duration;

	wxGenericStaticText *_textPosition;
	wxGenericStaticText *_textPercent;
	wxGenericStaticText *_textDuration;
	wxGenericStaticText *_textVolume;

	wxSlider *_sliderPosition;
	wxSlider *_sliderVolume;

	wxButton *_buttonPlay;
	wxButton *_buttonPause;
	wxButton *_buttonStop;

	wxTimer *_timer;


	void onPlay(wxCommandEvent &event);
	void onPause(wxCommandEvent &event);
	void onStop(wxCommandEvent &event);

	void onTimer(wxTimerEvent &event);

	void onVolumeChange(wxScrollEvent &event);


	void update();

	void setVolume();


	void createLayout(const Common::UString &title);

	void setButtons(bool enablePlay, bool enablePause, bool enableStop);

	static Common::UString formatTime(uint64 t);
	static Common::UString formatPercent(uint64 total, uint64 t);

	static int getSliderPos(uint64 total, uint64 t);

	wxDECLARE_EVENT_TABLE();
};

} // End of namespace GUI

#endif // PANELPREVIEWSOUND_H
