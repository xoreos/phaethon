#ifndef RESOURCEINFOPANEL_H
#define RESOURCEINFOPANEL_H

#include <QWidget>
#include "src/gui/resourcetreeitem.h"
#include "verdigris/wobjectdefs.h"
#include "ui/ui_resourceinfopanel.h"

class ResourceInfoPanel : public QFrame
{
    W_OBJECT(ResourceInfoPanel)

public:
    ResourceInfoPanel(QWidget *parent = 0);

    Ui::ResourceInfoPanel &getUi();
    void showExportButtons(ResourceTreeItem *item);
    void showExportButtons(bool enableRaw, bool showMP3, bool showWAV, bool showTGA);
    void setLabels(ResourceTreeItem *item);
    void update(ResourceTreeItem *item);
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

public /*slots*/:
    void slotSave();
    W_SLOT(slotSave, W_Access::Public)

    void slotExportTGA();
    W_SLOT(slotExportTGA, W_Access::Public)

private /*slots*/:
    void slotLoadKotorDir();
    W_SLOT(slotLoadKotorDir, W_Access::Private)

    void slotCloseDir();
    W_SLOT(slotCloseDir, W_Access::Private)

private:
    Ui::ResourceInfoPanel _ui;
};

#endif // RESOURCEINFOPANEL_H
