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
 *  Panel showing general information and actions on resources.
 */

#ifndef GUI_PANELRESOURCEINFO_H
#define GUI_PANELRESOURCEINFO_H

#include "verdigris/wobjectdefs.h"

namespace GUI {

class ResourceTreeItem;

class PanelResourceInfo : public QFrame {
	W_OBJECT(PanelResourceInfo)

public:
	PanelResourceInfo(QWidget* parent);

	/** Decides which of the export buttons are required and shows them. */
	void showExportButtons(const GUI::ResourceTreeItem *item);
	void showExportButtons(bool enableRaw, bool showMP3, bool showWAV, bool showTGA);

	/** Updates the labels which display information about the resource. */
	void setLabels(const GUI::ResourceTreeItem *item);

	/** Calls showExportButtons and setLabels. */
	void update(const GUI::ResourceTreeItem *item);

	/** Used when a directory is closed. */
	void clearLabels();
	void setButtonsForClosedDir();

public /*signals*/ :
	void loadModel(const QString &path)
	W_SIGNAL(loadModel, path)

	void log(const QString &text)
	W_SIGNAL(log,       text)

	void closeDirClicked()
	W_SIGNAL(closeDirClicked)

	void saveClicked()
	W_SIGNAL(saveClicked)

	void exportTGAClicked()
	W_SIGNAL(exportTGAClicked)

	void exportBMUMP3Clicked()
	W_SIGNAL(exportBMUMP3Clicked)

	void exportWAVClicked()
	W_SIGNAL(exportWAVClicked)

public /*slots*/ :
	void slotSave();
	void slotExportTGA();
	void slotExportBMUMP3();
	void slotExportWAV();

private:
	QPushButton *_buttonExportRaw;
	QPushButton *_buttonExportBMUMP3;
	QPushButton *_buttonExportTGA;
	QPushButton *_buttonExportWAV;

	QLabel *_labelName;
	QLabel *_labelSize;
	QLabel *_labelFileType;
	QLabel *_labelResType;
};

} // End of namespace GUI

#endif // GUI_PANELRESOURCEINFO_H
