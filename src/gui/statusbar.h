#ifndef STATUSBAR_H
#define STATUSBAR_H

#include <QMainWindow>
#include <QStatusBar>
#include <QWidget>

namespace GUI {

/** Class to have identical functionality to Phaethon.
 *  Or wx.                                              **/

class StatusBar
{
public:
    StatusBar(QStatusBar *statusBar);
    void setText(const QString &text);
    void push(const QString &text, int timeout = 0);
    void pop();

private:
    QStatusBar *_statusBar;
    QString _text;
};

} // End of namespace GUI

#endif // STATUSBAR_H
