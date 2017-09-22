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
#pragma once
#ifndef STORAGEITEM_H
#define STORAGEITEM_H

#include "storagelibrary_global.h"
#include <QObject>
#include <QPixmap>
#include <QFile>
#include <QHash>
#include <QDateTime>
#include <QMutex>
#include <QAbstractItemModel>


class Storage;

/**
 *  @class StorageItemInfo
 *  @brief 某个存储项的文件信息集合，例如类型、大小等
 */
class STORAGELIBRARY_EXPORT StorageItemInfo
{
public:
    quint32  Magic;                             //!< 存储项的Magic（0x4d455449），用于校验有效性
    quint32  Revision;                          //!< 存储格式的版本号，用于载入多版本的存储文件
    quint32  Size;                              //!< sizeof(StorageItemInfo)=256
    quint32  Checksum;                          //!< StorageItemInfo的校验和
    quint32  ItemType;                          //!< 存储项类型
    quint32  ItemSize;                          //!< 当前存储项的总大小（包括sizeof(StorageItemInfo)=256本身）
    quint32  ItemTag;                           //!< 存储项在当前文件夹的文件序号，从1开始命名
    quint32  FileCount;                         //!< 存储项中文件数量

    quint64  CreateTime;                        //!< 创建UTC时间
    quint64  AccessTime;                        //!< 最后访问（读/写）UTC时间

    quint32 AutoSavedMark;                      //!< 自动存储标记 1代表true 0代表false

    quint32  DescriptionLength;                 //!< 描述信息长度
    char Description[200];                      //!< 描述信息（存储前必须转换为UTF-8格式，可通过帮助函数设置一个QString）

    char DeviceName[256];                       //!< 设备名称

    quint32 DeviceSerial;                       //!< 序列号
    quint32 Reserved[31];                       //!< 保留字段

    char GUIVersion[128];                       //!< gui版本号

    char DSPVersion[128];                       //!< dsp版本号
    char FPGAVersion[128];                      //!< fpga版本号


public:
    StorageItemInfo(quint32 type = 0) : 
        Magic(0x4d455449),
        Revision(0x01000001),
        Size(1024),
        Checksum(0),
        ItemType(type),
        ItemSize(0),
        ItemTag(0),
        FileCount(1),
        CreateTime(0),
        AccessTime(0),
        AutoSavedMark(0),
        DescriptionLength(0),
        DeviceSerial(0)
    {
        AccessTime = CreateTime = QDateTime::currentMSecsSinceEpoch();
        memset(Description, 0, sizeof(Description));
        memset(GUIVersion, 0, sizeof(GUIVersion));
        memset(DSPVersion, 0, sizeof(DSPVersion));
        memset(FPGAVersion, 0, sizeof(FPGAVersion));
        memset(DeviceName, 0, sizeof(DeviceName));
    }

    bool isValid() const
    {
        return Magic == 0x4d455449 && Revision == 0x01000001 && Size == sizeof(StorageItemInfo) && Checksum == 0;
    }
};

class STORAGELIBRARY_EXPORT StorageItemPosition
{
public:
    int m_fileIndex;                        //!< 当前存储项文件夹内的文件序号
    int m_fileFrameCount;                   //!< 一个数据文件内存储多少帧数据
    int m_frameIndex;                       //!< 按帧存储的文件内的帧序号
    int m_frameOffset;                      //!< 【保留字段】帧内的数据点偏移量（LoopedFrameBuffer的起始偏移量）
    int m_pointCount;                       //!< 一帧数据所包含的点数
    int m_pointIndex;                       //!< 当前点对应的位置索引
    bool m_endFlag;                         //!< 是否是结束POS标志(默认false)

public:
    StorageItemPosition(int fileFrameCount = 1) : m_fileFrameCount(fileFrameCount)
    {
        reset();
    }

    inline bool isValid() const
    {
        return m_fileIndex >= 0 ;
    }

    inline bool operator <= (StorageItemPosition & other) 
    {
        return m_fileIndex <= other.m_fileIndex;
    }

    inline bool operator == (StorageItemPosition & other) 
    {
        return m_fileIndex == other.m_fileIndex;
    }

    inline const StorageItemPosition operator++(int)
    {
        m_fileIndex ++;
        m_pointIndex = 0;
        return *this;
    }

    void reset()
    {
        m_fileIndex = 0;
        m_frameIndex = 0;
        m_frameOffset = 0;
        m_pointCount = 0;
        m_pointIndex = 0;
        m_endFlag = false;
    }

    StorageItemPosition nextFrame() const
    {
        StorageItemPosition pos = *this;
        if (++pos.m_frameIndex >= m_fileFrameCount)
        {
            pos.m_frameIndex = 0;
            pos.m_fileIndex++;
        }
        
        return pos;
    }

    StorageItemPosition prevFrame() const
    {
        StorageItemPosition pos = *this;
        if (--pos.m_frameIndex < 0)
        {
            pos.m_frameIndex = m_fileFrameCount-1;
            pos.m_fileIndex--;
            if (pos.m_fileIndex < 0)
            {
                pos.reset();
            }
        }

        return pos;
    }
};

class StorageItem;

/**
*  @class StorageItemFactory
*  @brief 创建StorageItem的抽象工厂
*/
class STORAGELIBRARY_EXPORT StorageItemFactory
{
public:
    StorageItemFactory()
    {

    }

    /*!
    * 存储项工厂
    * @param[in]  typeId  存储项类型
    * @return     操作成功返回工厂生产的存储项对象，否则返回NULL
    */
    virtual StorageItem* createItem(int typeId) = 0;
};

class STORAGELIBRARY_EXPORT ItemNameProvider
{
public:
    ItemNameProvider()
        : m_counter(0)
    {

    }

#if 0
    void resetCounter()
    {
        m_counter = 0;
    }

    void increaseCounter()
    {
        m_counter++;
    }

    void decreaseCounter()
    {
        m_counter--;
    }
#endif

    /*!
    * 获取一个空闲可用的存储项名称（其实是一个五位数的数字编号），用于正在创建的新存储项
    * @param[in] 存储项根路径
    * @return     创建的存储项名称
    */
    virtual QString getAvailableItemName(const QString & rootPath, StorageItem* item);

protected:
    int  m_counter;
};


class IconProvider;

/**
 *  @class StorageItem
 *  @brief 存储项对象
 */
class STORAGELIBRARY_EXPORT StorageItem : public QObject
{
    Q_OBJECT
    Q_ENUMS(StorageStatus)
    Q_PROPERTY(StorageStatus status READ status WRITE setStatus NOTIFY statusChanged)
    Q_PROPERTY(QObject* model READ model WRITE setModel)
    Q_PROPERTY(QString description READ description WRITE setDescription)
    Q_PROPERTY(quint64 accessTime READ accessTime WRITE setAccessTime)
    Q_PROPERTY(QPixmap icon READ icon WRITE setIcon)
    Q_PROPERTY(QString typeName READ typeName MEMBER m_typeName)
    //Q_PROPERTY(quint32 type READ type MEMBER m_info.ItemType)

public:

    /**
     *  @enum  AccessFlags
     *  @brief 存储项的访问权限
     */
    enum AccessFlags
    {
        NoAccess = 0,                       //!< 没有需要权限的操作，例如枚举所有文件时不需要执行任何操作
                                            //!< 
        RecallAccess = 0x00000001,          //!< Recall
        ViewAccess   = 0x00000002,          //!< View
        RenameAccess = 0x00000004,          //!< Rename
        RemoveAccess = 0x00000008,          //!< Remove，即Delete

        EnumAccess   = NoAccess,
        LoadAccess   = RecallAccess | ViewAccess
    };

    /**
     *  @enum  StorageStatus
     *  @brief 存储项的存储状态
     */
    enum StorageStatus
    {
        ItemIdle = 0,
        ItemSaving = 0x00000001,
        ItemSaved = 0x00000002,
        ItemLoading = 0x00000004,
        ItemLoaded = 0x00000008,
        ItemError = 0x00000010
    };

signals:
    void statusChanged();

public:
    StorageItem();
    StorageItem(const StorageItem& other);
    virtual ~StorageItem();

    /*! 
     * Model & SetModel
     */
    QAbstractItemModel* model() const
    {
        return m_model;
    }
    void setModel(QObject* model)
    {
        m_model = reinterpret_cast<QAbstractItemModel*>(model);
    }

    /*!
    * parentItem & setParentItem
    */
    StorageItem* parentItem() const
    {
        return m_parentItem;
    }
    void setParentItem(StorageItem* item);

    StorageStatus status()
    {
        m_locker.lock();
        StorageStatus theStatus = m_status;
        m_locker.unlock();
        return theStatus;
    }
    void setStatus(StorageStatus theStatus)
    {
        m_locker.lock();
        m_status = theStatus;
        m_locker.unlock();
        emit statusChanged();
    }

    /*! 
     *  自动保存的Item标志处理
     */
    bool isAutoSavedItem()
    {
        m_locker.lock();
        int autoSaved = m_info.AutoSavedMark;
        m_locker.unlock();
        return autoSaved == 1;
    }
    void setAutoSavedMask()
    {
        m_locker.lock();
        m_info.AutoSavedMark = 1;
        m_locker.unlock();
    }
    void resetAutoSavedMask()
    {
        m_locker.lock();
        m_info.AutoSavedMark = 0;
        m_locker.unlock();
    }

    /*! 
     * 获取当前存储项的类型
     * @return     当前存储项的类型
     */
    int type() const;

    /*! 
     * 获取当前存储项的子类型
     * @return     当前存储项的子类型
     */
    virtual quint32 subType()
    {
        return 0;
    };

    /*! 
     * 获取当前存储项的类型名称，用于自动DESCRIPTION
     * @return     当前存储项的类型名称
     */
    QString typeName() const;

    /*! 
     * 获取当前存储项的图标（与存储项类型和对应的测量模式有关，用于显示在存储视图）
     * @return     当前存储项的图标
     */
    QPixmap icon() const;

    void setIcon(const QPixmap& pix)
    {
        m_icon = pix;
    }

    /*! 
     * 获取当前存储项的创建时间
     * @return     当前存储项的创建时间
     */
    quint64 createTime() const;

    /*! 
     * 设置当前存储项的创建时间
     * @para[in]   time  存储项的创建时间
     */
    void setCreateTime(quint64 time)
    {
        m_info.CreateTime = time;
    }

    /*!
    * 获取当前存储项的保存访问时间
    * @return     当前存储项的创建时间
    */
    quint64 accessTime() const
    {
        return m_info.AccessTime;
    }

    void setAccessTime(quint64 time)
    {
        m_info.AccessTime = time;
    }

    /*! 
     * 获取当前存储项的描述信息（用于显示在存储视图）
     * @return     当前存储项的描述信息
     */
    QString description() const;

    /*! 
     * 设置当前存储项的描述信息（用于显示在存储视图）
     * @param[in]  desc 当前存储项的描述信息
     */
    void setDescription(const QString& desc);

    /*! 
     * 获取当前存储项的文件信息
     * @return     当前存储项的文件信息
     */
    const StorageItemInfo& info() const;

    /*! 
     * 设置当前存储项的文件信息
     * @param[in]  info 当前存储项的文件信息
     */
    void setInfo(const StorageItemInfo& info);

    /*!
     * setDeviceInfo 设置设备型号相关信息
     * @param[in]: serial 序列号
     * @param[in]: name   设备名
     * @param[in]: guiVersion GUI版本号
     * @param[in]: dspVersion DSP版本号
     * @param[in]: fpqaVersion FPGA版本号
     * @return:   void
     */
    void setDeviceInfo(int serial, const QString& name,
            const QString& guiVersion, const QString& dspVersion, const QString& fpqaVersion);

    /*!
    * setItemTypeInfo 设置Item类型相关信息
    * @param[in]: name 存储项名称
    * @param[in]: type 存储类型
    * @param[in]: iconPath 存储图标路径
    * @return:   void
    */
    void setItemTypeInfo(const QString & name, int type, const QString & iconPath);

    /*! 
     * 获取当前存储项的路径（其实是文件夹名称，因为所有的存储项均存储在同一层次，每个存储项为一个文件夹）
     * @return     当前存储项的路径
     */
    QString path() const;

    /*! 
     * 设置当前存储项的路径（其实是文件夹名称，因为所有的存储项均存储在同一层次，每个存储项为一个文件夹）
     * @param[in]  path 当前存储项的路径
     */
    virtual bool setPath(const QString& path);

    /**
     * 获取当前存储项的访问模式标记
     * 主要用于判断是否支持Recall和View操作，返回的数据类型是各个支持权限的位或结果。
     * @return 当前存储项的访问模式标记
     */
    AccessFlags accessFlags() const;

    /*! 
     * 获取当前存储项的一帧文件有多少个字节
     * @return     当前存储项的一帧文件有多少个字节
     */
    virtual int fileSizeInBytes() const;

    /*! 
     * 获取当前存储项绑定的数据指针
     * @return     数据指针
     */
    template <typename T> 
    T dataPtr() const
    {
        return reinterpret_cast<T>(m_dataPtr);
    }

    /*! 
     * 设置当前存储项绑定的数据指针
     * @param[in]  data 数据指针
     */
    virtual void setDataPtr(void* data)
    {
        m_dataPtr = data;
    }

    bool isAsyncronuous() const
    {
        return m_isAsyncronuous;
    }

    QFile* currentFile()
    {
        return m_curFile;
    }

    StorageItemPosition& currentFilePosition()
    {
        return m_filePosition;
    }

    void setCurrentFilePosition(const StorageItemPosition& pos)
    {
        m_filePosition = pos;
    }

    inline bool operator < (const StorageItem& other) const
    {
        return createTime() < other.createTime();
    }

    static void setStorageItemFactory(StorageItemFactory* factory)
    {
        s_factory = factory;
    }

public:

    /*! 
     * 存储项工厂
     * @param[in]  typeId  存储项类型
     * @return     操作成功返回工厂生产的存储项对象，否则返回NULL
     */
    static StorageItem* create(int typeId);

    /*! 
     * 根据指定的路径（实际上是一个文件夹）打开路径内的存储项
     * @param[in]  path  指定的路径
     * @param[in]  flags 访问模式（打开用来做什么）
     * @return     操作成功返回打开的存储项对象，否则返回NULL
     */
    static StorageItem* open(const QString& path, AccessFlags flags);

    /*! 
     * 随时强制保存未保存的文件（尤其是StorageItemInfo和SessionItemInfo），避免意外掉电彻底损坏数据
     * @return     操作成功返回true
     */
    virtual bool flush();

    /**
     * 关闭并保存当前存储项
     * 处于SaveMode模式时，close前保存截图
     */
    virtual void close();

    /**
     * 将当前存储项从操作系统中移除
     * @return 操作成功返回true
     */
    bool remove();

    /*! 
     * 清空当前存储项，其数据全部删除，创建时间置为当前时间，但类型等配置不变
     * @return     操作成功返回true
     */
    bool restart();

public:
    /*! 
     * 打开当前的存储项并将文件指针跳转至指定位置
     * @param[in]  position 指定位置
     * @return     指定位置对应的文件对象，已经跳转至正确位置，可以直接进行I/O操作
     */
    virtual QFile* seek(const StorageItemPosition* position = NULL);

    /*! 
     * 异步读取当前存储项
     * @param[in]  position 偏移量
     * @param[in]  data 数据缓冲区，由存储项本身生成并维护
     * @return     操作成功返回true
     */
    virtual void asyncRead(const StorageItemPosition* position, void* data = NULL);

    /*! 
     * 异步写入当前存储项
     * @param[in]  position 偏移量
     * @param[in]  data 数据缓冲区，由存储项本身生成并维护
     * @return     操作成功返回true
     */
    virtual void asyncWrite(const StorageItemPosition* position, void* data = NULL);

    /*! 
     * 显示加载完成的对应Item的视图
     */
    virtual void showStorageItemView(void *data = 0, void *pview = 0)
    {
        return;
    }
    
    /*! 
     * 保存存储项目数据(默认单帧同步保存)
     */
    virtual bool save();

    /*! 
     * 载入指定位置的存储项目数据(默认单帧同步加载)
     */
    virtual bool load();

     /*! 
     *  重新加载指定类型的数据
     * @param[in] startTime 起始时间
     * @param[in] endTime   结束时间
     * @param[in] type      reload存储数据类型
     * @return  操作成功返回true   
     */
    virtual bool reload(quint64  startTime = 0, quint64  endTime = 0, int type = 0)
    {
        return false;
    }

     /*! 
     * 读取当前存储项（到字段树或者存储项本身的缓冲区），不同的存储项的读取方法不同
     * @param[in]  file 当前正在操作的数据文件（已经打开并跳转至合适的偏移量）
     * @param[in]  data 数据缓冲区，由存储项本身生成并维护
     * @return     操作成功返回true
     */
    virtual bool read(QFile* file, void* data = NULL);

    /*! 
     * 写入当前存储项（从字段树或者存储项本身的缓冲区），不同的存储项的写入方法不同
     * @param[in]  file 当前正在操作的数据文件（已经打开并跳转至合适的偏移量）
     * @param[in]  data 数据缓冲区，由存储项本身生成并维护
     * @return     操作成功返回true
     */
    virtual bool write(QFile* file, void* data = NULL);

    /*! 
     * 根据文件序号（存储项文件夹内的文件编号）生成对应的文件名，默认格式是nnnnn.DAT
     * @param[in]  fileIndex 文件序号（存储项文件夹内的文件编号）
     * @return     操作成功返回文件序号对应的文件名
     */
    virtual QString fileNameFromIndex(int fileIndex) const;

    virtual ItemNameProvider* getItemNameProvider() const
    {
        return NULL;
    }

    /*!
     * 自动存储Item子项每包数据对应item信息
     * @Return:   bool 存储成功返回true
     */
    virtual bool autoSaveItemInfo()
    {
        return false;
    }

    /*! 
     * 载入存储项信息
     * @param[in]  dir  存储项所在的文件夹
     * @param[out] info 存储项信息
     * @return     操作成功返回true
     */
    static bool loadItemInfo(const QString& dir, StorageItemInfo& info);

    /*! 
     * 保存存储项信息
     * @param[in]  dir  存储项所在的文件夹
     * @param[out] info 存储项信息
     * @return     操作成功返回true
     */
    static bool saveItemInfo(const QString& dir, StorageItemInfo info);

#if 0
    StorageItem* clone() const;
#endif

protected:
    /*!
     * saveToChar 将QString中的数据保存的固定长度的数组中
     * @param[in]: srcString 源QString
     * @param[in]: destChar  目的字符数组首地址
     * @param[in]: length  目的数组长度
     * @return:   bool 操作成功返回true
     */
    bool saveToChar(const QString & srcString, char * destChar, int length);

    virtual IconProvider * getIconProvider() const
    {
        return NULL;
    }

public slots :
    /*!
     * 数据模型通知数据更新
     * @param[in]  topLeft     左上角索引
     * @param[in]  bottomRight 右下角索引
     * @param[in]  roles       数据角色
     */
    virtual void onModelDataChanged(
        const QModelIndex &topLeft,
        const QModelIndex &bottomRight,
        const QVector<int> &roles
        )
    {
    
    }

    /*!
     * 监听子存储项存储状态
     */
    virtual void listenSubItemStatus()
    {

    }

protected:
    QAbstractItemModel* m_model;            //!< 当前存储项所对应的数据model
    StorageItem*    m_parentItem;           //!< 当前存储项对应的主存储项
    QString         m_typeName;             //!< 类型名称
    QPixmap         m_icon;                 //!< 当前存储类型对应的图标
    QString         m_path;                 //!< 当前存储项对应的文件夹路径（一个存储项的各个数据块存储在同一个根文件夹内）
    AccessFlags     m_accessFlags;
    StorageStatus   m_status;               //!< 当前存储项的存储状态
    StorageItemInfo m_info;

    QFile*              m_curFile;          //!< 用于执行实际读写操作的文件
    StorageItemPosition m_filePosition;     //!< 读写的当前位置

    void*           m_dataPtr;              //!< 与当前存储项对应的数据指针，仅内部能够解析（测量活动/字段树/图片等）

    bool            m_isAsyncronuous;       //!< 是否异步I/O操作（大文件必须异步）

    QMutex          m_locker;               //线程安全锁

    static StorageItemFactory* s_factory;   //!<用于创建StorageItem的工厂

    static QHash<int, QPixmap> s_typeIcons;//!< 各个不同的存储项类型所对应的图标
};

class IconProvider
{
public:
    IconProvider()
    {

    }

    virtual QPixmap getIconFromType(quint32 subTypeId) const
    {
        return QPixmap();
    }
};
#endif
/*********************************************************************************************************
** End of file
*********************************************************************************************************/
