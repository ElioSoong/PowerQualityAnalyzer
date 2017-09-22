/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  HardwareManager.cpp
** Latest Version:             V1.0.0
** Latest modified Date:       
** Modified by:                
** Descriptions:               
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 
** Created date:               2017/6/6
** Descriptions:               HardwareManager class  
** 
*********************************************************************************************************/
#include "includes.h"
//#include "config.h"
//#include "version.h"
#include "common/HardwareManager.h"

#include <QMessageBox>
#include <QByteArray>
#include <stdlib.h> 

#ifdef _WIN32
#include <Windows.h>
#else
#include <sys/statfs.h> 
#include <sys/sysinfo.h>
#include <signal.h>
#include <execinfo.h>
#endif


/*!
 * 构造函数   
 */
HardwareManager::HardwareManager() : QObject()
{

}

/*!
 * 重启系统
 */
bool HardwareManager::reboot()
{
#ifndef _WIN32
    int ret = system("reboot");
    if(-1 != ret)
        return true;
    return false;
#else
    return false;
#endif
}

/*!
 * 关闭系统
 */
bool HardwareManager::shutdown()
{
#ifndef _WIN32
    int ret = system("shutdown -h now");
    if(-1 != ret)
        return true;
    return false;

#else
    return false;
#endif
}

/*! 
 * root权限登录
 * @param[in]  code 登录密码
 * @return     bool 是否登录成功
 */
bool HardwareManager::rootLogin( const QString &code )
{
    return true;
}

/*! 
 * root权限登出
 * @return     bool 是否登出成功
 */
bool HardwareManager::rootLogout()
{
    return true;
}

/*!
* 设置设备背光亮度
* @param[in] brightness 亮度百分比
* @return 设置成功返回 true，否则返回false
*/
bool HardwareManager::setBrightness(int brightness)
{
    bool ok = false;
    QString str = QString("echo %1 > /sys/class/backlight/zynq-pl,pwm-backlight/brightness").arg(brightness);
    qDebug() << "cmdString:" << str;
    QByteArray byteArray = str.toLatin1();
    const char *command = byteArray.data();
    qDebug() << "command:" << command;
#ifndef _WIN32
    int ret = system(command);
    qDebug() << "setBrightnessRet:" << ret;
    if (-1 != ret)
    {
        ok = true;
    }
#endif    
    qDebug() << "setBrightness:" << ok;
    return ok;
}

/*!
* 蜂鸣器设置函数
*/
bool HardwareManager::setBuzzer()
{
    //     bool ok = false;
    //     QUERY_SETUP_FIELD_BOOL("/sys/setup/userPreferenceSet/buzzer", enabled);
    //     if (!enabled) 
    //     {
    //         return ok;
    //     }
    // #ifndef _WIN32
    //     int ret = system("echo 1 > /sys/devices/amba.0/pl-buzzer-dev.11/enable_buzzer/enable_buzzer");
    //     if (ret != -1)
    //     {
    //         ok = true;
    //     }
    // #endif
    /*    return ok;*/
    return false;
}

/*********************************************************************************************************
** End of file
*********************************************************************************************************/