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

#include "src/gui/panelpreviewsound.h"

#include "verdigris/wobjectimpl.h"

#include "src/common/util.h"

namespace GUI {

W_OBJECT_IMPL(PanelPreviewSound)

PanelPreviewSound::PanelPreviewSound(QObject *parent)
{
    _timer = new QTimer(this);

    _layout = new QGridLayout();
    _layout->setObjectName("panelPreviewSoundLayout");
    _volBox = new QVBoxLayout();
    _volBox->setObjectName("panelPreviewSoundLayoutVol");
    _btnBox = new QHBoxLayout();
    _btnBox->setObjectName("panelPreviewSoundLayoutBtns");
    _txtBox = new QHBoxLayout();
    _txtBox->setObjectName("panelPreviewSoundLayoutTxt");

    _volSlider = new QSlider(Qt::Vertical);
    _posSlider = new QSlider(Qt::Horizontal);
    _play = new QPushButton("Play");
    _pause = new QPushButton("Pause");
    _stop = new QPushButton("Stop");
    _textPosition = new QLabel();
    _textDuration = new QLabel();
    _textPercent = new QLabel();
    _textVolume = new QLabel();

    _layout->addLayout(_volBox, 0, 0);
    _volBox->addWidget(_textVolume);
    _volBox->addWidget(_volSlider);

    _layout->addLayout(_btnBox, 0, 1);
    _btnBox->addWidget(_play);
    _btnBox->addWidget(_pause);
    _btnBox->addWidget(_stop);

    _layout->addLayout(_txtBox, 1, 1);
    _txtBox->addWidget(_textPosition);
    _txtBox->addWidget(_textPercent);
    _txtBox->addWidget(_textDuration);

    _layout->addWidget(_posSlider, 2, 1);

    changeVolume(20);

    connect(_play, &QPushButton::clicked, this, &PanelPreviewSound::play);
    connect(_pause, &QPushButton::clicked, this, &PanelPreviewSound::pause);
    connect(_stop, &QPushButton::clicked, this, &PanelPreviewSound::stop);
    connect(_volSlider, &QSlider::valueChanged, this, &PanelPreviewSound::changeVolume);
    connect(_timer, &QTimer::timeout, this, &PanelPreviewSound::update);

    _timer->start(50);

    setLayout(_layout);
}

PanelPreviewSound::~PanelPreviewSound()
{
    delete _layout;
}

void PanelPreviewSound::setItem(ResourceTreeItem *item) {
    if (item == _currentItem)
        return;

    stop();

    if (item->getResourceType() != Aurora::kResourceSound)
        return;

    _currentItem = item;

    if (!item) {
        _duration = Sound::RewindableAudioStream::kInvalidLength;
        return;
    }

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
        Common::ScopedPtr<Sound::AudioStream> sound(_currentItem->getAudioStream());

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
    // fixme: only goes to 99%
    _textVolume->setText(QString("%1%").arg(value));

    _volSlider->setValue(value);

    SoundMan.setListenerGain((double)value / (double)100);
}

void PanelPreviewSound::positionChanged(qint64 position) {
    _posSlider->setValue(position/100);
}

QString PanelPreviewSound::formatTime(uint64 t) {
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

QString PanelPreviewSound::formatPercent(uint64 total, uint64 t) {
    if ((total == Sound::RewindableAudioStream::kInvalidLength) ||
        (t == Sound::RewindableAudioStream::kInvalidLength))
        return "???%";

    if (t == 0)
        return "  0%";

    uint percent = CLIP<uint>((t * 100) / total, 0, 100);

    QString ret;
    return ret.sprintf("%3u%%", percent);
}

int PanelPreviewSound::getSliderPos(uint64 total, uint64 t) {
    if ((total == Sound::RewindableAudioStream::kInvalidLength) ||
        (t == Sound::RewindableAudioStream::kInvalidLength))
        return 0;

    if (t == 0)
        return 0;

    return CLIP<uint>((t * 100) / total, 0, 100);
}

void PanelPreviewSound::setButtons(bool enablePlay, bool enablePause, bool enableStop) {
    _play->setEnabled(enablePlay);
    _pause->setEnabled(enablePause);
    _stop->setEnabled(enableStop);
}

void PanelPreviewSound::update() {
    uint64 t = SoundMan.getChannelDurationPlayed(_sound);

    QString played  = formatTime(t);
    QString total   = formatTime(_duration);
    QString percent = formatPercent(_duration, t);

    _textPosition->setText(played);
    _textPercent->setText(percent);
    _textDuration->setText(total);
//    _textPosition->setText("pos");
//    _textPercent->setText("per%");
//    _textDuration->setText("length");

    _posSlider->setValue(getSliderPos(_duration, t));

    bool isPlaying = SoundMan.isPlaying(_sound);
    bool isPaused  = SoundMan.isPaused(_sound);

    setButtons(!isPlaying || isPaused, isPlaying && !isPaused, isPlaying);
}

} // End of namespace GUI
