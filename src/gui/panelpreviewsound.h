    #ifndef PANELPREVIEWSOUND_H
#define PANELPREVIEWSOUND_H

#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QSpacerItem>
#include <QtMultimedia>
#include <QWidget>

#include "verdigris/wobjectdefs.h"

#include "src/common/ustring.h"
#include "src/sound/types.h"

#include "ui/ui_previewsound.h"

namespace GUI {

class ResourceTreeItem;

class PanelPreviewSound : public QFrame
{
    W_OBJECT(PanelPreviewSound)

private:
    const ResourceTreeItem *_currentItem;

    Sound::ChannelHandle _sound;
    uint64 _duration;
    QTimer *_timer;

    Ui::PreviewSound _ui;

public:
    PanelPreviewSound(QWidget *parent = 0);

    void setItem(const ResourceTreeItem *node);

    bool play();
    void pause();
    void stop();
    void changeVolume(int value);
    void positionChanged(qint64 position);

    QString formatTime(uint64 t) const;
    QString formatPercent(uint64 total, uint64 t) const;
    int getSliderPos(uint64 total, uint64 t) const;
    void setButtons(bool enablePlay, bool enablePause, bool enableStop);
    void update();
};

} // End of namespace GUI

#endif // PANELPREVIEWSOUND_H
