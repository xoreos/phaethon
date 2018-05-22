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

#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QSpacerItem>
#include <QtMultimedia>
#include <QWidget>

#include "verdigris/wobjectdefs.h"

#include "src/sound/types.h"

namespace GUI {

class ResourceTreeItem;

class PanelPreviewSound : public QFrame {
	W_OBJECT(PanelPreviewSound)

public:
	PanelPreviewSound(QWidget *parent);

	void setItem(const ResourceTreeItem *item);

	void stop();

private:
	QSlider *_sliderPosition;
	QSlider *_sliderVolume;

	QLabel *_labelPosition;
	QLabel *_labelDuration;
	QLabel *_labelPercent;
	QLabel *_labelVolume;

	QPushButton *_buttonPlay;
	QPushButton *_buttonPause;
	QPushButton *_buttonStop;

	const ResourceTreeItem *_currentItem;

	Sound::ChannelHandle _sound;
	uint64 _duration;
	QTimer *_timer;

	bool play();
	void pause();
	void changeVolume(int value);
	void positionChanged(qint64 position);

	QString formatTime(uint64 t) const;
	QString formatPercent(uint64 total, uint64 t) const;
	int     getSliderPos(uint64 total, uint64 t) const;
	void    setButtons(bool enablePlay, bool enablePause, bool enableStop);
	void    update();
};

} // End of namespace GUI

#endif // PANELPREVIEWSOUND_H
