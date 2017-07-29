#ifndef PANELRESOURCEINFO_H
#define PANELRESOURCEINFO_H

#include <QWidget>

#include "verdigris/wobjectdefs.h"

#include "ui/ui_resourceinfo.h"

namespace GUI {

class ResourceTreeItem;

class PanelResourceInfo : public QFrame
{
    W_OBJECT(PanelResourceInfo)

public:
    PanelResourceInfo(QWidget *parent = 0);

    Ui::PanelResourceInfo &getUi();
    void showExportButtons(const GUI::ResourceTreeItem *item);
    void showExportButtons(bool enableRaw, bool showMP3, bool showWAV, bool showTGA);
    void setLabels(const GUI::ResourceTreeItem *item);
    void update(const GUI::ResourceTreeItem *item);
    void clearLabels();
    void setButtonsForClosedDir();

public /*signals*/:
    void loadModel(const QString &path)
    W_SIGNAL(loadModel, path)

    void logAppend(const QString &text)
    W_SIGNAL(logAppend, text)

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

public /*slots*/:
    void slotSave();
    void slotExportTGA();
    void slotExportBMUMP3();
    void slotExportWAV();

private /*slots*/:
    void slotLoadKotorDir();
    void slotCloseDir();

private:
    Ui::PanelResourceInfo _ui;

    QPushButton *_btnExportRaw;
    QPushButton *_btnExportBMUMP3;
    QPushButton *_btnExportTGA;
    QPushButton *_btnExportWAV;
};

} // End of namespace GUI

#endif // PANELRESOURCEINFO_H
