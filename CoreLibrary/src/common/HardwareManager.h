/****************************************Copyright (c)****************************************************
**
**                             INNO Instrument(China) Co.Ltd.
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  HardwareManager.h
** Latest Version:             
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
#ifndef __HARDWAREMANAGER_H__
#define __HARDWAREMANAGER_H__

#include "corelibrary_global.h"
#include <QApplication>

/*!
 * @class HardwareManager
 * @brief 管理硬件相关的操作
 */
class CORELIBRARY_EXPORT HardwareManager : public QObject
 {
	 Q_OBJECT
public: 
    /*!
     * 构造函数    
     */
     HardwareManager();

	 
public slots:
    /*!
     * 重启系统
     */
    bool reboot();

    /*!
     * 关闭系统
     */
    bool shutdown();

    /*! 
     * root权限登录
     * @param[in]  code 登录密码
     * @return     bool 是否登录成功
     */
    bool rootLogin(const QString &code);

    /*! 
     * root权限登出
     * @return     bool 是否登出成功
     */
    bool rootLogout();

    /*!
    * 设置设备背光亮度
    * @param[in] brightness 亮度百分比
    * @return  设置成功返回 true，否则返回false
    */
    bool setBrightness(int brightness);

    /*!
    * 蜂鸣器设置函数
    */
    bool setBuzzer();
 };
#endif
/*********************************************************************************************************
** End of file
*********************************************************************************************************/