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

#include <wx/wx.h>
#include <wx/treectrl.h>

#include <wx/generic/stattextg.h>

#include "common/ustring.h"
#include "common/filetree.h"

#include "aurora/types.h"

class ResourceTreeItem : public wxTreeItemData {
public:
	ResourceTreeItem(const Common::FileTree::Entry &entry);
	~ResourceTreeItem();

	const Common::FileTree::Entry &getEntry() const;

	Aurora::FileType getFileType() const;
	Aurora::ResourceType getResourceType() const;

private:
	const Common::FileTree::Entry &_entry;
};

class MainWindow;

class ResourceTree : public wxTreeCtrl {
wxDECLARE_DYNAMIC_CLASS(ResourceTree);

public:
	enum Image {
		kImageDir  = 0,
		kImageFile,
		kImageMAX
	};

	ResourceTree();
	ResourceTree(wxWindow *parent, MainWindow &mainWindow);
	~ResourceTree();

	int OnCompareItems(const wxTreeItemId &item1, const wxTreeItemId &item2);

	void onSelChanged(wxTreeEvent &event);

private:
	MainWindow *_mainWindow;

	wxDECLARE_EVENT_TABLE();
};

class MainWindow : public wxFrame {
public:
	MainWindow(const wxString &title, const wxPoint &pos, const wxSize &size);
	~MainWindow();

	bool open(Common::UString path);

	void resourceTreeSelect(const ResourceTreeItem *item);

private:
	Common::UString _path;
	Common::FileTree _files;

	ResourceTree *_resourceTree;

	wxGenericStaticText *_resInfoName;
	wxGenericStaticText *_resInfoFileType;
	wxGenericStaticText *_resInfoResType;


	void onOpenDir(wxCommandEvent &event);
	void onOpenFile(wxCommandEvent &event);
	void onClose(wxCommandEvent &event);
	void onQuit(wxCommandEvent &event);
	void onAbout(wxCommandEvent &event);

	void forceRedraw();

	void close();
	void populateTree();
	void populateTree(const Common::FileTree::Entry &e, wxTreeItemId t);

	wxDECLARE_EVENT_TABLE();
};

#endif // MAINWINDOW_H
