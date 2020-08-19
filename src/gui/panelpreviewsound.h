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

#ifndef GUI_PANELPREVIEWSOUND_H
#define GUI_PANELPREVIEWSOUND_H

#include "src/gui/panelbase.h"

#include "src/sound/types.h"

namespace GUI {

class ResourceTreeItem;

class PanelPreviewSound : public PanelBase {
	W_OBJECT(PanelPreviewSound)

public:
	PanelPreviewSound(QWidget *parent);

	virtual void show(const ResourceTreeItem *item);

	void stop();

private:
	QSlider *_sliderPosition { nullptr };
	QSlider *_sliderVolume { nullptr };

	QLabel *_labelPosition { nullptr };
	QLabel *_labelDuration { nullptr };
	QLabel *_labelPercent { nullptr };
	QLabel *_labelVolume { nullptr };

	QPushButton *_buttonPlay { nullptr };
	QPushButton *_buttonPause { nullptr };
	QPushButton *_buttonStop { nullptr };

	const ResourceTreeItem *_currentItem { nullptr };

	Sound::ChannelHandle _sound;
	uint64_t _duration { 0 };
	QTimer *_timer { nullptr };

	bool play();
	void pause();
	void changeVolume(int value);
	void positionChanged(qint64 position);

	QString formatTime(uint64_t t) const;
	QString formatPercent(uint64_t total, uint64_t t) const;
	int     getSliderPos(uint64_t total, uint64_t t) const;
	void    setButtons(bool enablePlay, bool enablePause, bool enableStop);
	void    update();
};

} // End of namespace GUI

#endif // GUI_PANELPREVIEWSOUND_H
