/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  SessionStorageItem.h
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
#pragma once
#ifndef SESSIONSTORAGEITEM_H
#define SESSIONSTORAGEITEM_H

#include "storagelibrary_global.h"
#include "storage/StorageItem.h"
#include "common/Activity.h"

#define SESSIONINFO_PATH    "/SESSION.INFO"

class STORAGELIBRARY_EXPORT SessionInfo
{
public:
    quint32  Magic;                             //!< SessionInfo的Magic，用于校验有效性
    quint32  Revision;                          //!< 存储格式的版本号，用于载入多版本的存储文件
    quint32  Size;                              //!< sizeof(SessionInfo)
    quint32  Checksum;                          //!< SessionInfo的校验和
    quint32  SessionType;                       //!< 测量模式
    quint32  SessionReserved[2];                //!< 保留字段，用于扩展
    quint32  StorageFlags;                      //!< 子存储项标志位

    quint64  StartTime;                         //!< 开始测量的UTC时间
    quint64  EndTime;                           //!< 结束测量的UTC时间

    quint32  TimeReserved[4];

public:
    SessionInfo() : 
        Magic(0x5341454d),
        Revision(0x01000001),
        Size(64),
        Checksum(0),
        SessionType(0),
        StorageFlags(0),
        StartTime(0),
        EndTime(0)
    {

    }

    bool isValid() const
    {
        return Magic == 0x5341454d && Revision == 0x01000001 && Size == sizeof(SessionInfo);
    }
};

/**
 *  @class SessionStorageItem
 *  @brief Session存储类型
 */
class STORAGELIBRARY_EXPORT SessionStorageItem : public StorageItem
{
    Q_OBJECT

public:
    SessionStorageItem();
    ~SessionStorageItem();

public:

    /*! 
     * 设置当前存储项的Session类型(m_sessionInfo.sessionType)
     * @param[in]  type 标识类型的数据
     */
    void setSessionType(int type)
    {
        m_sessionInfo.SessionType = type;
    };

     /*! 
     * 返回当前测量存储项的测量信息
     */
    SessionInfo sessionInfo()
    {
        return m_sessionInfo;
    }

    /*!
     * 自动存储Item子项每包数据保存的对应session和item信息
     * @Return:   bool 存储成功返回true
     */
    bool autoSaveItemInfo() Q_DECL_OVERRIDE;

    /*!
     * flushItemInfo 恢复自动存储标记并存储ItemInfo
     * @return:   bool
     */
    bool flushItemInfo();

    /*! 
     * 载入会话信息
     * @param[in]  dir  存储项所在的文件夹
     * @param[out] info 存储项信息
     * @return     操作成功返回true
     */
    static bool loadSessionInfo(const QString& dir, SessionInfo& info);

    /*! 
     * 保存会话信息
     * @param[in]  dir  存储项所在的文件夹
     * @param[out] info 存储项信息
     * @return     操作成功返回true
     */
    static bool saveSessionInfo(const QString& dir, SessionInfo info);

public slots:

    void listenSubItemStatus();

protected:
    /*! 
     * 设置当前存储项绑定的数据指针
     * @param[in]  data 数据指针
     */
    void setDataPtr(void* data) Q_DECL_OVERRIDE;

    /*! 
     * 获取当前存储项的类型
     * @return     当前存储项的类型
     */
    quint32 subType()  Q_DECL_OVERRIDE;

    /*! 
     * 保存存储项目数据
     */
    bool save() Q_DECL_OVERRIDE;

    /*! 
     * 载入指定位置的存储项目数据
     */
    bool load() Q_DECL_OVERRIDE;

    /*!
     *  重新加载指定类型的数据
     * @param[in] startTime 起始时间
     * @param[in] endTime   结束时间
     * @param[in] type      reload存储数据类型
     * @return  操作成功返回true
     */
    bool reload(quint64  startTime = 0, quint64  endTime = 0, int type = 0) Q_DECL_OVERRIDE;

    /*! 
     * 读取当前存储项（到字段树或者存储项本身的缓冲区），不同的存储项的读取方法不同
     * @param[in]  file 当前正在操作的数据文件（已经打开并跳转至合适的偏移量）
     * @param[in]  data 数据缓冲区，由存储项本身生成并维护
     * @return     操作成功返回true
     */
    bool read(QFile* file, void* data = NULL) Q_DECL_OVERRIDE;

    /*! 
     * 写入当前存储项（从字段树或者存储项本身的缓冲区），不同的存储项的写入方法不同
     * @param[in]  file 当前正在操作的数据文件（已经打开并跳转至合适的偏移量）
     * @param[in]  data 数据缓冲区，由存储项本身生成并维护
     * @return     操作成功返回true
     */
    bool write(QFile* file, void* data = NULL) Q_DECL_OVERRIDE;

    /*! 
     * 随时强制保存未保存的文件（尤其是StorageItemInfo和SessionItemInfo），避免意外掉电彻底损坏数据
     * @return     操作成功返回true
     */
    bool flush() Q_DECL_OVERRIDE;

    /**
     * 关闭并保存当前存储项
     * 处于SaveMode模式时，close前保存截图
     */
    void close() Q_DECL_OVERRIDE;

    /*! 
     * 设置当前存储项的路径（其实是文件夹名称，因为所有的存储项均存储在同一层次，每个存储项为一个文件夹）
     * @param[in]  path 当前存储项的路径
     */
    bool setPath(const QString& path) Q_DECL_OVERRIDE;

    /*!
     * getActivity 根据sessionType返回Activity指针
     * @param[in]: sessionType 测量活动类型
     * @return:   Activity*   测量活动指针
     */
    virtual Activity* getActivity(int sessionType)
    {
        return NULL;
    }

protected:
    /*! 
     * 移除全部的子存储项目
     */
    void removeAllSubItems();

    /*! 
     * 初始化子存储项目
     */
    virtual void initializeSubItems(Activity* activity = NULL);

protected:
    SessionInfo m_sessionInfo;              //!< 当前会话存储项目的信息

    StorageItem*        m_setupStorageItem; //!< 配置存储项标志(每个测量会话均有配置)

    QList<StorageItem*> m_subItemList;      //!< 会话项目包含的实际子项目列表（趋势、波形等）
};

#endif // SESSIONSTORAGEITEM_H
/*********************************************************************************************************
** End of file
*********************************************************************************************************/

