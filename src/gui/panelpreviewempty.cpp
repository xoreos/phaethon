#include "verdigris/wobjectimpl.h"

#include "src/common/system.h"
#include "src/gui/panelpreviewempty.h"

namespace GUI {

W_OBJECT_IMPL(PanelPreviewEmpty)

PanelPreviewEmpty::PanelPreviewEmpty(QWidget *parent) 
    : QFrame(parent) {
    _layoutHorizontal = new QHBoxLayout(this);
    _layoutHorizontal->setContentsMargins(0, 0, 0, 0);

    _label = new QLabel(tr("[Nothing to preview.]"), this);
    _label->setObjectName(QStringLiteral("_label"));
    _label->setEnabled(false);
    _label->setAlignment(Qt::AlignCenter);

    _layoutHorizontal->addWidget(_label);

    this->setLayout(_layoutHorizontal);
}

} // End of namespace GUI
