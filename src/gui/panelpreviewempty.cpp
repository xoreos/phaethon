#include "verdigris/wobjectimpl.h"

#include "src/common/system.h"
#include "src/gui/panelpreviewempty.h"

namespace GUI {

W_OBJECT_IMPL(PanelPreviewEmpty)

PanelPreviewEmpty::PanelPreviewEmpty(QWidget *UNUSED(parent)) {
		this->setObjectName(QStringLiteral("PanelPreviewEmpty"));
        this->resize(400, 300);
        _boxLayoutHorizontal = new QHBoxLayout(this);
        _boxLayoutHorizontal->setObjectName(QStringLiteral("_boxLayoutHorizontal"));
        _boxLayoutHorizontal->setContentsMargins(0, 0, 0, 0);
        _label = new QLabel(this);
        _label->setObjectName(QStringLiteral("_label"));
        _label->setEnabled(false);
        _label->setAlignment(Qt::AlignCenter);

        _boxLayoutHorizontal->addWidget(_label);

        _label->setText(tr("[Nothing to preview.]"));
}

} // End of namespace GUI
