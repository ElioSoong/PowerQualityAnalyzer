/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**-----------------------------------------File Info------------------------------------------------------
** File name:                  ScpiRemoteAdaptor.h
** Latest Version:             
** Latest modified Date:       2017/03/10
** Modified by:                
** Descriptions:               
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 Song Yunhua
** Created date:               2017/03/10
** Descriptions:               
** 
*********************************************************************************************************/

#ifndef SCPIREMOTEADAPTOR_H
#define SCPIREMOTEADAPTOR_H

#include <QObject>
#include <QtDBus/QDBusVariant>

class F280Application;
class FieldModel;
class PQA;
class Storage;

class ScpiRemoteAdaptor : public QObject
{
    Q_OBJECT

public:
    ScpiRemoteAdaptor();

    static ScpiRemoteAdaptor* init();

public slots:
    /*! 
     * 字段树读写操作
     */
    QString readField(const QString &path, const QString &role,
        int row = -1, int col = -1);

    bool writeField(const QString &path, const QString &role,
        QDBusVariant data, int row = -1, int col = -1);

    /*! 
     * 测量控制
     */
    bool requestAnalyzerStart(const QString &activityName, const QString& time);
    bool requestAnalyzerStop();
    bool requestAnalyzerRestart(const QString& time);
    bool changeSyncSrcMode(int mode, int channel = -1);

    /*! 
     * 存储相关操作
     */
    QString storageFileList();
    int     storageItemCount();
    bool    saveStorageInfo(int index);
    bool    removeStorageInfo(int index);
    bool    loadStorageInfo(int index);
    bool    renameStorageFile(int index, const QString &name);
    /*!
     * 获取指定存储项的信息
     * @param[in]
     * @return    类型，时间，长度等，尤其是根文件夹路径
     */
    QString storageItemInfo(int index);

    /*! 
     * 输入事件模拟
     */
    void emulateMouse(int type, int x, int y, int button);
    void emulateKey(int type, int key, int modifier);
    void emulateTouch();
    void emulateGesture();

    /*! 
     * 系统相关操作
     */
    QDBusVariant requestSystemSnapshot();
    bool    requestSystemReboot();
    bool    requestSystemShutdown();
    bool    requestSystemRootLogin(const QString& code);
    bool    requestSystemRootLogout();
    QString requestFirmwareVersion();
    bool    requestFirmwareUpgrade();
    QString requestSystemTime();
    bool    requestSetSystemTime(const QString& time);
    QString requestDeviceID();
    bool    requestGPSStatus();

private:
    static ScpiRemoteAdaptor*   s_instance;
    PQA*                    m_Analyzer;                  //!< 指向PQA的静态对象，方便对测量的控制
    Storage*                m_IStorage;             //!< 指向IStorage的静态对象，对存储功能的操作
    F280Application*        m_Application;       //!< 指向PqaApplication的静态对象，方便对系统的控制
    QString                 m_fileInfo;
};

#endif // SCPIREMOTEADAPTOR_H
/*********************************************************************************************************
** End of file
*********************************************************************************************************/
