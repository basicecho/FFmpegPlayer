#include "myconfig.h"
#include <QSettings>

QMutex MyConfig::m_lock;
MyConfig *MyConfig::m_pMyConfig = nullptr;

MyConfig *MyConfig::GetInstance()
{
    m_lock.lock();

    if(!m_pMyConfig)
    {
        m_pMyConfig = new MyConfig();
    }

    m_lock.unlock();

    return m_pMyConfig;
}
void MyConfig::DestroyInstance()
{
    m_lock.lock();

    if(m_pMyConfig)
    {
        delete m_pMyConfig;
        m_pMyConfig = nullptr;
    }

    m_lock.unlock();
}

MyConfig::MyConfig()
{

}

MyConfig::~MyConfig()
{

}

void MyConfig::LoadMyConfig()
{
    m_lock.lock();

    if(m_pMyConfig && !m_initialized)
    {
        QSettings iniReader(m_iniFile, QSettings::IniFormat);

        videoSource = iniReader.value("video/videoSource", "test.mp4").toString();

        m_initialized = true;
    }

    m_lock.unlock();
}
