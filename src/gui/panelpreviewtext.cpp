#include "panelpreviewtext.h"

#include "verdigris/wobjectimpl.h"

namespace GUI {

W_OBJECT_IMPL(PanelPreviewText)

PanelPreviewText::PanelPreviewText(QWidget *parent)
{
    _ui.setupUi(this);
}

void PanelPreviewText::setText(QString text)
{
    _ui.textEdit->setText(text);
}

void PanelPreviewText::setItem(const ResourceTreeItem *item) {
    if (item == _currentItem)
        return;

    if (item->getFileType() != Aurora::FileType::kFileTypeTXT &&
        item->getFileType() != Aurora::FileType::kFileTypeINI)
        return;

    if (item->getSize() > 1000000) // 1 MB
        return; // fixme: exception

    _currentItem = item;

    QFile file(item->getFileInfo().canonicalFilePath());
    file.open(QFile::ReadOnly | QFile::Text);
    QTextStream textStream(&file);

    setText(textStream.readAll());

    return; // fixme: exception
}

} // End of namespace GUI
