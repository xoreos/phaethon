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

#include <wx/sizer.h>
#include <wx/gbsizer.h>
#include <wx/statbox.h>

#include "common/util.h"
#include "common/error.h"
#include "common/stream.h"

#include "sound/sound.h"
#include "sound/audiostream.h"

#include "soundcontrols.h"
#include "eventid.h"
#include "resourcetree.h"

wxBEGIN_EVENT_TABLE(SoundControls, wxPanel)
	EVT_BUTTON(kEventButtonPlay , SoundControls::onPlay)
	EVT_BUTTON(kEventButtonPause, SoundControls::onPause)
	EVT_BUTTON(kEventButtonStop , SoundControls::onStop)

	EVT_TIMER(wxID_ANY, SoundControls::onTimer)

	EVT_COMMAND_SCROLL(kEventSliderVolume, SoundControls::onVolumeChange)
wxEND_EVENT_TABLE()

SoundControls::SoundControls(wxWindow *parent, const Common::UString &title) :
	wxPanel(parent, wxID_ANY), _currentItem(0),
	_duration(Sound::RewindableAudioStream::kInvalidLength), _timer(0) {

	createLayout(title);

	_timer = new wxTimer(this, wxID_ANY);
	_timer->Start(10);
}

SoundControls::~SoundControls() {
	delete _timer;
}

void SoundControls::createLayout(const Common::UString &title) {
	wxStaticBox *boxPreviewSound = new wxStaticBox(this, wxID_ANY, title);
	boxPreviewSound->Lower();

	wxStaticBoxSizer *sizerPreviewSound = new wxStaticBoxSizer(boxPreviewSound, wxVERTICAL);

	_textPosition = new wxGenericStaticText(this, wxID_ANY, wxEmptyString);
	_textPercent  = new wxGenericStaticText(this, wxID_ANY, wxEmptyString);
	_textDuration = new wxGenericStaticText(this, wxID_ANY, wxEmptyString);
	_textVolume   = new wxGenericStaticText(this, wxID_ANY, wxEmptyString);

	_sliderPosition = new wxSlider(this, wxID_ANY, 0, 0, 10000,
	                               wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL);

	_sliderVolume = new wxSlider(this, kEventSliderVolume, 100, 0, 100,
	                             wxDefaultPosition, wxDefaultSize, wxSL_VERTICAL | wxSL_INVERSE);

	_buttonPlay  = new wxButton(this, kEventButtonPlay , wxT("Play"));
	_buttonPause = new wxButton(this, kEventButtonPause, wxT("Pause"));
	_buttonStop  = new wxButton(this, kEventButtonStop , wxT("Stop"));

	_textPosition->SetLabelMarkup(wxT("<tt>00:00:00.000</tt>"));
	_textPercent->SetLabelMarkup(wxT("<tt>???%</tt>"));
	_textDuration->SetLabelMarkup(wxT("<tt>??:??:??.???""</tt>"));
	_textVolume->SetLabelMarkup(wxT("<tt>100%</tt>"));

	_sliderPosition->Disable();

	wxGridBagSizer *sizerSoundControls = new wxGridBagSizer();

	sizerSoundControls->Add(_textPosition, wxGBPosition(0, 0), wxGBSpan(1, 1), wxALIGN_LEFT   | wxBOTTOM, 5);
	sizerSoundControls->Add(_textPercent , wxGBPosition(0, 1), wxGBSpan(1, 1), wxALIGN_CENTER | wxBOTTOM, 5);
	sizerSoundControls->Add(_textDuration, wxGBPosition(0, 2), wxGBSpan(1, 1), wxALIGN_RIGHT  | wxBOTTOM, 5);
	sizerSoundControls->Add(_buttonPlay  , wxGBPosition(2, 0), wxGBSpan(1, 1), wxALIGN_LEFT   | wxTOP   , 5);
	sizerSoundControls->Add(_buttonPause , wxGBPosition(2, 1), wxGBSpan(1, 1), wxALIGN_CENTER | wxTOP   , 5);
	sizerSoundControls->Add(_buttonStop  , wxGBPosition(2, 2), wxGBSpan(1, 1), wxALIGN_RIGHT  | wxTOP   , 5);

	sizerSoundControls->Add(_sliderPosition, wxGBPosition(1, 0), wxGBSpan(1, 3), wxALIGN_CENTER | wxEXPAND, 0);

	sizerSoundControls->Add(_sliderVolume, wxGBPosition(0, 3), wxGBSpan(3, 1), wxALIGN_CENTER | wxEXPAND | wxLEFT, 10);

	sizerSoundControls->Add(_textVolume, wxGBPosition(0, 4), wxGBSpan(3, 1), wxALIGN_CENTER | wxEXPAND | wxLEFT, 5);

	sizerPreviewSound->Add(sizerSoundControls, 0, 0, 0);
	SetSizer(sizerPreviewSound);
}

void SoundControls::setCurrentItem(const ResourceTreeItem *item) {
	if (item == _currentItem)
		return;

	stop();

	_currentItem = item;
	if (!item) {
		_duration = Sound::RewindableAudioStream::kInvalidLength;
		return;
	}

	_duration = item->getSoundDuration();
}

void SoundControls::setButtons(bool enablePlay, bool enablePause, bool enableStop) {
	_buttonPlay->Enable(enablePlay);
	_buttonPause->Enable(enablePause);
	_buttonStop->Enable(enableStop);
}

Common::UString SoundControls::formatTime(uint64 t) {
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

Common::UString SoundControls::formatPercent(uint64 total, uint64 t) {
	if ((total == Sound::RewindableAudioStream::kInvalidLength) ||
	    (t == Sound::RewindableAudioStream::kInvalidLength))
		return "???%";

	if (t == 0)
		return "  0%";

	uint percent = CLIP<uint>((t * 100) / total, 0, 100);

	return Common::UString::sprintf("%3u%%", percent);
}

int SoundControls::getSliderPos(uint64 total, uint64 t) {
	if ((total == Sound::RewindableAudioStream::kInvalidLength) ||
	    (t == Sound::RewindableAudioStream::kInvalidLength))
		return 0;

	if (t == 0)
		return 0;

	return CLIP<int>((t * 10000) / total, 0, 10000);
}

void SoundControls::update() {
	uint64 t = SoundMan.getChannelDurationPlayed(_sound);

	Common::UString played  = formatTime(t);
	Common::UString total   = formatTime(_duration);
	Common::UString percent = formatPercent(_duration, t);

	_textPosition->SetLabelMarkup(Common::UString::sprintf("<tt>%s</tt>", played.c_str()));
	_textPercent->SetLabelMarkup(Common::UString::sprintf("<tt>%s</tt>", percent.c_str()));
	_textDuration->SetLabelMarkup(Common::UString::sprintf("<tt>%s</tt>", total.c_str()));

	_sliderPosition->SetValue(getSliderPos(_duration, t));

	bool isPlaying = SoundMan.isPlaying(_sound);
	bool isPaused  = SoundMan.isPaused(_sound);

	setButtons(!isPlaying || isPaused, isPlaying && !isPaused, isPlaying);
}

void SoundControls::setVolume() {
	double volume = _sliderVolume->GetValue() / (double)_sliderVolume->GetMax();

	Common::UString label = Common::UString::sprintf("<tt>%3d%%</tt>", (int) (volume * 100));
	_textVolume->SetLabelMarkup(label);

	SoundMan.setListenerGain(volume);
}

bool SoundControls::play() {
	if (!_currentItem || (_currentItem->getResourceType() != Aurora::kResourceSound))
		return false;

	Common::SeekableReadStream *res = 0;
	try {
		res = _currentItem->getResourceData();
	} catch (Common::Exception &e) {
		Common::printException(e, "WARNING: ");
		return false;
	}

	if (SoundMan.isPlaying(_sound)) {
		if (SoundMan.isPaused(_sound)) {
			SoundMan.pauseChannel(_sound, false);
			return true;
		}

		SoundMan.stopChannel(_sound);
	}

	try {
		_sound = SoundMan.playSoundFile(res, Sound::kSoundTypeUnknown);
	} catch (Common::Exception &e) {
		delete res;
	}

	SoundMan.startChannel(_sound);
	return true;
}

void SoundControls::pause() {
	if (SoundMan.isPlaying(_sound))
		SoundMan.pauseChannel(_sound);
}

void SoundControls::stop() {
	if (SoundMan.isPlaying(_sound))
		SoundMan.stopChannel(_sound);
}

void SoundControls::onPlay(wxCommandEvent &event) {
	play();
}

void SoundControls::onPause(wxCommandEvent &event) {
	pause();
}

void SoundControls::onStop(wxCommandEvent &event) {
	stop();
}

void SoundControls::onTimer(wxTimerEvent &event) {
	update();
}

void SoundControls::onVolumeChange(wxScrollEvent &event) {
	setVolume();
}

