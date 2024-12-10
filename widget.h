#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "myconfig.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

protected:
    virtual void showEvent(QShowEvent *event) override;
    virtual void closeEvent(QCloseEvent *event) override;

private:
    Ui::Widget *ui;
    MyConfig m_config;
};
#endif // WIDGET_H
