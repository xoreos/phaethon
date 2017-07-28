#include "src/gui/panelresourceinfo.h"

#include <QDir>
#include <QFileDialog>
#include "src/common/filepath.h"
#include "src/common/writefile.h"
#include "verdigris/wobjectimpl.h"

W_OBJECT_IMPL(ResourceInfoPanel)

ResourceInfoPanel::ResourceInfoPanel(QWidget *parent)
    : QFrame(parent)
    , _btnExportRaw(new QPushButton("Save", this))
    , _btnExportBMUMP3(new QPushButton("Export as MP3", this))
    , _btnExportTGA(new QPushButton("Export as TGA", this))
    , _btnExportWAV(new QPushButton("Export as WAV",this))
{
    _ui.setupUi(this);

    _ui.resLabelName->setText("Resource name:");
    _ui.resLabelSize->setText("Size:");
    _ui.resLabelFileType->setText("File type:");
    _ui.resLabelResType->setText("Resource type:");

    _ui.horizontalLayout->addWidget(_btnExportRaw);
    _ui.horizontalLayout->addWidget(_btnExportBMUMP3);
    _ui.horizontalLayout->addWidget(_btnExportTGA);
    _ui.horizontalLayout->addWidget(_btnExportWAV);

    _btnExportRaw->setVisible(false);
    _btnExportBMUMP3->setVisible(false);
    _btnExportTGA->setVisible(false);
    _btnExportWAV->setVisible(false);

    QObject::connect(_btnExportRaw, &QPushButton::clicked, this, &ResourceInfoPanel::slotSave);
    QObject::connect(_btnExportTGA, &QPushButton::clicked, this, &ResourceInfoPanel::slotExportTGA);
    QObject::connect(_btnExportBMUMP3, &QPushButton::clicked, this, &ResourceInfoPanel::slotExportBMUMP3);
    QObject::connect(_btnExportWAV, &QPushButton::clicked, this, &ResourceInfoPanel::slotExportWAV);

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

void ResourceInfoPanel::slotExportBMUMP3() {
    emit exportBMUMP3Clicked();
}

void ResourceInfoPanel::slotExportWAV() {
    emit exportWAVClicked();
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
    _btnExportRaw->setVisible(enableRaw);
    _btnExportTGA->setVisible(showTGA);
    _btnExportBMUMP3->setVisible(showMP3);
    _btnExportWAV->setVisible(showWAV);
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
    _btnExportRaw->setVisible(false);
    _btnExportBMUMP3->setVisible(false);
    _btnExportWAV->setVisible(false);
    _btnExportTGA->setVisible(false);
}
