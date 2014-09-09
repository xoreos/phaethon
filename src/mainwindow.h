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

/** @file mainwindow.h
 *  Phaethon's main window.
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <map>

#include <wx/wx.h>
#include <wx/splitter.h>

#include <wx/generic/stattextg.h>

#include "common/ustring.h"
#include "common/filetree.h"

#include "sound/types.h"

#include "aurora/types.h"
#include "aurora/archive.h"

namespace Common {
	class SeekableReadStream;
	class WriteStream;
}

namespace Aurora {
	class KEYFile;
	class KEYDataFile;
}

class ResourceTree;
class ResourceTreeItem;

class MainWindow : public wxFrame {
public:
	MainWindow(const wxString &title, const wxPoint &pos, const wxSize &size);
	~MainWindow();

	bool open(Common::UString path);

	void resourceTreeSelect(const ResourceTreeItem *item);

	bool play(const ResourceTreeItem &item);
	void pause();
	void stop();

	Aurora::Archive *getArchive(const boost::filesystem::path &path);

private:
	typedef std::map<Common::UString, Aurora::Archive *> ArchiveMap;
	typedef std::map<Common::UString, Aurora::KEYDataFile *> KEYDataFileMap;

	Common::UString _path;
	Common::FileTree _files;

	ResourceTree *_resourceTree;

	wxSplitterWindow *_splitterInfoPreview;

	wxGenericStaticText *_resInfoName;
	wxGenericStaticText *_resInfoSize;
	wxGenericStaticText *_resInfoFileType;
	wxGenericStaticText *_resInfoResType;

	wxBoxSizer *_sizerExport;

	wxButton *_buttonExportRaw;
	wxButton *_buttonExportBMUMP3;
	wxButton *_buttonExportWAV;

	wxPanel *_panelPreviewEmpty;
	wxPanel *_panelPreviewSound;

	ArchiveMap _archives;
	KEYDataFileMap _keyDataFiles;

	Sound::ChannelHandle _sound;

	void onOpenDir(wxCommandEvent &event);
	void onOpenFile(wxCommandEvent &event);
	void onClose(wxCommandEvent &event);
	void onQuit(wxCommandEvent &event);
	void onAbout(wxCommandEvent &event);

	void onExportRaw(wxCommandEvent &event);
	void onExportBMUMP3(wxCommandEvent &event);
	void onExportWAV(wxCommandEvent &event);

	void onPlay(wxCommandEvent &event);
	void onPause(wxCommandEvent &event);
	void onStop(wxCommandEvent &event);

	void createLayout();
	void forceRedraw();

	void showExportButtons(bool enableRaw, bool showMP3, bool showWAV);
	void showPreviewPanel(wxPanel *panel);
	void showPreviewPanel(Aurora::ResourceType type);

	Common::UString getSizeLabel(uint32 size);
	Common::UString getFileTypeLabel(Aurora::FileType type);
	Common::UString getResTypeLabel(Aurora::ResourceType type);

	void close();

	Common::UString dialogOpenDir(const Common::UString &title);
	Common::UString dialogOpenFile(const Common::UString &title, const Common::UString &mask);
	Common::UString dialogSaveFile(const Common::UString &title, const Common::UString &mask,
	                               const Common::UString &def = "");

	bool exportRaw(const ResourceTreeItem &item, const Common::UString &path);
	bool exportBMUMP3(const ResourceTreeItem &item, const Common::UString &path);
	bool exportWAV(const ResourceTreeItem &item, const Common::UString &path);

	void exportBMUMP3(Common::SeekableReadStream &bmu, Common::WriteStream &mp3);
	void exportWAV(Common::SeekableReadStream *soundData, Common::WriteStream &wav);

	void loadKEYDataFiles(Aurora::KEYFile &key);
	Aurora::KEYDataFile *getKEYDataFile(const Common::UString &file);

	wxDECLARE_EVENT_TABLE();
};

#endif // MAINWINDOW_H
