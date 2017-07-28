#ifndef PANELPREVIEWSOUND_H
#define PANELPREVIEWSOUND_H

#include "verdigris/wobjectdefs.h"

#include "ui/ui_previewsound.h"

#include "src/common/ustring.h"
#include "src/gui/resourcetreeitem.h"

#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QSpacerItem>
#include <QtMultimedia>
#include <QWidget>

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
    W_SLOT(play, W_Access::Private);
    void pause();
    W_SLOT(pause, W_Access::Private);
    void stop();
    W_SLOT(stop, W_Access::Private);
    void changeVolume(int value);
    W_SLOT(changeVolume, W_Access::Private);
    void positionChanged(qint64 position);
    W_SLOT(positionChanged, W_Access::Private);

    QString formatTime(uint64 t);
    QString formatPercent(uint64 total, uint64 t);
    int getSliderPos(uint64 total, uint64 t);
    void setButtons(bool enablePlay, bool enablePause, bool enableStop);
    void update();
};

#endif // PANELPREVIEWSOUND_H
