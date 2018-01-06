#ifndef PANELPREVIEWEMPTY_H
#define PANELPREVIEWEMPTY_H

#include "ui/ui_previewempty.h"

#include "verdigris/wobjectdefs.h"

#include <QFrame>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QWidget>

class PanelPreviewEmpty : public QFrame
{
    W_OBJECT(PanelPreviewEmpty)

private:
    Ui::PreviewEmpty _ui;

public:
    PanelPreviewEmpty(QWidget *parent = 0);
};

#endif // PANELPREVIEWEMPTY_H
