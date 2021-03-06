﻿/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  Application.h
** Latest Version:             V1.0.0
** Latest modified Date:       2016/3/1
** Modified by:                
** Descriptions:               
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 
** Created date:               2016/2/29
** Descriptions:               Application class  [Generated by VP-UML, IGT1.0]
** 
*********************************************************************************************************/
#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include "corelibrary_global.h"
#include <QApplication>
#include <QtWidgets/QSplashScreen>

#include "JsonLoader/JsonLoader.h"
#include "FieldLibrary/IField.h"
#include "FieldLibrary/FieldWidgetCreator.h"
#include "MetaSystemHelper.h"
#include "Plugin.h"
#include "gui/MainWindow.h"
#include "HardwareManager.h"


/*!
 * @class Application
 * @brief 应用程序对象，单例模式，可通过该对象访问其他各个单例模式的对象，例如mainWindow
 */
class CORELIBRARY_EXPORT Application : public QApplication
{
    Q_OBJECT
    Q_PROPERTY(MainWindow* mainWindow READ mainWindow CONSTANT)
    Q_PROPERTY(QList<PluginList*> plugins READ plugins WRITE setPlugins)

public: 
    /*!
     * 构造函数
     * @param[in]    argc	
     * @param[in]    argv	
     */
    Application(int& argc, char** argv);

    /*! 
     * Getter for mainWindow
     * @return     mainWindow
     */
    MainWindow* mainWindow() const
    {
        return m_mainWindow;
    }

    /*!
     * Getter/Setter for metaSystemHelper
     */
    MetaSystemHelper* metaSystemHelper() const
    {
        return m_metaSystemHelper;
    }
    void setMetaSystemHelper(MetaSystemHelper* metaSystemHelper)
    {
        m_metaSystemHelper = metaSystemHelper;
    }

    virtual MetaSystemHelper* createMetaSystemHelper();

    virtual FieldWidgetCreator* createFieldWidgetCreator();

    /*! 
     * Getter for jsonLoader
     * @return     jsonLoader
     */
    JsonLoader& jsonLoader()
    {
        return m_jsonLoader;
    }

    /*! 
     * 从JsonLoader加载的对象中，根据名称查找指定的对象
     * @param[in]  objectName 对象名称
     * @return     查找到的对象，可通过qobject_cast转换为指定子类类型
     */
    QObject* findJsonObject(const QString& objectName)
    {
        return m_jsonLoader.findObject(objectName);
    }

    /*! 
     * 将指定的QObject对象添加至JsonLoader，使之可以在JSON中通过对象名进行访问
     * @param[in]  object 需要添加的对象
     * @return     操作成功返回true
     */
    bool addJsonObject(QObject* object)
    {
        return m_jsonLoader.addGlobalObject(object);
    }

    /*!
     * Getter/Setter for plugins
     */
    QList<PluginList*> plugins() const
    {
        return m_plugins;
    }
    void setPlugins(const QList<PluginList*>& plugins);

    /** 
     * 获取指定类型名对应的全部已加载插件对象
     * @param[in]  name         类型名（例如"devices"）
     * @return     QObjectList  插件对象列表，注意返回的是插件中实际包含的实例对象，而不是插件对象本身
     */
    Q_SLOT QObjectList pluginInstances(const QString& name) const;

    /** 
     * 获取指定对象名获取本Application的一个单例的module
     * @param[in]  moduleInstanceName 模块实例对象名
     * @return     QObject*
     */
    Q_SLOT QObject* module(const QString& moduleInstanceName) const;

    /** 
     * MainWindow的工厂，在子类中创建自身需要的MainWindow类型，支持Widget与QML
     * @return     MainWindow*
     */
    virtual MainWindow* createMainWindow() = 0;

    /** 
     * 设置一个外部创建好的MainWindow对象
     * @param[in]  mainWindow
     * @return     void
     */
    void setMainWindow(MainWindow* mainWindow);

    /*! 
     * 在欢迎界面上显示文本信息，仅在初始化时使用
     * @param[in]  message 需要显示的文本信息
     */
    void showSplashMessage(const QString& message);

    /*! 
     * 关闭欢迎界面，显示主窗口
     * @return 操作成功返回true
     */
    bool showMainWindow();

    /*!
     * 初始化整个系统，创建各个单例模式对象
     * 注意在此函数中仍可通过this->arguments()获取main函数传入的argv，
     * 并可通过此方式将初始化代码移动至此函数，
     * 从而保持main函数的整洁。
     * @return       操作成功返回true
     */
    virtual bool init();

    /*!
     * 销毁整个系统
     */
    virtual void destroy();

    /*!
     * 判断当前磁盘空间是否已满
     */
    static bool availableDiskSpace(const QString & diskPath);

public slots:
    /*!
     * 获取软件版本号
     * @return  返回系统软件版本号，硬件版本号，出厂日期，校准日期等
     */
    QString version();

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
     * QT的消息handler，可以用于格式化打印信息等特殊操作
     */
    static void qtMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);

    /*!
     * 系统信号handler，可以用于捕获段错误等
     * @param[in]    signo	 Linux系统信号id
     */
    static void systemSignalHandler(int signo);

    /*! 
     * 获取jsonLoader的错误提示信息
     * @param[in]  code     错误码
     * @param[in]  message  错误信息
     * @return     void
     */
    void onJsonLoaderError(int code, const QString& message);
    /*! 
     * 设置设备背光亮度
     * @param[in] brightness 亮度百分比
     * @return  设置成功返回 true，否则返回false
     */
    bool setBrightness(int brightness);
    /*! 
     * 蜂鸣器设置函数
     */
    virtual bool setBuzzer();

protected:
    static void dump(int signo);
    static int visualStudioPrintf(const char* fmt, ...);

    void setModuleProperties();
    void loadPlugins();
    void resetModuleProperties();


signals:
    void error(const QString& message);

private: 
    JsonLoader          m_jsonLoader;       //!< JSON脚本加载器
    MetaSystemHelper*   m_metaSystemHelper; //!< 用于扩展注册元对象系统的帮助类
    QList<PluginList*>  m_plugins;          //!< 所有的插件类型以及每种类型对应的插件对象

    QSplashScreen       m_splash;           //!< 欢迎界面
    MainWindow* 		m_mainWindow;       //!< 全局唯一的主窗口对象
    HardwareManager     m_hardwareManager;  //!< 硬件相关操作
};

#define theApp ((Application*)QCoreApplication::instance())

#endif
/*********************************************************************************************************
** End of file
*********************************************************************************************************/
