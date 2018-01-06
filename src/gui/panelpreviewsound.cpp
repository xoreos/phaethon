#include "src/gui/panelpreviewsound.h"

#include "verdigris/wobjectimpl.h"

#include "src/common/util.h"

namespace GUI {

W_OBJECT_IMPL(PanelPreviewSound)

PanelPreviewSound::PanelPreviewSound(QWidget *parent)
{
    _ui.setupUi(this);
    _ui.volSlider->setMaximum(100);

    _timer = new QTimer(this);

    changeVolume(20);

    connect(_ui.play, &QPushButton::clicked, this, &PanelPreviewSound::play);
    connect(_ui.pause, &QPushButton::clicked, this, &PanelPreviewSound::pause);
    connect(_ui.stop, &QPushButton::clicked, this, &PanelPreviewSound::stop);
    connect(_ui.volSlider, &QSlider::valueChanged, this, &PanelPreviewSound::changeVolume);
    connect(_timer, &QTimer::timeout, this, &PanelPreviewSound::update);

    _timer->start(50);
}

PanelPreviewSound::~PanelPreviewSound()
{
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
    _ui.volSlider->setValue(value);
    _ui.volume->setText(QString("%1%").arg(_ui.volSlider->value()));

    SoundMan.setListenerGain((double)value / (double)100);
}

void PanelPreviewSound::positionChanged(qint64 position) {
    _ui.posSlider->setValue(position/100);
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
    _ui.play->setEnabled(enablePlay);
    _ui.pause->setEnabled(enablePause);
    _ui.stop->setEnabled(enableStop);
}

void PanelPreviewSound::update() {
    uint64 t = SoundMan.getChannelDurationPlayed(_sound);

    QString played  = formatTime(t);
    QString total   = formatTime(_duration);
    QString percent = formatPercent(_duration, t);

    _ui.position->setText(played);
    _ui.percent->setText(percent);
    _ui.duration->setText(total);

    _ui.posSlider->setValue(getSliderPos(_duration, t));

    bool isPlaying = SoundMan.isPlaying(_sound);
    bool isPaused  = SoundMan.isPaused(_sound);

    setButtons(!isPlaying || isPaused, isPlaying && !isPaused, isPlaying);
}

} // End of namespace GUI
