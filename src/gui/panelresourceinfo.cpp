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

/** @file gui/panelresourceinfo.cpp
 *  Panel showing general information and actions on resources.
 */

#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/button.h>
#include <wx/filedlg.h>

#include <wx/generic/stattextg.h>

#include "common/util.h"
#include "common/error.h"
#include "common/stream.h"
#include "common/file.h"
#include "common/filepath.h"

#include "sound/sound.h"
#include "sound/audiostream.h"

#include "aurora/util.h"

#include "gui/panelresourceinfo.h"
#include "gui/eventid.h"
#include "gui/mainwindow.h"
#include "gui/resourcetree.h"

namespace GUI {

wxBEGIN_EVENT_TABLE(PanelResourceInfo, wxPanel)
	EVT_BUTTON(kEventButtonExportRaw   , PanelResourceInfo::onExportRaw)
	EVT_BUTTON(kEventButtonExportBMUMP3, PanelResourceInfo::onExportBMUMP3)
	EVT_BUTTON(kEventButtonExportWAV   , PanelResourceInfo::onExportWAV)
wxEND_EVENT_TABLE()

PanelResourceInfo::PanelResourceInfo(wxWindow *parent, MainWindow &mainWindow, const Common::UString &title) :
	wxPanel(parent, wxID_ANY), _mainWindow(&mainWindow), _currentItem(0) {

	createLayout(title);
}

PanelResourceInfo::~PanelResourceInfo() {
}

void PanelResourceInfo::createLayout(const Common::UString &title) {
	wxStaticBox *boxInfo = new wxStaticBox(this, wxID_ANY, title);
	boxInfo->Lower();

	wxStaticBoxSizer *sizerInfo = new wxStaticBoxSizer(boxInfo, wxVERTICAL);

	_textName     = new wxGenericStaticText(this, wxID_ANY, wxEmptyString);
	_textSize     = new wxGenericStaticText(this, wxID_ANY, wxEmptyString);
	_textFileType = new wxGenericStaticText(this, wxID_ANY, wxEmptyString);
	_textResType  = new wxGenericStaticText(this, wxID_ANY, wxEmptyString);

	_buttonExportRaw    = new wxButton(this, kEventButtonExportRaw   , wxT("Save"));
	_buttonExportBMUMP3 = new wxButton(this, kEventButtonExportBMUMP3, wxT("Export as MP3"));
	_buttonExportWAV    = new wxButton(this, kEventButtonExportWAV   , wxT("Export as PCM WAV"));

	_buttonExportRaw->Disable();
	_buttonExportBMUMP3->Hide();
	_buttonExportWAV->Hide();

	_sizerExport = new wxBoxSizer(wxHORIZONTAL);
	_sizerExport->Add(_buttonExportRaw   , 0, wxEXPAND, 0);
	_sizerExport->Add(_buttonExportBMUMP3, 0, wxEXPAND, 0);
	_sizerExport->Add(_buttonExportWAV   , 0, wxEXPAND, 0);

	sizerInfo->Add(_textName    , 0, wxEXPAND, 0);
	sizerInfo->Add(_textSize    , 0, wxEXPAND, 0);
	sizerInfo->Add(_textFileType, 0, wxEXPAND, 0);
	sizerInfo->Add(_textResType , 0, wxEXPAND, 0);

	sizerInfo->Add(_sizerExport, 0, wxEXPAND | wxTOP, 5);

	SetSizer(sizerInfo);
}

void PanelResourceInfo::setCurrentItem(const ResourceTreeItem *item) {
	if (item == _currentItem)
		return;

	_currentItem = item;
	update();
}

void PanelResourceInfo::showExportButtons(bool enableRaw, bool showMP3, bool showWAV) {
	_buttonExportRaw->Enable(enableRaw);
	_buttonExportBMUMP3->Show(showMP3);
	_buttonExportWAV->Show(showWAV);
}

Common::UString PanelResourceInfo::constructStatus(const Common::UString &action,
		const Common::UString &name, const Common::UString &destination) {

	return action + " \"" + name + "\" to \"" + destination + "\"...";
}

Common::UString PanelResourceInfo::getSizeLabel(uint32 size) {
	if (size == Common::kFileInvalid)
		return "-";

	if (size < 1024)
		return Common::UString::sprintf("%u", size);

	Common::UString humanRead = Common::FilePath::getHumanReadableSize(size);

	return Common::UString::sprintf("%s (%u)", humanRead.c_str(), size);
}

Common::UString PanelResourceInfo::getFileTypeLabel(Aurora::FileType type) {
	Common::UString label = Common::UString::sprintf("%d", type);
	if (type != Aurora::kFileTypeNone)
		label += Common::UString::sprintf(" (%s)", TypeMan.getExtension(type).c_str());

	return label;
}

Common::UString PanelResourceInfo::getResTypeLabel(Aurora::ResourceType type) {
	Common::UString label = Common::UString::sprintf("%d", type);
	if (type != Aurora::kResourceNone)
		label += Common::UString::sprintf(" (%s)", getResourceTypeDescription(type).c_str());

	return label;
}

Common::UString PanelResourceInfo::dialogSaveFile(
		const Common::UString &title, const Common::UString &mask, const Common::UString &def) {

	wxFileDialog dialog(this, title, wxEmptyString, def, mask, wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
	if (dialog.ShowModal() == wxID_OK)
		return dialog.GetPath();

	return "";
}

void PanelResourceInfo::onExportRaw(wxCommandEvent &event) {
	if (!_currentItem)
		return;

	const Common::UString title = "Save Aurora game resource file";
	const Common::UString mask  = "Aurora game resource (*.*)|*.*";
	const Common::UString def   = _currentItem->getName();

	exportRaw(dialogSaveFile(title, mask, def));
}

void PanelResourceInfo::onExportBMUMP3(wxCommandEvent &event) {
	if (!_currentItem)
		return;

	assert(_currentItem->getFileType() == Aurora::kFileTypeBMU);

	const Common::UString title = "Save MP3 file";
	const Common::UString mask  = "MP3 file (*.mp3)|*.mp3";
	const Common::UString def   = TypeMan.setFileType(_currentItem->getName(), Aurora::kFileTypeMP3);

	exportBMUMP3(dialogSaveFile(title, mask, def));
}

void PanelResourceInfo::onExportWAV(wxCommandEvent &event) {
	if (!_currentItem)
		return;

	assert(_currentItem->getResourceType() == Aurora::kResourceSound);

	const Common::UString title = "Save PCM WAV file";
	const Common::UString mask  = "WAV file (*.wav)|*.wav";
	const Common::UString def   = TypeMan.setFileType(_currentItem->getName(), Aurora::kFileTypeWAV);

	exportWAV(dialogSaveFile(title, mask, def));
}

bool PanelResourceInfo::exportRaw(const Common::UString &path) {
	if (!_currentItem || path.empty())
		return false;

	_mainWindow->pushStatus(constructStatus("Saving", _currentItem->getName(), path));

	Common::SeekableReadStream *res = 0;
	try {
		res = _currentItem->getResourceData();

		Common::DumpFile file(path);

		file.writeStream(*res);

		if (!file.flush() || file.err())
			throw Common::Exception(Common::kWriteError);

		delete res;

	} catch (Common::Exception &e) {
		delete res;

		_mainWindow->popStatus();
		Common::printException(e, "WARNING: ");
		return false;
	}

	_mainWindow->popStatus();
	return true;
}

bool PanelResourceInfo::exportBMUMP3(const Common::UString &path) {
	if (!_currentItem || path.empty())
		return false;

	_mainWindow->pushStatus(constructStatus("Exporting", _currentItem->getName(), path));

	Common::SeekableReadStream *res = 0;
	try {
		res = _currentItem->getResourceData();

		Common::DumpFile file(path);

		exportBMUMP3(*res, file);

		if (!file.flush() || file.err())
			throw Common::Exception(Common::kWriteError);

		delete res;

	} catch (Common::Exception &e) {
		delete res;

		_mainWindow->popStatus();
		Common::printException(e, "WARNING: ");
		return false;
	}

	_mainWindow->popStatus();
	return true;
}

bool PanelResourceInfo::exportWAV(const Common::UString &path) {
	if (!_currentItem || path.empty())
		return false;

	_mainWindow->pushStatus(constructStatus("Exporting", _currentItem->getName(), path));

	Common::SeekableReadStream *res = 0;
	Common::DumpFile *file = 0;
	try {
		res  = _currentItem->getResourceData();
		file = new Common::DumpFile(path);
	} catch (Common::Exception &e) {
		delete res;

		_mainWindow->popStatus();
		Common::printException(e, "WARNING: ");
		return false;
	}

	try {
		exportWAV(res, *file);

		if (!file->flush() || file->err())
			throw Common::Exception(Common::kWriteError);

	} catch (Common::Exception &e) {
		delete file;

		_mainWindow->popStatus();
		Common::printException(e, "WARNING: ");
		return false;
	}

	delete file;

	_mainWindow->popStatus();
	return true;
}

void PanelResourceInfo::exportBMUMP3(Common::SeekableReadStream &bmu, Common::WriteStream &mp3) {
	if ((bmu.size() <= 8) ||
	    (bmu.readUint32BE() != MKTAG('B', 'M', 'U', ' ')) ||
	    (bmu.readUint32BE() != MKTAG('V', '1', '.', '0')))
		throw Common::Exception("Not a valid BMU file");

	mp3.writeStream(bmu);
}

struct SoundBuffer {
	int16 buffer[4096];
	int samples;

	SoundBuffer() : samples(0) {
	}
};

void PanelResourceInfo::exportWAV(Common::SeekableReadStream *soundData, Common::WriteStream &wav) {
	Sound::AudioStream *sound = 0;
	try {
		sound = SoundMan.makeAudioStream(soundData);
	} catch (Common::Exception &e) {
		delete soundData;
		throw;
	}

	const uint16 channels = sound->getChannels();
	const uint32 rate     = sound->getRate();

	std::deque<SoundBuffer> buffers;

	uint64 length = Sound::RewindableAudioStream::kInvalidLength;
	Sound::RewindableAudioStream *rewSound = dynamic_cast<Sound::RewindableAudioStream *>(sound);
	if (rewSound)
		length = rewSound->getLength();

	if (length != Sound::RewindableAudioStream::kInvalidLength)
		buffers.resize((length / (ARRAYSIZE(SoundBuffer::buffer) / channels)) + 1);

	uint32 samples = 0;
	std::deque<SoundBuffer>::iterator buffer = buffers.begin();
	while (!sound->endOfStream()) {
		if (buffer == buffers.end()) {
			buffers.push_back(SoundBuffer());
			buffer = --buffers.end();
		}

		try {
			buffer->samples = sound->readBuffer(buffer->buffer, 4096);
		} catch (Common::Exception &e) {
			delete sound;
			throw;
		}

		if (buffer->samples > 0)
			samples += buffer->samples;

		++buffer;
	}

	delete sound;

	samples /= channels;

	const uint32 dataSize   = samples * channels * 2;
	const uint32 byteRate   = rate * channels * 2;
	const uint16 blockAlign = channels * 2;

	wav.writeUint32BE(MKTAG('R', 'I', 'F', 'F'));
	wav.writeUint32LE(36 + dataSize);
	wav.writeUint32BE(MKTAG('W', 'A', 'V', 'E'));

	wav.writeUint32BE(MKTAG('f', 'm', 't', ' '));
	wav.writeUint32LE(16);
	wav.writeUint16LE(1);
	wav.writeUint16LE(channels);
	wav.writeUint32LE(rate);
	wav.writeUint32LE(byteRate);
	wav.writeUint16LE(blockAlign);
	wav.writeUint16LE(16);

	wav.writeUint32BE(MKTAG('d', 'a', 't', 'a'));
	wav.writeUint32LE(dataSize);

	for (std::deque<SoundBuffer>::const_iterator b = buffers.begin(); b != buffers.end(); ++b)
		for (int i = 0; i < b->samples; i++)
			wav.writeUint16LE(b->buffer[i]);
}

void PanelResourceInfo::update() {
	Common::UString labelName     = "Resource name: ";
	Common::UString labelSize     = "Size: ";
	Common::UString labelFileType = "File type: ";
	Common::UString labelResType  = "Resource type: ";

	if (_currentItem) {
		labelName += _currentItem->getName();

		if (_currentItem->getSource() == ResourceTreeItem::kSourceDirectory) {

			showExportButtons(false, false, false);

			labelSize     += "-";
			labelFileType += "Directory";
			labelResType  += "Directory";

		} else if ((_currentItem->getSource() == ResourceTreeItem::kSourceFile) ||
		           (_currentItem->getSource() == ResourceTreeItem::kSourceArchiveFile)) {

			Aurora::FileType     fileType = _currentItem->getFileType();
			Aurora::ResourceType resType  = _currentItem->getResourceType();

			labelSize     += getSizeLabel(_currentItem->getSize());
			labelFileType += getFileTypeLabel(fileType);
			labelResType  += getResTypeLabel(resType);

			showExportButtons(true, fileType == Aurora::kFileTypeBMU, resType == Aurora::kResourceSound);

		}
	} else {
		showExportButtons(false, false, false);
	}

	_textName->SetLabel(labelName);
	_textSize->SetLabel(labelSize);
	_textFileType->SetLabel(labelFileType);
	_textResType->SetLabel(labelResType);

	_sizerExport->Layout();
}

} // End of namespace GUI
