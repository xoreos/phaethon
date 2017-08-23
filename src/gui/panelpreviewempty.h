#ifndef PANELPREVIEWEMPTY_H
#define PANELPREVIEWEMPTY_H

#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QSpacerItem>
#include <QWidget>

#include "verdigris/wobjectdefs.h"

namespace GUI {

class PanelPreviewEmpty : public QFrame {
    W_OBJECT(PanelPreviewEmpty)

private:
    QHBoxLayout *_layoutHorizontal;
    QLabel *_label;

public:
    PanelPreviewEmpty(QWidget *parent = 0);
};

} // End of namespace GUI

#endif // PANELPREVIEWEMPTY_H
