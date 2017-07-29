#include "verdigris/wobjectimpl.h"

#include "src/common/system.h"
#include "src/gui/panelpreviewempty.h"

namespace GUI {

W_OBJECT_IMPL(PanelPreviewEmpty)

PanelPreviewEmpty::PanelPreviewEmpty(QWidget *UNUSED(parent))
{
    _ui.setupUi(this);
}

} // End of namespace GUI
