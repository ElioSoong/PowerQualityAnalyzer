/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  IStorage.h
** Latest Version:             V1.0.0
** Latest modified Date:       2017/03/07
** Modified by:                ZhangWenBo
** Descriptions:               将IStorage类重命名为Storage，以避免与Windows的COM库对象重名问题
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 Chen Honghao
** Created date:               2015/11/25
** Descriptions:               存储接口，通常作为存储操作对应用程序的唯一接口
** 
*********************************************************************************************************/
#ifndef ISTORAGE_H
#define ISTORAGE_H

#include "storagelibrary_global.h"
#include "storage/StorageItem.h"
#include "storage/StorageThread.h"
#include "common/StorageModel.h"
#include "common/Activity.h"

#include <QObject>
#include <QList>


class STORAGELIBRARY_EXPORT StorageFilter
{
public:
    quint64             StartTime;                                    //!< 当前的数据过滤起始时间（主要用于趋势图过滤）
    quint64             EndTime;                                      //!< 当前的数据过滤结束时间（主要用于趋势图过滤）
    quint32             CurPage;                                      //!< 当前页
    quint32             TotalPage;                                    //!< 总页数

public:
    StorageFilter()
        : StartTime(0)
        , EndTime(0)
        , CurPage(0)
        , TotalPage(0)
    {

    }
};

/**
 *  @class IStorage
 *  @brief 存储接口，通常作为存储操作对应用程序的唯一接口
 */
class STORAGELIBRARY_EXPORT Storage : public QObject
{
    Q_OBJECT

public:
    Storage();
    ~Storage();

    /**
     *  @enum  StorageMode
     *  @brief 存储模式，测量活动正在进行时为SaveMode，载入历史保存的文件为LoadMode
     */
    enum StorageMode
    {
        IdleMode = 0,                       //!< 空闲模式
        SaveMode,                           //!< 保存模式
        MeasureMode = SaveMode,             //!< 测量模式
        RunningMode = SaveMode,             //!< 运行模式
                                            
        LoadMode,                           //!< 载入模式
        PlaybackMode = LoadMode,            //!< 回放模式
    };


public:

    /*! 
     * 枚举全部存储项，并初步获取存储项的类型、时间、图标等信息，但并不加载存储项
     * @param[in]  rootPath   指定的存储根目录
     * @param[in]  typeFilter 存储项类型过滤器，若指定了合法类型，则只枚举该类型
     * @return     枚举到的存储项个数，枚举到的存储项列表将缓存在存储接口内部
     */
    int enumerate(const QString& rootPath = QString(), int typeFilter = 0);

    /*! 
     * 排序全部存储项，默认按照时间从新到旧排序
     * @param[in]  column     提供排序依据的列（对应存储视图：0=Date,1=Time,2=Description,3=Type）
     * @param[in]  descending 是否降序排列，精确地说是反序排列：日期和时间从旧到新，描述从小到大，类型从高到低
     */
    void sortAllItems(int column = 0, bool descending = false);

    /*! 
     * 获取已经枚举到的存储项个数
     * @return     已经枚举到的存储项个数
     */
    int itemCount() const;
    
    /*! 
     * 获取指定位置的存储项对象
     * @param[in]  index 存储项序号
     * @return     存储项对象
     */
    StorageItem* itemAt(int index);

    /*! 
     * 获取当前的存储模式
     * @return     当前的存储模式
     */
    inline StorageMode currentMode() const
    {
        return m_curStorageMode;
    }

    /*! 
     * 获取当前载入的或者正在保存存储项目（根据存储模式确定）
     * @return     当前载入的或者正在保存存储项目
     */
    StorageItem* currentItem() const
    {
        return m_curStorageItem;
    }

    QList<StorageItem*> items() const
    {
        return m_storageItemList;
    }

    /*! 
     * 新建一个指定类型的存储项（存储项的工厂）
     * @param[in]  type 存储项的类型
     * @return     创建的存储项
     * @note       该函数将创建对应的对象（构造函数中可能绑定字段树或图片），并为存储项自动命名
     */
    StorageItem* createItem(int type, void* data = NULL);

    /*! 
     * 保存指定位置的存储项目，并将该项目加入存储项列表，从而显示在存储视图
     * @param[in]  item 指定的存储项目
     * @return     操作成功返回true
     */
    virtual bool save(StorageItem* item);

    /*! 
     * 载入指定位置的存储项目
     * @param[in]  item 指定的存储项目
     * @return     操作成功返回true
     */
    virtual bool load(StorageItem* item);

    /*!
    *  重新加载指定类型的数据
    * @param[in] startTime 起始时间
    * @param[in] endTime   结束时间
    * @param[in] type      reload存储数据类型
    * @return  操作成功返回true
    */
    virtual bool reload(quint64  startTime = 0, quint64  endTime = 0, int type = 0);

    /*! 
     * 重命名指定的存储项目到指定位置
     * @param[in]  item    存储项目
     * @param[in]  newPath 新的保存位置
     * @return     操作成功返回true
     */
    virtual bool rename(StorageItem* item, const QString& newPath);

    /**
     * 删除指定的存储项目，此处命名为remove只是为了避开C++的delete关键字
     * @param[in]  item 指定的存储项目
     * @return     操作成功返回true
     */
    virtual bool remove(StorageItem* item);

    /**
     * 删除所有存储项目，此处命名为remove只是为了避开C++的delete关键字
     * @return     操作成功返回true
     */
    bool removeAll();

    /**
     * 添加指定的存储项目
     * @param[in]  item 指定的存储项目
     * @return     操作成功返回true
     */
    bool addItem(StorageItem* item) 
    {
        return insertItem(item, 0);
    };

    /*! 
     * 获取存储线程对象，仅用于StorageItem基类调用
     * @return     存储线程对象
     */
    StorageThread* storageThread()
    {
        return m_storageThread;
    }

    /*!
     * 获取当前活动的（最后创建的）存储接口实例
     * @return     当前活动的（最后创建的）存储接口实例
     */
    static Storage* instance()
    {
        return s_instance;
    }

    QAbstractItemModel* storageModel()
    {
        return m_storageModel;
    }

    StorageFilter getStorageFilter() const
    {
        return m_storageFilter;
    }

    /*!
    *  返回数据过滤的起始时间
    */
    inline quint64 startTime() const
    {
        return m_storageFilter.StartTime;
    }

    inline void setStartTime(quint64 time)
    {
        m_storageFilter.StartTime = time;
    }

    /*!
    *  返回数据过滤的结束时间
    */
    inline quint64 endTime() const
    {
        return m_storageFilter.EndTime;
    }

    inline void setEndTime(quint64 time)
    {
        m_storageFilter.EndTime = time;
    }

    inline void setCurPage(quint32 curPage)
    {
        m_storageFilter.CurPage = curPage;
    }

    inline void setTotalPage(quint32 totalPage)
    {
        m_storageFilter.TotalPage = totalPage;
    }

    /*! 
     * 设置存储模式
     * @param[in]  mode 存储模式
     * @return     操作成功返回true
     */
    bool setMode(StorageMode mode);

    /*!
    * 判断当前磁盘空间是否可用
    */
    static bool availableDiskSpace();

    /*!
    * doRename 对应到文件夹的实际文件重命名
    * @param[in]: item
    * @Return:   bool 操作成功返回true
    */
    bool doRename(StorageItem* item);

signals:

    void diskFull();
    void readRatio(int ratio);
    void itemToRemove(StorageItem* item);

    void itemSaving(StorageItem* item);
    void itemSaved(StorageItem* item);
    void itemLoading(StorageItem* item);
    void itemLoaded(StorageItem* item);

protected:


    /*! 
     * 内部调用的帮助函数，向存储项队列中添加一个新项目，并刷新StorageModel
     * @param[in]  item  存储项
     * @param[in]  index 添加位置，0代表起始项目的前面，-1代表末尾项目的后面，itemCount-1代表末尾项目的前面
     * @return     操作成功返回true
     */
    bool insertItem(StorageItem* item, int index = 0);

    /*! 
     * 内部调用的帮助函数，从存储项队列中移除一个新项目，并刷新StorageModel
     * @param[in]  item  存储项
     * @return     操作成功返回true
     */
    bool removeItem(StorageItem* item);

    /*! 
     * 内部调用的帮助函数，从存储项队列中移除全部项目，并刷新StorageModel
     */
    void removeAllItems();

    virtual int toStorageType(Activity* activity)
    {
        Q_UNUSED(activity);
        return 0;
    }

public slots:

    /*!
     * 启动了一个新的测量活动（包括Restart）
     * @param[in]  activity 测量活动
     */
    void onActivityStarted(Activity* activity);
    
    /*!
     * 结束了一个新的测量活动（包括Restart）
     * @param[in]  activity 测量活动
     */
    void onActivityStopped(Activity* activity);


    bool doRemoveItem(StorageItem* item);
    /**
    *  @brief 以下槽主要用于远程通信调用服务
    */

    /*! 
     * Helper function for load
     */
    bool load(int index)
    {
        return load(itemAt(index));
    }

    /*! 
     * Helper function for rename
     */
    bool rename(int index, const QString& newPath)
    {
        return rename(itemAt(index), newPath);
    }

    /*! 
     * Helper function for remove
     */
    bool remove(int index)
    {
        return remove(itemAt(index));
    }

    /*! 
     * Helper function for save
     */
    bool save(int index)
    {
        return save(itemAt(index));
    }

    /*! 
     * Helper function for ItemInfo
     */
    const QString itemInfo(int index)
    {
        return itemAt(index)->path();
    }


protected:

    StorageFilter       m_storageFilter;                                //!< 存储过滤器
    StorageItem*        m_curStorageItem;                               //!< 当前打开的存储文件
    StorageThread*      m_storageThread;                                //!< 后台存储线程
    StorageModel*       m_storageModel;                                 //!< 存储数据模型
    QList<StorageItem*> m_storageItemList;                              //!< 已经枚举到的存储项目列表（内部自动维护，重新枚举时会清空）
    StorageMode         m_curStorageMode;                               //!< 当前测量模式
    QString             m_rootPath;                                     //!< 当前存储接口实例对应的存储根目录
    static Storage*     s_instance;                                     //!< 当前活动的（最后创建的）存储接口实例
};
#endif
/*********************************************************************************************************
** End of file
*********************************************************************************************************/

