/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  FieldTreeStorageItem.h 
** Latest Version:             V1.0.0
** Latest modified Date:       2017/03/07
** Modified by:                ZhangWenBo
** Descriptions:               
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 Zhang Wenbo
** Created date:               2015/12/31
** Descriptions:               
** 
*********************************************************************************************************/
#pragma once
#ifndef FIELDTREESTORAGEITEM_H
#define FIELDTREESTORAGEITEM_H

#include "StorageItem.h"


class IField;

/**
*  @class FieldItemInfo
*  @brief Field相关存储项的数据信息集合
*/
class FieldItemInfo
{
public:
    quint32 Size;
    quint32 FrameSize;
    quint32 FrameCount;
    quint32 PointSize;
    quint32 ChannelCount;
    quint32 FlushNumber;
    quint32 Reserved[2];

public:
    FieldItemInfo() :
        Size(32),
        FrameSize(0),
        FrameCount(0),
        PointSize(0),
        ChannelCount(0),
        FlushNumber(0)
    {

    }

    bool isValid() const
    {
        return Size == sizeof(FieldItemInfo);
    }
};

/**
 *  @class FieldTreeStorageItem
 *  @brief 字段树相关同步存储类型
 */
class STORAGELIBRARY_EXPORT FieldTreeStorageItem : public StorageItem
{
    Q_OBJECT

public:
    FieldTreeStorageItem();
    ~FieldTreeStorageItem();

    /*
     * @brief Getter/Setter for m_whiteList
     */
    QStringList whiteList() const
    {
        return m_whiteList;
    }

    void setWhiteList(const QStringList& list)
    {
        m_whiteList = list;
    }

    /*!
     * 设置当前存储项绑定的数据指针
     * @param[in]  data 数据指针
     */
    void setDataPtr(void* data) Q_DECL_OVERRIDE;

    /*!
     *  重新加载指定类型的数据
     * @param[in] startTime 起始时间
     * @param[in] endTime   结束时间
     * @param[in] type      reload存储数据类型
     * @return  操作成功返回true
     */
    bool reload(quint64  startTime = 0, quint64  endTime = 0, int type = 0) Q_DECL_OVERRIDE;


protected:

    /*!
     * setFieldInfo 根据field设置字段Info信息
     * @param[in]: field
     * @return:   void
     */
    void setFieldInfo(IField* field);

    /*!
     * 向当前存储项的文件夹下写入白名单
     * @return     操作成功返回true
     */
    bool saveWhiteList() const;

    /*!
     * 从当前存储项的文件夹下载入白名单
     * @return     操作成功返回true
     */
    bool loadWhiteList();

    /*!
     * 向当前存储项的文件夹下写入FieldItemInfo
     * @return     操作成功返回true
     */
    bool saveFieldInfo() const;

    /*!
     * 从当前存储项的文件夹下载入FieldItemInfo
     * @return     操作成功返回true
     */
    bool loadFieldInfo();

protected:
    QStringList m_whiteList;                //!< 需要读写节点的白名单
    FieldItemInfo m_fieldInfo;              //!< 字段Item数据相关信息集合
};

class STORAGELIBRARY_EXPORT SyncFieldStorageItem : public FieldTreeStorageItem
{
public:

    /*!
     * 保存和加载当前字段树节点到指定path
     * @param[in]: filePath 保存路径
     * @param[in]: fieldXPath 字段树根节点路径
     * @return:   bool 保存或加载成功返回true
     */
    bool saveTree(const QString & filePath, const QString & fieldXPath);
    bool loadTree(const QString & filePath, const QString & fieldXPath);

    bool read(QFile* file, void* data = NULL) Q_DECL_OVERRIDE;
    bool write(QFile* file, void* data = NULL) Q_DECL_OVERRIDE;

};

class STORAGELIBRARY_EXPORT AsyncFieldStorageItem : public FieldTreeStorageItem
{
    Q_OBJECT
public:
    AsyncFieldStorageItem()
        : m_curPosition(m_filePosition)
        , m_dataTypeFlag(0)
        , m_positionChanged(true)
    {
        connect(this, &StorageItem::statusChanged, this, &AsyncFieldStorageItem::onStorageStatusChanged);
    }
    ~AsyncFieldStorageItem()
    {
        disconnect(this, &StorageItem::statusChanged, this, &AsyncFieldStorageItem::onStorageStatusChanged);
    }

    /*!
     *  重新加载指定类型的数据
     * @param[in] startTime 起始时间
     * @param[in] endTime   结束时间
     * @param[in] type      reload存储数据类型
     * @return  操作成功返回true
     */
    bool reload(quint64  startTime = 0, quint64  endTime = 0, int type = 0) Q_DECL_OVERRIDE;


protected:

    /*!
     * 设置当前存储项绑定的数据指针
     * @param[in]  data 数据指针
     */
    void setDataPtr(void* data) Q_DECL_OVERRIDE;

    /*!
     * 自动存储Item子项每包数据保存item信息
     * @Return:   bool 存储成功返回true
     */
    bool autoSaveItemInfo() Q_DECL_OVERRIDE;

    /*!
     * 随时强制保存未保存的文件（尤其是StorageItemInfo和SessionItemInfo），避免意外掉电彻底损坏数据
     * @return     操作成功返回true
     */
    bool flush() Q_DECL_OVERRIDE;

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
    * 获取当前存储项的一帧有多少个字节
    * @return     当前存储项的一帧有多少个字节
    */
    int fileSizeInBytes() const Q_DECL_OVERRIDE;

    /*!
     * @param[in]  startTime 趋势图设置的起始时间
     * @param[out]  startPos   起始时间转换对应的存储位置信息
     * @param[in]  endTime 趋势图设置的结束时间
     * @param[out]  endPos   结束时间转换对应的存储位置信息
     * @return     操作成功返回true
     */
    virtual bool toItemPosition(quint64  startTime, StorageItemPosition & startPos,
            quint64  endTime, StorageItemPosition & endPos);

protected slots:

    void onModelDataChanged(const QModelIndex &topLeft, 
        const QModelIndex &bottomRight, const QVector<int> &roles) Q_DECL_OVERRIDE;

    void onStorageStatusChanged();

protected:
    static float        m_fileCount;        //!< 时间过滤区间读取的文件帧数
    StorageItemPosition m_curPosition;      //!< 当前正在计划读写的位置，不同于基类的m_filePosition（当前文件打开的位置）
    int                 m_dataTypeFlag;     //!< 要读写的数据类型标志
    int                 m_flushDataFlag;    //!< flush数据类型标记
    bool                m_positionChanged;  //!< 用于标记当前计划读写的位置改变标志

};

#endif // FIELDTREESTORAGEITEM_H
/*********************************************************************************************************
** End of file
*********************************************************************************************************/
