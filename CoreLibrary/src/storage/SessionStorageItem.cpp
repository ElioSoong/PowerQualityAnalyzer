/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  SessionStorageItem.cpp
** Latest Version:             V1.0.0
** Latest modified Date:       2017/3/17
** Modified by:                ZhangWenBo
** Descriptions:               
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 Chen Honghao
** Created date:               2015/12/01
** Descriptions:               Session存储类型
** 
*********************************************************************************************************/
#include "SessionStorageItem.h"
#include "IStorage.h"
#include "SetupStorageItem.h"

#define SETUP_FILE_PATH         (path() + "/Config.DAT")
#define SETUP_FIELD_PATH        SetupStorageItem::setupXPath()

SessionStorageItem::SessionStorageItem()
    : StorageItem()
    , m_sessionInfo()
{

}

SessionStorageItem::~SessionStorageItem()
{
    if (m_setupStorageItem)
    {
        delete m_setupStorageItem;
        m_setupStorageItem = NULL;
    }
    removeAllSubItems();
}

/*! 
 * 移除全部的子存储项目
 */
void SessionStorageItem::removeAllSubItems()
{
#if 0
    foreach (StorageItem* item, m_subItemList)
    {
        delete item;
    }
#endif
    m_subItemList.clear();
}

void SessionStorageItem::initializeSubItems(Activity* activity /*= NULL*/)
{
    if (activity == NULL || !m_subItemList.isEmpty())
        return;

    QList<StorageItem*> items = activity->storageItems();
    int index = 0;
    foreach(StorageItem* item, items)
    {
#if 0
        StorageItem* cloneItem = item->clone();
#endif
        QString thePath = m_path + "/" + item->typeName() + QString::number(index);
        item->setPath(thePath);
        item->setStatus(ItemIdle);
        item->setParentItem(this);
        QAbstractItemModel* theModel = item->model();
        if (theModel && Storage::instance()->currentMode() == Storage::RunningMode)
        {
            theModel->disconnect(item);
            connect(theModel, &QAbstractItemModel::dataChanged, item, &StorageItem::onModelDataChanged);
        }
        item->setDataPtr(theModel);
        m_subItemList.push_back(item);
        ++index;
    }
}

/*!
 * 设置当前存储项绑定的数据指针
 * @param[in]  data 数据指针
 */
void SessionStorageItem::setDataPtr(void* data)
{
    Activity* activity = reinterpret_cast<Activity*>(data);
    if (activity == NULL)
        return;

    int mode = activity->mode();
    m_sessionInfo.SessionType = mode;

    removeAllSubItems();

    initializeSubItems(activity);

    return StorageItem::setDataPtr(data);
}

/*! 
 * 读取当前存储项（到字段树或者存储项本身的缓冲区），不同的存储项的读取方法不同
 * @param[in]  file 当前正在操作的数据文件（已经打开并跳转至合适的偏移量）
 * @param[in]  data 数据缓冲区，由存储项本身生成并维护
 * @return     操作成功返回true
 */
bool SessionStorageItem::read(QFile* file, void* data)
{
    Q_ASSERT(0);
    return false;
}

/*! 
 * 写入当前存储项（从字段树或者存储项本身的缓冲区），不同的存储项的写入方法不同
 * @param[in]  file 当前正在操作的数据文件（已经打开并跳转至合适的偏移量）
 * @param[in]  data 数据缓冲区，由存储项本身生成并维护
 * @return     操作成功返回true
 */
bool SessionStorageItem::write(QFile* file, void* data)
{
    Q_ASSERT(0);
    return false;
}

/*! 
 * 保存存储项目数据
 */
bool SessionStorageItem::save()
{
    this->close();
    return true;
}

/*! 
 * 载入指定位置的存储项目数据
 */
bool SessionStorageItem::load()
{
    setStatus(ItemLoading);

    if (!loadSessionInfo(m_path, m_sessionInfo)) 
    {
        setStatus(ItemError);
        qDebug() << "SessionStorageItem loadSessionInfo Failed !!";
        return false;
    }

    SetupStorageItem* configItem = qobject_cast<SetupStorageItem*>(m_setupStorageItem);
    if (configItem)
    {
        bool ok = configItem->loadTree(SETUP_FILE_PATH, SETUP_FIELD_PATH);
        if (!ok)
        {
            setStatus(ItemError);
            qDebug() << "SessionStorageItem loadConfig Failed !!";
            return false;
        }
    }

    // 根据SessionType 获取测量活动 (后期直接调用ActivityManager获取)
    Activity* activity = getActivity(m_sessionInfo.SessionType);
    setDataPtr(activity);

    emit Storage::instance()->itemLoading(this);

    foreach(StorageItem* item, m_subItemList)
    {
        bool ok;
        ok = item->reload();
        if (!ok)
        {
            qDebug() << "SessionsSubItems load Failed!, typeId = " << item->type();
            continue;
        }
    }
    setStatus(ItemLoaded);
    return true;
}


bool SessionStorageItem::reload(quint64 startTime /*= 0*/, quint64 endTime /*= 0*/, int type /*= 0*/)
{
    foreach(StorageItem* item, m_subItemList)
    {
        if (item->type() & type)
        {
            item->reload(startTime, endTime);
        }
    }
    return true;
}

/*!
 * 随时强制保存未保存的文件（尤其是StorageItemInfo和SessionItemInfo），避免意外掉电彻底损坏数据
 * @return     操作成功返回true
 */
bool SessionStorageItem::flush()
{

    foreach(StorageItem* item, m_subItemList)
    {
        item->flush();
    }

    bool ok = saveSessionInfo(m_path, m_sessionInfo);

    SetupStorageItem* configItem = qobject_cast<SetupStorageItem*>(m_setupStorageItem);
    if (configItem)
    {
        configItem->saveTree(SETUP_FILE_PATH, SETUP_FIELD_PATH);
    }

    return StorageItem::flush() && ok;
}

void SessionStorageItem::close()
{
    foreach(StorageItem* item, m_subItemList)
    {
        if (!item->isAsyncronuous())
            item->close();
    }
    return StorageItem::close();
}

/*!
 * 设置当前存储项的路径（其实是文件夹名称，因为所有的存储项均存储在同一层次，每个存储项为一个文件夹）
 * @param[in]  path 当前存储项的路径
 */
bool SessionStorageItem::setPath(const QString& path)
{
    bool ok = loadSessionInfo(path, m_sessionInfo);

    return StorageItem::setPath(path) && ok;
}

bool SessionStorageItem::autoSaveItemInfo()
{
    if (status() == ItemIdle)
    {
        setStatus(ItemSaving);
    }

    if (status() == ItemSaved)
        return false;
    setAutoSavedMask();
    SetupStorageItem* configItem = qobject_cast<SetupStorageItem*>(m_setupStorageItem);
    if (configItem)
    {
        configItem->saveTree(SETUP_FILE_PATH, SETUP_FIELD_PATH);
    }
    bool ok1 = SessionStorageItem::saveSessionInfo(m_path, m_sessionInfo);
    bool ok2 = StorageItem::saveItemInfo(m_path, m_info);
    return ok1 && ok2;
}

bool SessionStorageItem::flushItemInfo()
{
    resetAutoSavedMask();
    bool ok = StorageItem::saveItemInfo(m_path, m_info);
    return ok;
}

quint32 SessionStorageItem::subType()
{
    return m_sessionInfo.SessionType;
};

/*! 
 * 载入会话信息
 * @param[in]  dir  存储项所在的文件夹
 * @param[out] info 存储项信息
 * @return     操作成功返回true
 */
bool SessionStorageItem::loadSessionInfo(const QString& dir, SessionInfo& info)
{
    QString infoPath = dir + SESSIONINFO_PATH;
    QFile infoFile(infoPath);
    if (!infoFile.open(QFile::ReadOnly))
        return false;

    int readLength = infoFile.read((char*)&info, sizeof(info));
    infoFile.close();
    if (readLength < sizeof(info))
        return false;

    if (!info.isValid())
        return false;

    return true;
}

/*! 
 * 保存会话信息
 * @param[in]  dir  存储项所在的文件夹
 * @param[out] info 存储项信息
 * @return     操作成功返回true
 */
bool SessionStorageItem::saveSessionInfo(const QString& dir, SessionInfo info)
{
    qDebug() << "SessionStorageItem::startTime = " << info.StartTime;
    QString infoPath = dir + SESSIONINFO_PATH;
    QFile infoFile(infoPath);
    if (!infoFile.open(QFile::WriteOnly))
    {
        return false;
    }

    int writeLength = infoFile.write((char*)&info, sizeof(info));
    infoFile.flush();
    infoFile.close();
    if (writeLength < sizeof(info)) 
    {
        return false;
    }

    return true;
}

void SessionStorageItem::listenSubItemStatus()
{
    // 监听各个子Item的保存状态
    if (!m_subItemList.isEmpty())
    {
        bool allSaved = false;
        foreach (StorageItem* item, m_subItemList)
        {
            allSaved = (item->status() == ItemSaved);
            if (!allSaved)
                return;
        }
        if (status() != ItemSaved)
        {
            // 恢复自动存储标记并存储
            flushItemInfo();
            Storage* storagePtr = Storage::instance();
            setStatus(ItemSaved);
            storagePtr->doRename(this);
            emit storagePtr->itemSaved(this);
        }  
    }
}

/*********************************************************************************************************
** End of file
*********************************************************************************************************/
