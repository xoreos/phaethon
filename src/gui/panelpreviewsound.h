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

#include "verdigris/wobjectdefs.h"

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

namespace GUI {

class PanelPreviewSound : public QFrame
{
    W_OBJECT(PanelPreviewSound)
private:
    QGridLayout *_layout;
    QVBoxLayout *_volBox;
    QHBoxLayout *_btnBox;
    QHBoxLayout *_txtBox;

    ResourceTreeItem *_currentItem = nullptr;

    QSlider *_volSlider = nullptr;
    QSlider *_posSlider = nullptr;
    QPushButton *_play = nullptr;
    QPushButton *_pause = nullptr;
    QPushButton *_stop = nullptr;
    QLabel *_textPosition;
    QLabel *_textPercent;
    QLabel *_textDuration;
    QLabel *_textVolume;

    Sound::ChannelHandle _sound;
    uint64 _duration;
    QTimer *_timer;

public:
    PanelPreviewSound(QObject *parent = 0);
    ~PanelPreviewSound();

    void setItem(ResourceTreeItem *node);

    bool play();
    void pause();
    void stop();
    void changeVolume(int value);
    void positionChanged(qint64 position);

    QString formatTime(uint64 t);
    QString formatPercent(uint64 total, uint64 t);
    int getSliderPos(uint64 total, uint64 t);
    void setButtons(bool enablePlay, bool enablePause, bool enableStop);
    void update();
};

} // End of namespace GUI

#endif // PANELPREVIEWSOUND_H
