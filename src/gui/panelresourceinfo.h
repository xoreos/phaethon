#ifndef PANELRESOURCEINFO_H
#define PANELRESOURCEINFO_H

#include <QFrame>
#include <QLabel>
#include <QLayout>
#include <QWidget>

#include "verdigris/wobjectdefs.h"

namespace GUI {

class ResourceTreeItem;

class PanelResourceInfo : public QFrame {
    W_OBJECT(PanelResourceInfo)

private:
    QVBoxLayout *_layoutVertical;
    QHBoxLayout *_layoutHorizontal;
    QVBoxLayout *_layoutVertical_2;

    QPushButton *_buttonExportRaw;
    QPushButton *_buttonExportBMUMP3;
    QPushButton *_buttonExportTGA;
    QPushButton *_buttonExportWAV;

    QLabel *_labelName;
    QLabel *_labelSize;
    QLabel *_labelFileType;
    QLabel *_labelResType;

public:
    PanelResourceInfo(QWidget *parent = 0);

    void showExportButtons(const GUI::ResourceTreeItem *item);
    void showExportButtons(bool enableRaw, bool showMP3, bool showWAV, bool showTGA);
    void setLabels(const GUI::ResourceTreeItem *item);
    void update(const GUI::ResourceTreeItem *item);
    void clearLabels();
    void setButtonsForClosedDir();

public /*signals*/:
    void loadModel(const QString &path)
    W_SIGNAL(loadModel, path)

    void log(const QString &text)
    W_SIGNAL(log, text)

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
};

} // End of namespace GUI

#endif // PANELRESOURCEINFO_H
