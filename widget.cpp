#include "widget.h"
#include "ui_widget.h"
#include <iostream>
#include <QShowEvent>
#include <QCloseEvent>
#include <QMessageBox>
#include <QDir>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::showEvent(QShowEvent *event)
{
    // 加载配置文件
    m_config.InitMyConfig();
    std::cout << m_config.m_videoSource.toStdString() << std::endl;

    QString cur = QDir::currentPath();
    std::cout << QDir::currentPath().toStdString() << std::endl;

    QWidget::showEvent(event);
}

void Widget::closeEvent(QCloseEvent *event)
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "你是否确定要关闭程序？", "确定关闭",
                                  QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if(reply == QMessageBox::No)
    {
        event->ignore();
        return;
    }

    event->accept();
}

