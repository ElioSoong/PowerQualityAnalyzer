/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  IStorage.h
** Latest Version:             V1.0.0
** Latest modified Date:       2017/03/07
** Modified by:                ZhangWenBo
** Descriptions:               
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 Chen Honghao
** Created date:               2015/11/25
** Descriptions:               存储接口，通常作为存储操作对应用程序的唯一接口
** 
*********************************************************************************************************/
#include "IStorage.h"
#include <QDirIterator>
#include <QDebug>
#ifndef WIN32
#include <sys/statfs.h> 
#include <sys/sysinfo.h>
#endif

#define     AUTO_SAVED_DESCRIPTION      "AutoSavedItem"

Storage* Storage::s_instance = NULL;

/*
 * 禁止显式调用构造/析构函数
 */
Storage::Storage() :
    m_curStorageItem(NULL),
    m_storageThread(NULL),
    m_storageFilter(),
    m_curStorageMode(Storage::IdleMode),
#ifdef WIN32
    m_rootPath("./storage")
#else
    m_rootPath("/mnt/sd1")
#endif
{
    m_storageItemList.clear();
    // 创建后台存储线程
    StorageThread* storageThread = new StorageThread(NULL);
    storageThread->start();
    m_storageThread = storageThread;

    // 创建数据模型
    m_storageModel = new StorageModel(this);
    m_storageModel->setObjectName("storageModel");

    connect(this, &Storage::itemToRemove, this, &Storage::doRemoveItem);

    s_instance = this;
}

Storage::~Storage()
{
    removeAllItems();
}

/*! 
 * 枚举全部存储项，并初步获取存储项的类型、时间、图标等信息，但并不加载存储项
 * @param[in]  rootPath   指定的存储根目录
 * @param[in]  typeFilter 存储项类型过滤器，若指定了合法类型，则只枚举该类型
 * @return     枚举到的存储项个数，枚举到的存储项列表将缓存在存储接口内部
 */
int Storage::enumerate(const QString& rootPath, int typeFilter)
{
    static StorageItem* preAutoItem = NULL;
    removeAllItems();
    preAutoItem = NULL;
    m_rootPath = rootPath;

    QDirIterator iter(rootPath, QDir::Dirs | QDir::NoDotAndDotDot, QDirIterator::FollowSymlinks);
    while (iter.hasNext()) 
    {
        QString dirPath = iter.next();
        StorageItem* item = StorageItem::open(dirPath, StorageItem::EnumAccess);
        if (item == NULL)
        {
            qWarning() << "Failed to enumerate storage-item @ " << dirPath;           
            continue;
        }
        insertItem(item, -1);
#if 0
        ItemNameProvider* nameProvider = item->getItemNameProvider();
        Q_ASSERT(nameProvider);
        nameProvider->increaseCounter();
#endif

        // 处理自动存储项
        if (item->isAutoSavedItem())
        {
            QDir dirHelper;
            QString autoSavedPath = m_rootPath + QString("/") + AUTO_SAVED_DESCRIPTION;
            if (dirHelper.exists(autoSavedPath) && preAutoItem)
            {
                remove(preAutoItem);
            }
            preAutoItem = item;
            item->setDescription(AUTO_SAVED_DESCRIPTION);
            doRename(item);
        }
    }

    sortAllItems();

    return m_storageItemList.size();
}


/*! 
 * 排序全部存储项，默认按照时间从新到旧排序
 * @param[in]  column     提供排序依据的列（对应存储视图：0=Date,1=Time,2=Description,3=Type）
 * @param[in]  descending 是否降序排列，精确地说是反序排列：日期和时间从旧到新，描述从小到大，类型从高到低
 */
void Storage::sortAllItems(int column, bool descending)
{
    // 目前仅支持按时间从新到旧排列
    Q_ASSERT(column == 0);
    Q_ASSERT(descending == false);

    typedef QList<StorageItem*>::iterator ItemIterator;
    for (ItemIterator iter1 = m_storageItemList.begin(); iter1 != m_storageItemList.end(); iter1++)
    {
        for (ItemIterator iter2 = iter1 + 1; iter2 != m_storageItemList.end(); iter2++)
        {
            // 如果前面的项目比后面的项目更旧，则交换
            if (*(*iter1) < *(*iter2))
            {
                qSwap<StorageItem*>(*iter1, *iter2);
            }
        }
    }
}

/*! 
 * 新建一个指定类型的存储项（存储项的工厂）
 * @param[in]  type 存储项的类型
 * @return     创建的存储项
 * @note       该函数将创建对应的对象（构造函数中可能绑定字段树或图片），并为存储项自动命名
 */
StorageItem* Storage::createItem(int type, void* data)
{
    StorageItem* item = StorageItem::create(type);
    if (item == NULL)
        return NULL;
    ItemNameProvider* nameProvider = item->getItemNameProvider();
    Q_ASSERT(nameProvider);
    QString path = nameProvider->getAvailableItemName(m_rootPath, item);
    item->setPath(path);
    int length = path.length();
    int index = path.lastIndexOf("/");
    item->setDescription(path.mid((index + 1), (length - index)));
    item->setDataPtr(data);

    return item;
}

/*! 
 * 获取已经枚举到的存储项个数
 * @return     已经枚举到的存储项个数
 */
int Storage::itemCount() const
{
    return m_storageItemList.size();
}

/*! 
 * 获取指定位置的存储项对象
 * @param[in]  index 存储项序号
 * @return     存储项对象
 */
StorageItem* Storage::itemAt(int index)
{
    if (index < 0 || index >= m_storageItemList.size())
        return NULL;

    return m_storageItemList.at(index);
}

/*! 
 * 保存指定位置的存储项目，并将该项目加入存储项列表，从而显示在存储视图
 * @param[in]  item 指定的存储项目
 * @return     操作成功返回true
 */
bool Storage::save(StorageItem* item)
{
    if (item == NULL)
        return false;

    insertItem(item, 0);
    bool ok = true;
    ok = item->save();

    return ok;
}


/*! 
 * 载入指定位置的存储项目
 * @param[in]  item 指定的存储项目
 * @return     操作成功返回true
 */
bool Storage::load(StorageItem* item)
{
    if (item == NULL)
        return false;   

    // 异步加载时需要标记加载模式
    if (item->isAsyncronuous())
    {
        setMode(LoadMode);
    }
    bool ok = true;
    ok = item->load();

    return ok;
}

/*!
*  重新加载指定类型的数据
* @param[in] startTime 起始时间
* @param[in] endTime   结束时间
* @param[in] type      reload存储数据类型
* @return  操作成功返回true
*/
bool Storage::reload(quint64  startTime, quint64  endTime, int type)
{
    Q_ASSERT(m_curStorageItem);
    bool ok = m_curStorageItem->reload(startTime, endTime, type);
    return ok;
}

/*! 
 * 重命名指定的存储项目到指定位置
 * @param[in]  item    存储项目
 * @param[in]  newPath 新的保存位置
 * @return     操作成功返回true
 */
bool Storage::rename(StorageItem* item, const QString& newPath)
{
    if (item == NULL)
        return false;

    item->setDescription(newPath);

    StorageItemInfo theInfo = item->info();
    StorageItem::saveItemInfo(item->path(), theInfo);

    if (m_storageModel)
    {
        m_storageModel->reset();
    }

    // 文件重命名
    doRename(item);

    return true;
}

/**
 * 删除指定的存储项目，此处命名为remove只是为了避开C++的delete关键字
 * @param[in]  item 指定的存储项目
 * @return     操作成功返回true
 */
bool Storage::remove(StorageItem* item)
{
    if (item == NULL)
        return false;

    bool ok1 = item->remove();
    bool ok2 = removeItem(item);

    delete item;

    return ok1 && ok2;
}

/**
* 删除所有存储项目，此处命名为remove只是为了避开C++的delete关键字
* @return     操作成功返回true
*/
bool Storage::removeAll()
{
    bool ok = false;
    foreach (StorageItem* item, m_storageItemList)
    {
        ok = item->remove();
    }
    removeAllItems();

    return ok;
}

/*! 
 * 设置存储模式
 * @param[in]  mode 存储模式
 * @return     操作成功返回true
 */
bool Storage::setMode(StorageMode mode)
{
    m_curStorageMode = mode;
    return true;
}

/*!
 * 内部调用的帮助函数，向存储项队列中添加一个新项目，并刷新StorageModel
 * @param[in]  item  存储项
 * @param[in]  index 添加位置，0代表起始项目的前面，-1代表末尾项目的后面，itemCount-1代表末尾项目的前面
 * @return     操作成功返回true
 */
bool Storage::insertItem(StorageItem* item, int index)
{
    if (item == NULL)
        return false;

    int position = m_storageItemList.indexOf(item);
    if (position >= 0)
        return false;

    m_storageItemList.insert(index >= 0 ? index : m_storageItemList.size(), item);

    if (m_storageModel)
    {
        m_storageModel->reset();
    }

    return true;
}

/*! 
 * 内部调用的帮助函数，从存储项队列中移除一个新项目，并刷新StorageModel
 * @param[in]  item  存储项
 * @return     操作成功返回true
 */
bool Storage::removeItem(StorageItem* item)
{
    int count = m_storageItemList.removeAll(item);
    if (count < 0)
        return false;
    
#if 0
    ItemNameProvider* nameProvider = item->getItemNameProvider();
    Q_ASSERT(nameProvider);
    nameProvider->decreaseCounter();
#endif

    if (m_storageModel)
    {
        m_storageModel->reset();
    }

    return true;
}

/*! 
 * 内部调用的帮助函数，从存储项队列中移除全部项目，并刷新StorageModel
 */
void Storage::removeAllItems()
{
    for (int i = 0; i < m_storageItemList.size(); i++)
    {
#if 0
        ItemNameProvider* nameProvider = m_storageItemList.at(i)->getItemNameProvider();
        if (nameProvider)
        {
            nameProvider->resetCounter();
        }
#endif
        delete m_storageItemList.at(i);
    }
    m_storageItemList.clear();

    if (m_storageModel)
    {
        m_storageModel->reset();
    }
}

bool Storage::doRename(StorageItem* item)
{
    if (item == NULL)
        return false;

    QDir dirHelper;
    QString path = item->path();

    if (!path.startsWith(m_rootPath))
        return false;

    QString desc = item->description();

    QString newPath = m_rootPath + "/" + desc;

    bool ok = false;
    if (dirHelper.exists(path) && newPath != path)
    {
        ok = dirHelper.rename(path, newPath);
        if (ok)
        {
            item->setPath(newPath);
        }
    }
    return ok;
}

bool Storage::availableDiskSpace()
{
#ifndef WIN32
    struct statfs diskInfo;

    const char * path = "/mnt/sd1";
    statfs(path, &diskInfo);
    //每个block里包含的字节数  
    quint64 blocksize = diskInfo.f_bsize;
    //可用空间大小 
    quint64 availableDisk = diskInfo.f_bavail * blocksize;

    printf("The Disk Space is %llu M available, path = %s\n", (availableDisk >> 20), path);
    // 大于32M表示可用
    if ((availableDisk >> 20) > 32)
        return true;
    else
        return false;
#else
    return true;
#endif
}

/*!
* 启动了一个新的测量活动（包括Restart）
* @param[in]  activity 测量活动
*/
void Storage::onActivityStarted(Activity* activity)
{
    setMode(Storage::RunningMode);

    // 创建对应的存储项
    int type = toStorageType(activity);
    StorageItem* item = createItem(type, activity);

    if (item == NULL)
    {
        qWarning() << "Failed to create session storage item.";
        return;
    }

    m_curStorageItem = item;
}

/*!
* 结束了一个新的测量活动（包括Restart）
* @param[in]  activity 测量活动
*/
void Storage::onActivityStopped(Activity* activity)
{
    Q_ASSERT(m_curStorageItem);

    // 关闭当前的存储项
    if (m_curStorageItem && m_curStorageMode == RunningMode)
    {
        m_curStorageItem->flush();
        m_curStorageItem->close();
        m_curStorageItem = NULL;
    }

    setMode(Storage::IdleMode);
}

bool Storage::doRemoveItem(StorageItem* item)
{
    //return remove(item); 退出时删除有可能缓存文件很大影响主线程故放到线程中执行
    emit m_storageThread->doRemoveItem(item);
    return true;
}

/*********************************************************************************************************
** End of file
*********************************************************************************************************/
