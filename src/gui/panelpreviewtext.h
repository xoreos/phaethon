#ifndef PANELPREVIEWTEXT_H
#define PANELPREVIEWTEXT_H

#include <QFrame>
#include <QHBoxLayout>
#include <QPlainTextEdit>
#include <QTextStream>
#include <QWidget>

#include "verdigris/wobjectdefs.h"

namespace GUI {

class ResourceTreeItem;

class PanelPreviewText : public QFrame {
    W_OBJECT(PanelPreviewText)

private:
	QHBoxLayout *_layoutHorizontal;
	QTextEdit *_textEdit;

    const ResourceTreeItem *_currentItem;

public:
    PanelPreviewText(QWidget *parent = 0);

    void setText(const QString &text);
    void setItem(const ResourceTreeItem *item);

    void log(const QString &text)
    W_SIGNAL(log, text)
};

} // End of namespace GUI

#endif // PANELPREVIEWTEXT_H
