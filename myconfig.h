#ifndef MYCONFIG_H
#define MYCONFIG_H

#include <QString>
#include <QMutex>
#include <QSettings>

class MyConfig
{
public:
    static MyConfig *GetInstance();
    static void DestroyInstance();

private:
    static QMutex m_lock;
    static MyConfig *m_pMyConfig;

private:
    MyConfig();
    ~MyConfig();
    void LoadMyConfig();

private:
    bool m_initialized = false;
    QString m_iniFile = "";

public:
    QString videoSource = "";
};

#endif // MYCONFIG_H
