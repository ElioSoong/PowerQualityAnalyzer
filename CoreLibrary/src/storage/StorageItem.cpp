/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  StorageItem.h
** Latest Version:             V1.0.0
** Latest modified Date:       2017/03/07
** Modified by:                ZhangWenBo
** Descriptions:               
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 Chen Honghao
** Created date:               2015/11/25
** Descriptions:               存储项，例如测量会话的数据、限值配置值集合
** 
*********************************************************************************************************/
#include "StorageItem.h"
#include "IStorage.h"
#include <QDateTime>
#include <QDebug>
#include <QDir>

QString ItemNameProvider::getAvailableItemName(const QString & rootPath, StorageItem* item)
{

    QString path;
    QString typeName = item->typeName();
    for (int i = m_counter + 1; i != m_counter; (i = i >= 9999 ? 0 : (i + 1)))
    {
        path = rootPath + "/" + typeName + QString(" %1").arg(i, 5, 10, QChar('0'));
        if (!QFile::exists(path))
        {
            m_counter = i;
            return path;
        }
    }

    return QString();
}


#define ITEMINFO_PATH       "/ITEM.INFO"

QHash<int, QPixmap> StorageItem::s_typeIcons;
StorageItemFactory* StorageItem::s_factory = NULL;

StorageItem::StorageItem()
    : m_model(NULL),
    m_parentItem(NULL),
    m_path(), 
    m_info(), 
    m_accessFlags(NoAccess),
    m_status(ItemIdle),
    m_curFile(NULL),
    m_filePosition(),
    m_dataPtr(NULL), 
    m_typeName("UNKNOWN"), 
    m_isAsyncronuous(false)
{
    
}

StorageItem::StorageItem(const StorageItem& other)
    : m_model(NULL),
      m_parentItem(NULL)
{
    m_path = other.m_path;
    m_icon = other.m_icon;
    m_info = other.m_info;
    m_filePosition = other.m_filePosition;
    m_accessFlags = other.m_accessFlags;
    m_status = other.m_status;
    m_curFile = other.m_curFile;
    m_typeName = other.m_typeName;
    m_isAsyncronuous = other.m_isAsyncronuous;
}

StorageItem::~StorageItem()
{

}


/*! 
 * 获取当前存储项的类型
 * @return     当前存储项的类型
 */
int StorageItem::type() const
{
    return m_info.ItemType;
}

/*! 
 * 获取当前存储项的类型名称，用于自动DESCRIPTION
 * @return     当前存储项的类型名称
 */
QString StorageItem::typeName() const
{
    return m_typeName;
}

/*! 
 * 获取当前存储项的图标（与存储项类型和对应的测量模式有关，用于显示在存储视图）
 * @return     当前存储项的图标
 */
QPixmap StorageItem::icon() const
{
    int typeId = this->type();
    quint32 subTypeId = const_cast<StorageItem*>(this)->subType();
    int newTypeId = static_cast<int>((subTypeId << 16) + (typeId << 0));
    QHash<int, QPixmap>::const_iterator iter = s_typeIcons.find(newTypeId);
    if (iter != s_typeIcons.cend())
    {
        return iter.value();
    }

    QPixmap icon;
    IconProvider * provider = this->getIconProvider();
    if (provider)
    {
        icon = provider->getIconFromType(subTypeId);
    }
    else
    {
        icon = m_icon;
    }
    if (!icon.isNull())
    {
        s_typeIcons.insert(newTypeId, icon);
    }

    return icon;
}

/*! 
 * 获取当前存储项的创建时间
 * @return     当前存储项的创建时间
 */
quint64 StorageItem::createTime() const
{
    return m_info.CreateTime;
}


/*! 
 * 获取当前存储项的描述信息（用于显示在存储视图）
 * @return     当前存储项的描述信息
 */
QString StorageItem::description() const
{
    QString descr = QString::fromUtf8(m_info.Description, QString(m_info.Description).size());
    return descr;
}

/*! 
 * 设置当前存储项的描述信息（用于显示在存储视图）
 * @param[in]  desc 当前存储项的描述信息
 */
void StorageItem::setDescription(const QString& desc)
{
    QByteArray data = desc.toLocal8Bit();
    int inputLength = data.length();
    m_info.DescriptionLength = inputLength >= sizeof(m_info.Description) ? sizeof(m_info.Description) -1 : inputLength;
    strncpy(m_info.Description, data.data(), m_info.DescriptionLength);
    m_info.Description[m_info.DescriptionLength] = '\0';
}

/*! 
 * 获取当前存储项的文件信息
 * @return     当前存储项的文件信息
 */
const StorageItemInfo& StorageItem::info() const
{
    return m_info;
}

/*! 
 * 设置当前存储项的文件信息
 * @param[in]  info 当前存储项的文件信息
 */
void StorageItem::setInfo(const StorageItemInfo& info)
{
    m_info = info;
}

void StorageItem::setDeviceInfo(int serial, const QString& name,
        const QString& guiVersion, const QString& dspVersion, const QString& fpqaVersion)
{
    m_info.DeviceSerial = serial;
    saveToChar(name, m_info.DeviceName, sizeof(m_info.DeviceName));
    saveToChar(guiVersion, m_info.GUIVersion, sizeof(m_info.GUIVersion));
    saveToChar(dspVersion, m_info.DSPVersion, sizeof(m_info.DSPVersion));
    saveToChar(fpqaVersion, m_info.FPGAVersion, sizeof(m_info.FPGAVersion));
}

/*!
 * setItemTypeInfo 设置Item类型相关信息
 * @param[in]: name 存储项名称
 * @param[in]: type 存储类型
 * @param[in]: iconPath 存储图标路径
 * @return:   void
 */
void StorageItem::setItemTypeInfo(const QString & name, int type, const QString & iconPath)
{
    m_typeName = name;
    m_info.ItemType = type;
    m_icon = QPixmap(iconPath);
}

void StorageItem::setParentItem(StorageItem* item)
{
    m_parentItem = item;
    if (m_parentItem && Storage::instance()->currentMode() == Storage::RunningMode)
    {
        disconnect(this, &StorageItem::statusChanged, m_parentItem, &StorageItem::listenSubItemStatus);
        connect(this, &StorageItem::statusChanged, m_parentItem, &StorageItem::listenSubItemStatus);
    }
}


/*!
 * 获取当前存储项的路径（其实是文件夹名称，因为所有的存储项均存储在同一层次，每个存储项为一个文件夹）
 * @return     当前存储项的路径
 */
QString StorageItem::path() const
{
    return m_path;
}

/*! 
 * 设置当前存储项的路径（其实是文件夹名称，因为所有的存储项均存储在同一层次，每个存储项为一个文件夹）
 * @param[in]  path 当前存储项的路径
 */
bool StorageItem::setPath(const QString& path)
{
    m_path = path;

    QDir dirHelper;
    if (!dirHelper.exists(path))
    {
        if (!dirHelper.mkpath(path))
        {
            qWarning() << "Failed to create dir for storage item: " << path;
            return false;
        }
    }
    
    return true;
}

/**
 * 获取当前存储项的访问模式标记
 * 主要用于判断是否支持Recall和View操作，返回的数据类型是各个支持权限的位或结果。
 * @return 当前存储项的访问模式标记
 */
StorageItem::AccessFlags StorageItem::accessFlags() const
{
    return m_accessFlags;
}

/*! 
 * 获取当前存储项的一帧有多少个字节
 * @return     当前存储项的一帧有多少个字节
 */
int StorageItem::fileSizeInBytes() const
{
    return 0;
}

/*! 
 * 打开当前的存储项并将文件指针跳转至指定位置
 * @param[in]  position 指定位置
 * @return     操作成功返回true
 */
QFile* StorageItem::seek(const StorageItemPosition* position)
{
    if (position != NULL)
    {
        if (position->isValid())
        {
            m_filePosition = *position;
        }
    }
    else
    {
        m_filePosition.reset();
    }
    QString filePath = m_path + "/" + this->fileNameFromIndex(m_filePosition.m_fileIndex);
    
    if (m_curFile)
    {
#if 0
        delete m_curFile;
        m_curFile = NULL;
#else
        // 避免频繁创建对象 [1/9/2016 CHENHONGHAO]
        m_curFile->close();
#endif
    }

    
    if (m_curFile == NULL)
    {
        m_curFile = new QFile;
    }

    m_curFile->setFileName(filePath);
    if (!m_curFile->open(QFile::ReadWrite))
    {
        qWarning() << "Failed to open file @ " << filePath;
        return NULL;
    }

    quint64 fileSize = this->fileSizeInBytes();

    quint64 offset = fileSize * m_filePosition.m_frameIndex;
    if (!m_curFile->seek(offset))
    {
        qWarning() << "Failed to seek file @ " << filePath 
                   << " to frame " << m_filePosition.m_frameIndex 
                   << "(offset=" << offset << ")";
        return NULL;
    }

    return m_curFile;
}

/*! 
 * 随时强制保存未保存的文件（尤其是StorageItemInfo和SessionItemInfo），避免意外掉电彻底损坏数据
 * @return     操作成功返回true
 */
bool StorageItem::flush()
{
    m_info.AccessTime = QDateTime::currentMSecsSinceEpoch();
    return StorageItem::saveItemInfo(m_path, m_info);
}

/**
 * 关闭并保存当前存储项
 * 处于SaveMode模式时，close前保存截图
 */
void StorageItem::close()
{
    if (m_curFile)
    {
        m_curFile->flush();
        m_curFile->close();
        delete m_curFile;
        m_curFile = NULL;
    }
    if (m_model)
    {
        m_model->disconnect(this);
    }
    m_filePosition.reset();
}

// From: http://www.xuebuyuan.com/994757.html
bool DeleteDirectory(const QString &path)
{
    if (path.isEmpty())
        return false;

    QDir dir(path);
    if(!dir.exists())
        return true;

    dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
    QFileInfoList fileList = dir.entryInfoList();
    foreach (QFileInfo fi, fileList)
    {
        if (fi.isFile())
        {
            fi.dir().remove(fi.fileName());
        }
        else
        {
            DeleteDirectory(fi.absoluteFilePath());
        }
    }

    if (!dir.exists())
        return true;
    else
        return dir.rmpath(dir.absolutePath());
}

/**
 * 将当前存储项从操作系统中移除
 * @return 操作成功返回true
 */
bool StorageItem::remove()
{
    close();
    m_locker.lock();
    QString thePath = m_path;
    m_locker.unlock();
    return DeleteDirectory(thePath);
}

/*! 
 * 清空当前存储项，其数据全部删除，创建时间置为当前时间，但类型等配置不变
 * @return     操作成功返回true
 */
bool StorageItem::restart()
{
    close();

    // 删除全部数据文件，仅保留ITEM.INFO
    remove();

    // 创建空文件夹
    bool ok1 = setPath(m_path);

    // 更新创建时间，回写ITEM.INFO
    m_info.CreateTime = QDateTime::currentMSecsSinceEpoch();
    bool ok2 = saveItemInfo(m_path, m_info);

    return ok1 && ok2;
}

/*! 
 * 存储项工厂
 * @param[in]  typeId  存储项类型
 * @param[in]  storage 存储接口
 * @return     操作成功返回工厂生产的存储项对象，否则返回NULL
 */
StorageItem* StorageItem::create(int typeId)
{
    Q_ASSERT(s_factory);
    return s_factory->createItem(typeId);
}

/*! 
 * 根据指定的路径（实际上是一个文件夹）打开路径内的存储项
 * @param[in]  path  指定的路径
 * @param[in]  flags 访问模式（打开用来做什么）
 * @return     操作成功返回打开的存储项对象，否则返回NULL
 */
StorageItem* StorageItem::open(const QString& path, StorageItem::AccessFlags flags)
{
    StorageItemInfo info;
    if (!loadItemInfo(path, info))
        return NULL;

    int typeId = info.ItemType;
    StorageItem* item = StorageItem::create(typeId);
    if (item == NULL)
        return NULL;

    item->setInfo(info);
    item->setPath(path);

    return item;
}

/*! 
 * 读取当前存储项（到字段树或者存储项本身的缓冲区），不同的存储项的读取方法不同
 * @param[in]  file 当前正在操作的数据文件（已经打开并跳转至合适的偏移量）
 * @param[in]  data 数据缓冲区，由存储项本身生成并维护
 * @return     操作成功返回true
 */
bool StorageItem::read(QFile* file, void* data)
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
bool StorageItem::write(QFile* file, void* data)
{
    Q_ASSERT(0);
    return false;
}

/*! 
 * 异步读取当前存储项
 * @param[in]  position 偏移量
 * @param[in]  data 数据缓冲区，由存储项本身生成并维护
 * @return     操作成功返回true
 */
void StorageItem::asyncRead(const StorageItemPosition* position, void* data)
{
    StorageThread* thread = NULL;
    if (position == NULL || Storage::instance() == NULL || (thread = Storage::instance()->storageThread()) == NULL)
        return;

    StorageIORequest request(this, *position, false, data);
    if (!thread->isInterrupet())
    {
        thread->request(request);
    }
}

/*! 
 * 异步写入当前存储项
 * @param[in]  position 偏移量
 * @param[in]  data 数据缓冲区，由存储项本身生成并维护
 * @return     操作成功返回true
 */
void StorageItem::asyncWrite(const StorageItemPosition* position, void* data)
{
    StorageThread* thread = NULL;
    if (position == NULL || Storage::instance() == NULL || (thread = Storage::instance()->storageThread()) == NULL)
        return;

    StorageIORequest request(this, *position, true, data);
    if (!thread->isInterrupet())
    {
        thread->request(request);
    }
}

/*! 
 * 根据文件序号（存储项文件夹内的文件编号）生成对应的文件名，默认格式是nnnnn.DAT
 * @param[in]  fileIndex 文件序号（存储项文件夹内的文件编号）
 * @return     操作成功返回文件序号对应的文件名
 */
QString StorageItem::fileNameFromIndex(int fileIndex) const
{
    return QString("%1.DAT").arg(fileIndex, 6, 10, QChar('0'));
}

/*! 
 * 载入存储项信息
 * @param[in]  dir  存储项所在的文件夹
 * @param[out] info 存储项信息
 * @return     操作成功返回true
 */
bool StorageItem::loadItemInfo(const QString& dir, StorageItemInfo& info)
{
    QString infoPath = dir + ITEMINFO_PATH;
    QFile infoFile(infoPath);
    if (!infoFile.open(QFile::ReadOnly)) 
    {
        return false;
    }

    int readLength = infoFile.read((char*)&info, sizeof(info));
    infoFile.close();
    if (readLength < sizeof(info)) 
    {
        return false;
    }
    
    bool ok = info.isValid();
    return ok;
}

bool StorageItem::saveToChar(const QString & srcString, char * destChar, int length)
{
    if (destChar == NULL)
        return false;
    QByteArray data = srcString.toLocal8Bit();
    int inputLength = data.length();
    inputLength = inputLength >= length ? length - 1 : inputLength;
    strncpy(destChar, data.data(), inputLength);
    *(destChar + inputLength) = '\0';
    return true;
}

/*!
 * 保存存储项信息
 * @param[in]  dir  存储项所在的文件夹
 * @param[out] info 存储项信息
 * @return     操作成功返回true
 */
bool StorageItem::saveItemInfo(const QString& dir, StorageItemInfo info)
{
    QString infoPath = dir + ITEMINFO_PATH;
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

/*! 
 * 保存存储项目数据(默认单帧同步保存)
 */
bool StorageItem::save()
{
    bool ok = true;
    QFile* file = this->seek(NULL);
    if (file == NULL)
        return false;
    ok = this->write(file, this->dataPtr<void*>());

    this->flush();
    this->close();

    setStatus(ItemSaved);
    return ok;
}

/*! 
* 载入指定位置的存储项目数据(默认单帧同步加载)
*/
bool StorageItem::load()
{

    if (!loadItemInfo(m_path, m_info))
    {
        qDebug() << "StorageItem loadItemInfo Failed !!";
        return false;
    }

    bool ok = true;
    QFile* file = this->seek(NULL);
    if (file == NULL)
        return false;
    ok = this->read(file, this->dataPtr<void*>());

    this->close();

    setStatus(ItemLoaded);
    if (m_model)
    {
        emit m_model->dataChanged(QModelIndex(), QModelIndex());
    }

    return ok;
}

#if 0
/*!
* 克隆当前对象，返回一个重名但数据独立的存储项对象
* @return     StorageItem*
*/
StorageItem* StorageItem::clone() const
{
    const QMetaObject* metaObject = this->metaObject();
    int constructorCount = metaObject->constructorCount();
    const char* metaType = metaObject->className();
    int metaTypeId = QMetaType::type(metaType);
    if (metaTypeId == QMetaType::UnknownType)
    {
        qCritical() << "Failed to clone StorageItem of " << metaType << " type, you may need to register this class to the Meta-System.";
        return NULL;
    }

    return reinterpret_cast<StorageItem*>(QMetaType::create(metaTypeId, this));
}
#endif

/*********************************************************************************************************
** End of file
*********************************************************************************************************/
