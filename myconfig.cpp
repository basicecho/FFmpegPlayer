#include "myconfig.h"

MyConfig::MyConfig()
{

}

MyConfig::~MyConfig()
{

}

void MyConfig::InitMyConfig()
{
    QSettings settings("../config/config.ini", QSettings::IniFormat);

    m_videoSource = settings.value("VideoSource/videoFilename", "test.mp4").toString();
}
