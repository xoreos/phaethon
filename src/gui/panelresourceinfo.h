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

#ifndef PANELRESOURCEINFO_H
#define PANELRESOURCEINFO_H

#include <wx/panel.h>

#include "common/ustring.h"

#include "aurora/types.h"

namespace Common {
	class SeekableReadStream;
	class WriteStream;
}

namespace Sound {
	class AudioStream;
}

class wxBoxSizer;
class wxGenericStaticText;
class wxButton;

class wxCommandEvent;

namespace GUI {

class MainWindow;
class ResourceTreeItem;

class PanelResourceInfo : public wxPanel {
public:
	PanelResourceInfo(wxWindow *parent, MainWindow &mainWindow, const Common::UString &title);
	~PanelResourceInfo();

	void setCurrentItem(const ResourceTreeItem *item);

private:
	MainWindow *_mainWindow;

	const ResourceTreeItem *_currentItem;

	wxGenericStaticText *_textName;
	wxGenericStaticText *_textSize;
	wxGenericStaticText *_textFileType;
	wxGenericStaticText *_textResType;

	wxBoxSizer *_sizerExport;

	wxButton *_buttonExportRaw;
	wxButton *_buttonExportBMUMP3;
	wxButton *_buttonExportWAV;
	wxButton *_buttonExportTGA;


	void onExportRaw(wxCommandEvent &event);
	void onExportBMUMP3(wxCommandEvent &event);
	void onExportWAV(wxCommandEvent &event);
	void onExportTGA(wxCommandEvent &event);


	void createLayout(const Common::UString &title);


	bool exportRaw(const Common::UString &path);
	bool exportBMUMP3(const Common::UString &path);
	bool exportWAV(const Common::UString &path);
	bool exportTGA(const Common::UString &path);

	void exportBMUMP3(Common::SeekableReadStream &bmu, Common::WriteStream &mp3);
	void exportWAV(Sound::AudioStream *sound, Common::WriteStream &wav);


	void update();
	void setLabels();
	void showExportButtons();
	void showExportButtons(bool enableRaw, bool showMP3, bool showWAV, bool showTGA);


	static uint64 getLength(Sound::AudioStream *sound);

	static Common::UString constructStatus(const Common::UString &action,
			const Common::UString &name, const Common::UString &destination);

	static Common::UString getSizeLabel(uint32 size);
	static Common::UString getFileTypeLabel(Aurora::FileType type);
	static Common::UString getResTypeLabel(Aurora::ResourceType type);

	Common::UString dialogSaveFile(const Common::UString &title, const Common::UString &mask,
	                               const Common::UString &def = "");

	wxDECLARE_EVENT_TABLE();
};

} // End of namespace GUI

#endif // PANELRESOURCEINFO_H
