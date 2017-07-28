#include "src/gui/panelresourceinfo.h"

#include <QDir>
#include <QFileDialog>
#include "src/common/filepath.h"
#include "src/common/writefile.h"
#include "verdigris/wobjectimpl.h"

W_OBJECT_IMPL(ResourceInfoPanel)

ResourceInfoPanel::ResourceInfoPanel(QWidget *parent)
    : QFrame(parent)
{
    _ui.setupUi(this);

    _ui.resLabelName->setText("Resource name:");
    _ui.resLabelSize->setText("Size:");
    _ui.resLabelFileType->setText("File type:");
    _ui.resLabelResType->setText("Resource type:");

    _ui.bSave->setEnabled(false);
    _ui.bExportTGA->setEnabled(false);

    QObject::connect(_ui.bSave, &QPushButton::clicked, this, &ResourceInfoPanel::slotSave);
    QObject::connect(_ui.bExportTGA, &QPushButton::clicked, this, &ResourceInfoPanel::slotExportTGA);

    QObject::connect(_ui.bLoadKotorDir, &QPushButton::clicked, this, &ResourceInfoPanel::slotLoadKotorDir);
    QObject::connect(_ui.bCloseDir, &QPushButton::clicked, this, &ResourceInfoPanel::slotCloseDir);
}

Ui::ResourceInfoPanel &ResourceInfoPanel::getUi() {
    return _ui;
}

/** SLOTS **/
void ResourceInfoPanel::slotLoadKotorDir() {
    QString myKotorPath("/home/mike/kotor");
    QDir dir(myKotorPath);
    if (dir.exists())
        emit loadModel(myKotorPath);
    else
        emit logAppend("Failed: /home/mike/kotor is doesn't exist.");
}

void ResourceInfoPanel::slotCloseDir() {
    emit closeDirClicked();
}

void ResourceInfoPanel::slotSave() {
    emit saveClicked();
}

void ResourceInfoPanel::slotExportTGA() {
    emit exportTGAClicked();
}

void ResourceInfoPanel::update(const ResourceTreeItem *item) {
    setLabels(item);
    showExportButtons(item);
}

void ResourceInfoPanel::showExportButtons(const ResourceTreeItem *item) {
    if (!item || item->getSource() == ResourceTreeItem::Source::kSourceDirectory) {
        showExportButtons(false, false, false, false);
        return;
    }

    bool isBMU   = item->getFileType()     == Aurora::kFileTypeBMU;
    bool isSound = item->getResourceType() == Aurora::kResourceSound;
    bool isImage = item->getResourceType() == Aurora::kResourceImage;

    showExportButtons(true, isBMU, isSound, isImage);
}

void ResourceInfoPanel::showExportButtons(bool enableRaw, bool showMP3, bool showWAV, bool showTGA) {
    _ui.bSave->setEnabled(enableRaw);
    _ui.bExportTGA->setEnabled(showTGA);
//    _buttonExportBMUMP3->Show(showMP3);
//    _buttonExportWAV->Show(showWAV);
}

const QString getSizeLabel(size_t size) {
    if (size == Common::kFileInvalid)
        return "-";

    if (size < 1024)
        return QString("%1").arg(size);

    QString humanRead = Common::FilePath::getHumanReadableSize(size).toQString();

    return QString("%1 (%2)").arg(humanRead).arg(size);
}

const QString getFileTypeLabel(Aurora::FileType type) {
    QString label = QString("%1").arg(type);

    if (type != Aurora::kFileTypeNone) {
        QString temp = TypeMan.getExtension(type).toQString();
        label += QString(" (%1)").arg(temp);
    }

    return label;
}

const QString getResTypeLabel(Aurora::ResourceType type) {
    QString label = QString("%1").arg(type);

    if (type != Aurora::kResourceNone) {
        QString temp = getResourceTypeDescription(type).toQString();
        label += QString(" (%1)").arg(temp);
    }

    return label;
}

void ResourceInfoPanel::setLabels(const ResourceTreeItem *item) {
    QString labelName     = "Resource name: ";
    QString labelSize     = "Size: ";
    QString labelFileType = "File type: ";
    QString labelResType  = "Resource type: ";

    labelName += item->getName();

    if (item->getSource() == ResourceTreeItem::Source::kSourceDirectory) {

        labelSize     += "-";
        labelFileType += "Directory";
        labelResType  += "Directory";

    } else if ((item->getSource() == ResourceTreeItem::Source::kSourceFile) ||
               (item->getSource() == ResourceTreeItem::Source::kSourceArchiveFile)) {

        Aurora::FileType     fileType = item->getFileType();
        Aurora::ResourceType resType  = item->getResourceType();

        labelSize     += getSizeLabel(item->getSize());
        labelFileType += getFileTypeLabel(fileType);
        labelResType  += getResTypeLabel(resType);
    }

    _ui.resLabelName->setText(labelName);
    _ui.resLabelSize->setText(labelSize);
    _ui.resLabelFileType->setText(labelFileType);
    _ui.resLabelResType->setText(labelResType);
}

void ResourceInfoPanel::clearLabels() {
    _ui.resLabelName->setText("Resource name:");
    _ui.resLabelSize->setText("Size:");
    _ui.resLabelFileType->setText("File type:");
    _ui.resLabelResType->setText("Resource type:");
}

void ResourceInfoPanel::setButtonsForClosedDir() {
    _ui.bCloseDir->setEnabled(false);
    _ui.bSave->setEnabled(false);
    _ui.bExportTGA->setEnabled(false);
}
