#ifndef PANELPREVIEWEMPTY_H
#define PANELPREVIEWEMPTY_H

#include "ui/ui_previewempty.h"

#include "verdigris/wobjectdefs.h"

#include <QFrame>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QWidget>

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
