#ifndef PANELPREVIEWTEXT_H
#define PANELPREVIEWTEXT_H

#include "verdigris/wobjectdefs.h"

#include "ui/ui_previewtext.h"
#include "src/gui/resourcetreeitem.h"

#include <QFrame>
#include <QHBoxLayout>
#include <QPlainTextEdit>
#include <QTextStream>
#include <QWidget>

namespace GUI {

class PanelPreviewText : public QFrame
{
    W_OBJECT(PanelPreviewText)

private:
    const ResourceTreeItem *_currentItem;
    Ui::PreviewText _ui;

public:
    PanelPreviewText(QWidget *parent = 0);

    void setText(QString text);
    void setItem(const ResourceTreeItem *node);
};

} // End of namespace GUI

#endif // PANELPREVIEWTEXT_H
