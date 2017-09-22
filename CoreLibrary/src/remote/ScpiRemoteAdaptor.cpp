/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**-----------------------------------------File Info------------------------------------------------------
** File name:                  ScpiRemoteAdaptor.cpp
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
#include <QDebug>
#include <QtDBus/QDBusConnection>
#include <QDateTime>

//#include "storage/F280StorageItem.h"
#include "ScpiRemoteAdaptor.h"
#include "Application.h"
#include "FieldModel.h"
//#include "PQA.h"
#include "storage/IStorage.h"
#include "remote_adaptor.h"

ScpiRemoteAdaptor* ScpiRemoteAdaptor::s_instance = NULL;

ScpiRemoteAdaptor::ScpiRemoteAdaptor()
{
    // 指向IStorage的静态对象，对存储功能的操作
    m_IStorage = Storage::instance();

    // 指向Application的静态对象，方便对系统的控制
    m_Application = F280Application::instance();

    // 指向Analyzer的静态对象，方便对测量的控制
    m_Analyzer = PQA::instance();
}

ScpiRemoteAdaptor* ScpiRemoteAdaptor::init()
{
    if (s_instance == NULL)
    {
        s_instance = new ScpiRemoteAdaptor();
    }

    //注册DBus服务
    QDBusConnection bus = QDBusConnection::sessionBus();
    new RemoteAdaptor(s_instance);
    if (!bus.registerService("com.inno.Analyzer"))
    {
        qDebug() << bus.lastError().message();
        exit(1);
    }

    bus.registerObject("/RemoteAdaptor", s_instance);

    return s_instance;
}

/*! 
 * 读写字段树
 */
QString ScpiRemoteAdaptor::readField(
                                const QString &path, 
                                const QString &role, 
                                int row /*= -1*/, 
                                int col /*= -1*/)
{
    int roleNum = FieldModel::parseTheRole(role);
    IField *field = IField::at(path);
    return field->data(row, col, roleNum).toString();
}

bool ScpiRemoteAdaptor::writeField(
                                const QString &path, 
                                const QString &role, 
                                QDBusVariant data, 
                                int row /*= -1*/, 
                                int col /*= -1*/)
{
    int roleNum = FieldModel::parseTheRole(role);
    IField *field = IField::at(path);
    return field->setData(row, col, data.variant(), roleNum);
}

/*! 
 * 测量控制
 */
bool ScpiRemoteAdaptor::requestAnalyzerStart(const QString &activityName, const QString& time)
{
    if (activityName.isEmpty())
    {
        return false;
    }
    return m_Analyzer->startActivity(activityName, time);
}

bool ScpiRemoteAdaptor::requestAnalyzerStop()
{
    return m_Analyzer->stopActivity();
}

bool ScpiRemoteAdaptor::requestAnalyzerRestart(const QString& time)
{
    return m_Analyzer->restartActivity(time);
}

bool ScpiRemoteAdaptor::changeSyncSrcMode(int mode, int channel /*= -1*/)
{
    return m_Analyzer->setSyncSrcMode(mode, channel);
}

/*! 
 * 存储相关操作
 */
QString ScpiRemoteAdaptor::storageFileList()
{
    m_fileInfo.clear();
    Storage *storage = Storage::instance();
    if (storage->enumerate(STORAGE_ROOT_PATH) <= 0)
    {
        qDebug() << "No Storage Items";
        return m_fileInfo;
    }

    for (int i = 0; i < storage->itemCount(); ++i)
    {
        StorageItem *item = storage->itemAt(i);
        if (item->type() & F280StorageItemFactory::SessionType)
        {
            m_fileInfo.append("Type:SessionType,");
        }
        else if (item->type() & F280StorageItemFactory::SnapshotType)
        {
            m_fileInfo.append("Type:SnapshotType,");
        }
        else if (item->type() & F280StorageItemFactory::MonitorType)
        {
            m_fileInfo.append("Type:MonitorType,");
        }
        else if (item->type() & F280StorageItemFactory::LimitType)
        {
            m_fileInfo.append("Type:LimitType,");
        }
        else if (item->type() & F280StorageItemFactory::SetupType)
        {
            m_fileInfo.append("Type:SetupType,");
        }
        else if (item->type() & F280StorageItemFactory::TrendType)
        {
            m_fileInfo.append("Type:TrendType,");
        }
        else if (item->type() & F280StorageItemFactory::EventType)
        {
            m_fileInfo.append("Type:EventType,");
        }
        else if (item->type() & F280StorageItemFactory::WaveType)
        {
            m_fileInfo.append("Type:WaveType,");
        }
        else if (item->type() & F280StorageItemFactory::HcRmsType)
        {
            m_fileInfo.append("Type:HcRmsType,");
        }
        else if (item->type() & F280StorageItemFactory::TaskType)
        {
            m_fileInfo.append("Type:TaskType,");
        }
        else if (item->type() & F280StorageItemFactory::BarType)
        {
            m_fileInfo.append("Type:BarType,");
        }

        m_fileInfo.append(QString("Info:%1,").arg(item->description()));
        m_fileInfo.append(QString("CreateTime:%1;").arg(
                              QDateTime::fromMSecsSinceEpoch(item->createTime()).toString("yyyy-MM-dd hh:mm:ss")
                              ));
    }
    return m_fileInfo;
}

int ScpiRemoteAdaptor::storageItemCount()
{
    return m_IStorage->itemCount();
}

bool ScpiRemoteAdaptor::saveStorageInfo(int index)
{
    return m_IStorage->save(index);
}

bool ScpiRemoteAdaptor::removeStorageInfo(int index)
{
    return m_IStorage->remove(index);
}

bool ScpiRemoteAdaptor::loadStorageInfo(int index)
{
    return m_IStorage->load(index);
}

bool ScpiRemoteAdaptor::renameStorageFile(int index, const QString &name)
{
    return m_IStorage->rename(index, name);
}

QString ScpiRemoteAdaptor::storageItemInfo(int index)
{
    return m_IStorage->itemInfo(index);
}

/*! 
 * 输入事件模拟
 */
void ScpiRemoteAdaptor::emulateMouse(int type, int x, int y, int button)
{
    QEvent::Type eventType = (QEvent::Type)type;

    if (!(QEvent::MouseButtonPress == eventType || QEvent::MouseButtonRelease == eventType ||
        QEvent::MouseButtonDblClick == eventType || QEvent::MouseMove == eventType))
        return;

    QMouseEvent *mouseEvent = new QMouseEvent(eventType, QPointF(x, y),
        (Qt::MouseButton)button, Qt::NoButton, Qt::NoModifier);

    if (0 != mouseEvent)
    {
        QCoreApplication::postEvent(m_Application, mouseEvent);
    }
}

void ScpiRemoteAdaptor::emulateKey(int type, int key, int modifier)
{
    QEvent::Type eventType = (QEvent::Type)type;

    if (!(QEvent::KeyPress == eventType || QEvent::KeyPress == eventType || QEvent::ShortcutOverride == eventType))
        return;

    QKeyEvent *keyEvent = new QKeyEvent(eventType, key, (Qt::KeyboardModifier)modifier);

    if (0 != keyEvent)
    {
        QCoreApplication::postEvent(m_Application, keyEvent);
    }
}

void ScpiRemoteAdaptor::emulateTouch()
{

}

void ScpiRemoteAdaptor::emulateGesture()
{

}

/*! 
 * 系统操作
 */
QDBusVariant ScpiRemoteAdaptor::requestSystemSnapshot()
{
    QDBusVariant data(m_Application->snapshot());
    return data;
}

bool ScpiRemoteAdaptor::requestSystemReboot()
{
    return m_Application->reboot();
}

bool ScpiRemoteAdaptor::requestSystemShutdown()
{
    return m_Application->shutdown();
}

bool ScpiRemoteAdaptor::requestSystemRootLogin(const QString& code)
{
    return m_Application->rootLogin(code);
}

bool ScpiRemoteAdaptor::requestSystemRootLogout()
{
    return m_Application->rootLogout();
}

QString ScpiRemoteAdaptor::requestFirmwareVersion()
{
    return m_Application->firmwareVersion();
}

bool ScpiRemoteAdaptor::requestFirmwareUpgrade()
{
    return m_Application->upgradeFirmware();
}

QString ScpiRemoteAdaptor::requestSystemTime()
{
    quint64 msec = 0;
    m_Analyzer->getCurrentTime(msec);
    return QDateTime::fromMSecsSinceEpoch(msec).toString("yyyy-MM-dd hh:mm:ss");
}

bool ScpiRemoteAdaptor::requestSetSystemTime(const QString& time)
{
    quint64 msec = QDateTime::fromString(time, "yyyy-MM-dd hh:mm:ss").toMSecsSinceEpoch();
    return m_Analyzer->setCurrentTime(msec);
}

QString ScpiRemoteAdaptor::requestDeviceID()
{
    QString data("f280-1.10");
    return data;
}

bool ScpiRemoteAdaptor::requestGPSStatus()
{
    QUERY_SETUP_FIELD_BOOL("/sys/setup/userPreferenceSet/timeSet/gps", enabled);
    return enabled;
}

/*********************************************************************************************************
** End of file
*********************************************************************************************************/
