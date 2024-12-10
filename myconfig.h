#ifndef MYCONFIG_H
#define MYCONFIG_H

#include <QSettings>

class MyConfig
{
public:
    MyConfig();
    ~MyConfig();
    void InitMyConfig();

public:
    QString m_videoSource;
};

#endif // MYCONFIG_H
