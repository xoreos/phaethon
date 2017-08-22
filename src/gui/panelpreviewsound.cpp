#include "verdigris/wobjectimpl.h"

#include "src/common/system.h"
#include "src/common/util.h"
#include "src/gui/panelpreviewsound.h"
#include "src/gui/resourcetreeitem.h"

namespace GUI {

W_OBJECT_IMPL(PanelPreviewSound)

PanelPreviewSound::PanelPreviewSound(QWidget *UNUSED(parent)) {
    _layoutGrid = new QGridLayout(this);
    _layoutHorizontal = new QHBoxLayout();
    _layoutHorizontal_2 = new QHBoxLayout();
    _layoutVertical = new QVBoxLayout();

    _timer = new QTimer(this);

    _labelVolume = new QLabel(this);
    _labelPosition = new QLabel(this);
    _labelPercent = new QLabel(this);
    _labelDuration = new QLabel(this);

    _buttonPlay = new QPushButton(tr("Play"), this);
    _buttonPause = new QPushButton(tr("Pause"), this);
    _buttonStop = new QPushButton(tr("Stop"), this);

    _sliderPosition = new QSlider(this);
    _sliderVolume = new QSlider(this);

    _sliderPosition->setOrientation(Qt::Horizontal);

    _layoutHorizontal->addWidget(_labelPosition);
    _layoutHorizontal->addWidget(_labelPercent);
    _layoutHorizontal->addWidget(_labelDuration);

    _layoutHorizontal_2->addWidget(_buttonPlay);
    _layoutHorizontal_2->addWidget(_buttonPause);
    _layoutHorizontal_2->addWidget(_buttonStop);

    _layoutVertical->addWidget(_labelVolume);
    _layoutVertical->addWidget(_sliderVolume);

    _layoutGrid->addWidget(_sliderPosition,     0, 0);
    _layoutGrid->addLayout(_layoutHorizontal,   1, 0);
    _layoutGrid->addLayout(_layoutVertical,     0, 1);
    _layoutGrid->addLayout(_layoutHorizontal_2, 2, 0);

    _layoutGrid->setSizeConstraint(QLayout::SetFixedSize);

    _sliderVolume->setMaximum(100);

    changeVolume(5);

    connect(_buttonPlay, &QPushButton::clicked, this, &PanelPreviewSound::play);
    connect(_buttonPause, &QPushButton::clicked, this, &PanelPreviewSound::pause);
    connect(_buttonStop, &QPushButton::clicked, this, &PanelPreviewSound::stop);
    connect(_sliderVolume, &QSlider::valueChanged, this, &PanelPreviewSound::changeVolume);
    connect(_timer, &QTimer::timeout, this, &PanelPreviewSound::update);

    _timer->start(50);
}

void PanelPreviewSound::setItem(const ResourceTreeItem *item) {
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
    _sliderVolume->setValue(value);
    _labelVolume->setText(QString("%1%").arg(_sliderVolume->value()));

    SoundMan.setListenerGain((double)value / (double)100);
}

void PanelPreviewSound::positionChanged(qint64 position) {
    _sliderPosition->setValue(position/100);
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
