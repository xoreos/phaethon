#include "statusbar.h"

StatusBar::StatusBar(QStatusBar *statusBar)
{
    _statusBar = statusBar;
}

void StatusBar::setText(const QString &text)
{
    _statusBar->showMessage(text);
    _text = text;
}

void StatusBar::push(const QString &text, int timeout)
{
    _statusBar->showMessage(text, timeout);
}

void StatusBar::pop()
{
    _statusBar->showMessage(_text);
}
