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

#include <memory>

#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QSpacerItem>
#include <QTimer>
#include <QWidget>

#include "external/verdigris/wobjectimpl.h"

#include "src/common/system.h"
#include "src/common/util.h"

#include "src/gui/panelpreviewsound.h"
#include "src/gui/resourcetreeitem.h"

namespace GUI {

W_OBJECT_IMPL(PanelPreviewSound)

PanelPreviewSound::PanelPreviewSound(QWidget *parent) :
	PanelBase(parent) {
	QGridLayout *layoutTop = new QGridLayout(this);
	QHBoxLayout *layoutLabels = new QHBoxLayout();
	QHBoxLayout *layoutButtons = new QHBoxLayout();
	QVBoxLayout *layoutVolume = new QVBoxLayout();

	_timer = new QTimer(this);

	_labelVolume = new QLabel(this);
	_labelPosition = new QLabel(this);
	_labelPercent = new QLabel(this);
	_labelPercent->setAlignment(Qt::AlignCenter);
	_labelDuration = new QLabel(this);

	_buttonPlay = new QPushButton(tr("Play"), this);
	_buttonPause = new QPushButton(tr("Pause"), this);
	_buttonStop = new QPushButton(tr("Stop"), this);

	_sliderPosition = new QSlider(this);
	_sliderVolume = new QSlider(this);

	_sliderPosition->setOrientation(Qt::Horizontal);

	layoutLabels->addWidget(_labelPosition);
	layoutLabels->addWidget(_labelPercent);
	layoutLabels->addWidget(_labelDuration);

	layoutButtons->addWidget(_buttonPlay);
	layoutButtons->addWidget(_buttonPause);
	layoutButtons->addWidget(_buttonStop);

	layoutVolume->addWidget(_labelVolume);
	layoutVolume->addWidget(_sliderVolume);

	layoutTop->addWidget(_sliderPosition, 0, 0);
	layoutTop->addLayout(layoutLabels,    1, 0);
	layoutTop->addLayout(layoutVolume,    0, 1);
	layoutTop->addLayout(layoutButtons,   2, 0);

	layoutTop->setSizeConstraint(QLayout::SetFixedSize);

	_sliderVolume->setMaximum(100);

	changeVolume(5);

	connect(_buttonPlay, &QPushButton::clicked, this, &PanelPreviewSound::play);
	connect(_buttonPause, &QPushButton::clicked, this, &PanelPreviewSound::pause);
	connect(_buttonStop, &QPushButton::clicked, this, &PanelPreviewSound::stop);
	connect(_sliderVolume, &QSlider::valueChanged, this, &PanelPreviewSound::changeVolume);
	connect(_timer, &QTimer::timeout, this, &PanelPreviewSound::update);

	_timer->start(50);
}

void PanelPreviewSound::show(const ResourceTreeItem *item) {
	PanelBase::show(item);

	stop();
	_currentItem = nullptr;
	_duration = Sound::RewindableAudioStream::kInvalidLength;

	if (!item || (item->getResourceType() != Aurora::kResourceSound))
		return;

	_currentItem = item;
	_duration = item->getSoundDuration();
}

bool PanelPreviewSound::play() {
	if (!_currentItem || (_currentItem->getResourceType() != Aurora::kResourceSound))
		return false;

	if (SoundMan.isPlaying(_sound)) {
		if (SoundMan.isPaused(_sound)) {
			SoundMan.pauseChannel(_sound, false);
			return true;
		}

		SoundMan.stopChannel(_sound);
	}

	try {
		std::unique_ptr<Sound::AudioStream> sound(_currentItem->getAudioStream());

		_sound = SoundMan.playAudioStream(sound.get(), Sound::kSoundTypeUnknown);
		sound.release();

	} catch (Common::Exception &e) {
		Common::printException(e, "WARNING: ");
		return false;
	}

	SoundMan.startChannel(_sound);
	return true;
}

void PanelPreviewSound::pause() {
	if (SoundMan.isPlaying(_sound))
			SoundMan.pauseChannel(_sound);
}

void PanelPreviewSound::stop() {
	if (SoundMan.isPlaying(_sound))
			SoundMan.stopChannel(_sound);
}

void PanelPreviewSound::changeVolume(int value) {
	_sliderVolume->setValue(value);
	_labelVolume->setText(QString("%1%").arg(_sliderVolume->value()));

	SoundMan.setListenerGain((double) value / (double) 100);
}

void PanelPreviewSound::positionChanged(qint64 position) {
	_sliderPosition->setValue(position / 100);
}

QString PanelPreviewSound::formatTime(uint64 t) const {
	if (t == Sound::RewindableAudioStream::kInvalidLength)
		return "??:??:??.???";

	uint32 ms = t % 1000;

	t /= 1000;

	uint32 s = t % 60;

	t /= 60;

	uint32 m = t % 60;

	t /= 60;

	uint32 h = t;

	QString ret;
	return ret.sprintf("%02u:%02u:%02u.%03u", h, m, s, ms);
}

QString PanelPreviewSound::formatPercent(uint64 total, uint64 t) const {
	if ((total == Sound::RewindableAudioStream::kInvalidLength) ||
        (t == Sound::RewindableAudioStream::kInvalidLength))
		return "???%";

	if (t == 0)
		return "  0%";

	uint percent = CLIP<uint>((t * 100) / total, 0, 100);

	QString ret;
	return ret.sprintf("%3u%%", percent);
}

int PanelPreviewSound::getSliderPos(uint64 total, uint64 t) const {
	if ((total == Sound::RewindableAudioStream::kInvalidLength) ||
        (t == Sound::RewindableAudioStream::kInvalidLength))
		return 0;

	if (t == 0)
		return 0;

	return CLIP<uint>((t * 100) / total, 0, 100);
}

void PanelPreviewSound::setButtons(bool enablePlay, bool enablePause, bool enableStop) {
	_buttonPlay->setEnabled(enablePlay);
	_buttonPause->setEnabled(enablePause);
	_buttonStop->setEnabled(enableStop);
}

void PanelPreviewSound::update() {
	uint64 t = SoundMan.getChannelDurationPlayed(_sound);

	QString played  = formatTime(t);
	QString total   = formatTime(_duration);
	QString percent = formatPercent(_duration, t);

	_labelPosition->setText(played);
	_labelPercent->setText(percent);
	_labelDuration->setText(total);

	_sliderPosition->setValue(getSliderPos(_duration, t));

	bool isPlaying = SoundMan.isPlaying(_sound);
	bool isPaused  = SoundMan.isPaused(_sound);

	setButtons(!isPlaying || isPaused, isPlaying && !isPaused, isPlaying);
}

} // End of namespace GUI
