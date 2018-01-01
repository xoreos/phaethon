#ifndef PANELPREVIEWTEXT_H
#define PANELPREVIEWTEXT_H

#include <QFrame>
#include <QHBoxLayout>
#include <QPlainTextEdit>
#include <QTextStream>
#include <QWidget>

#include "verdigris/wobjectdefs.h"

#include "ui/ui_previewtext.h"

namespace GUI {

class ResourceTreeItem;

class PanelPreviewText : public QFrame
{
    W_OBJECT(PanelPreviewText)

private:
    const ResourceTreeItem *_currentItem;
    Ui::PreviewText _ui;

public:
    PanelPreviewText(QWidget *parent = 0);

    void setText(const QString &text);
    void setItem(const ResourceTreeItem *item);
};

} // End of namespace GUI

#endif // PANELPREVIEWTEXT_H
