#ifndef SELFCENTERWINDOW_H
#define SELFCENTERWINDOW_H

#include <QWidget>

namespace Ui {
class SelfCenterWindow;
}

class SelfCenterWindow : public QWidget
{
    Q_OBJECT

public:
    explicit SelfCenterWindow(QWidget *parent = nullptr);
    ~SelfCenterWindow();

private:
    Ui::SelfCenterWindow *ui;
};

#endif // SELFCENTERWINDOW_H
