#ifndef PANELPREVIEWEMPTY_H
#define PANELPREVIEWEMPTY_H

#include <QFrame>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QWidget>

#include "verdigris/wobjectdefs.h"

#include "ui/ui_previewempty.h"

namespace GUI {

class PanelPreviewEmpty : public QFrame
{
    W_OBJECT(PanelPreviewEmpty)

private:
    Ui::PreviewEmpty _ui;

public:
    PanelPreviewEmpty(QWidget *parent = 0);
};

} // End of namespace GUI

#endif // PANELPREVIEWEMPTY_H
