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

#include <QFrame>
#include <QLabel>
#include <QLayout>
#include <QWidget>
#include <QDir>
#include <QFileDialog>
#include <QPushButton>

#include "external/verdigris/wobjectimpl.h"

#include "src/common/filepath.h"
#include "src/common/system.h"
#include "src/common/writefile.h"
#include "src/gui/panelresourceinfo.h"
#include "src/gui/resourcetreeitem.h"

namespace GUI {

W_OBJECT_IMPL(PanelResourceInfo)

PanelResourceInfo::PanelResourceInfo(QWidget *parent) : QFrame(parent) {
	QVBoxLayout *layoutTop = new QVBoxLayout(this);
	QVBoxLayout *layoutLabels = new QVBoxLayout();
	QHBoxLayout *layoutButtons = new QHBoxLayout();

	_buttonExportRaw = new QPushButton(tr("Save"), this);
	_buttonExportBMUMP3 = new QPushButton(tr("Export as MP3"), this);
	_buttonExportTGA = new QPushButton(tr("Export as TGA"), this);
	_buttonExportWAV = new QPushButton(tr("Export as WAV"),this);

	_labelName = new QLabel(tr("Resource name:"), this);
	_labelSize = new QLabel(tr("Size:"), this);
	_labelFileType = new QLabel(tr("File type:"), this);
	_labelResType = new QLabel(tr("Resource type:"), this);

	_labelName->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);

	layoutLabels->addWidget(_labelName);
	layoutLabels->addWidget(_labelSize);
	layoutLabels->addWidget(_labelFileType);
	layoutLabels->addWidget(_labelResType);

	layoutButtons->addWidget(_buttonExportRaw);
	layoutButtons->addWidget(_buttonExportBMUMP3);
	layoutButtons->addWidget(_buttonExportTGA);
	layoutButtons->addWidget(_buttonExportWAV);
	layoutButtons->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));

	layoutButtons->setSizeConstraint(QLayout::SetMinimumSize);

	layoutTop->addLayout(layoutLabels);
	layoutTop->addLayout(layoutButtons);

	layoutTop->setContentsMargins(0, 0, 0, 0);

	_buttonExportRaw->setVisible(false);
	_buttonExportBMUMP3->setVisible(false);
	_buttonExportTGA->setVisible(false);
	_buttonExportWAV->setVisible(false);

	QObject::connect(_buttonExportRaw, &QPushButton::clicked, this, &PanelResourceInfo::slotSave);
	QObject::connect(_buttonExportTGA, &QPushButton::clicked, this, &PanelResourceInfo::slotExportTGA);
	QObject::connect(_buttonExportBMUMP3, &QPushButton::clicked, this, &PanelResourceInfo::slotExportBMUMP3);
	QObject::connect(_buttonExportWAV, &QPushButton::clicked, this, &PanelResourceInfo::slotExportWAV);
}

void PanelResourceInfo::slotSave() {
	emit saveClicked();
}

void PanelResourceInfo::slotExportTGA() {
	emit exportTGAClicked();
}

void PanelResourceInfo::slotExportBMUMP3() {
	emit exportBMUMP3Clicked();
}

void PanelResourceInfo::slotExportWAV() {
	emit exportWAVClicked();
}

void PanelResourceInfo::update(const GUI::ResourceTreeItem *item) {
	setLabels(item);
	showExportButtons(item);
}

void PanelResourceInfo::showExportButtons(const GUI::ResourceTreeItem *item) {
	if (!item || item->getSource() == Source::kSourceDirectory) {
		showExportButtons(false, false, false, false);
		return;
	}

	bool isBMU   = item->getFileType()     == Aurora::kFileTypeBMU;
	bool isSound = item->getResourceType() == Aurora::kResourceSound;
	bool isImage = item->getResourceType() == Aurora::kResourceImage;

	showExportButtons(true, isBMU, isSound, isImage);
}

void PanelResourceInfo::showExportButtons(bool enableRaw, bool showMP3, bool showWAV, bool showTGA) {
	_buttonExportRaw->setVisible(enableRaw);
	_buttonExportTGA->setVisible(showTGA);
	_buttonExportBMUMP3->setVisible(showMP3);
	_buttonExportWAV->setVisible(showWAV);
}

const QString getSizeLabel(size_t size) {
	if (size == Common::kFileInvalid)
		return "-";

	if (size < 1024)
		return QString("%1").arg(size);

	QString humanRead = QString::fromUtf8(Common::FilePath::getHumanReadableSize(size).c_str());

	return QString("%1 (%2)").arg(humanRead).arg(size);
}

const QString getFileTypeLabel(Aurora::FileType type) {
	QString label = QString("%1").arg(type);

	if (type != Aurora::kFileTypeNone) {
		QString temp = QString::fromUtf8(TypeMan.getExtension(type).c_str());
		label += QString(" (%1)").arg(temp);
	}

	return label;
}

const QString getResTypeLabel(Aurora::ResourceType type) {
	QString label = QString("%1").arg(type);

	if (type != Aurora::kResourceNone) {
		QString temp = QString::fromUtf8(getResourceTypeDescription(type).c_str());
		label += QString(" (%1)").arg(temp);
	}

	return label;
}

void PanelResourceInfo::setLabels(const ResourceTreeItem *item) {
	QString labelName     = "Resource name: ";
	QString labelSize     = "Size: ";
	QString labelFileType = "File type: ";
	QString labelResType  = "Resource type: ";

	labelName += item->getName();

	if (item->getSource() == Source::kSourceDirectory) {

		labelSize     += "-";
		labelFileType += "Directory";
		labelResType  += "Directory";

	} else if ((item->getSource() == Source::kSourceFile) ||
			   (item->getSource() == Source::kSourceArchiveFile)) {

		Aurora::FileType     fileType = item->getFileType();
		Aurora::ResourceType resType  = item->getResourceType();

		labelSize     += getSizeLabel(item->getSize());
		labelFileType += getFileTypeLabel(fileType);
		labelResType  += getResTypeLabel(resType);
	}

	_labelName->setText(labelName);
	_labelSize->setText(labelSize);
	_labelFileType->setText(labelFileType);
	_labelResType->setText(labelResType);
}

void PanelResourceInfo::clearLabels() {
	_labelName->setText("Resource name:");
	_labelSize->setText("Size:");
	_labelFileType->setText("File type:");
	_labelResType->setText("Resource type:");
}

void PanelResourceInfo::setButtonsForClosedDir() {
	_buttonExportRaw->setVisible(false);
	_buttonExportBMUMP3->setVisible(false);
	_buttonExportWAV->setVisible(false);
	_buttonExportTGA->setVisible(false);
}

} // End of namespace GUI
