/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  TrendStorageItem.h
** Latest Version:             V1.0.0
** Latest modified Date:       2015/12/01
** Modified by:                
** Descriptions:               
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 Chen Honghao
** Created date:               2015/12/01
** Descriptions:               Trend存储类型
** 
*********************************************************************************************************/
#ifndef TRENDSTORAGEITEM_H
#define TRENDSTORAGEITEM_H

#include "storage/FieldTreeStorageItem.h"


/**
 *  @class TrendStorageItem
 *  @brief Trend存储类型
 */
class STORAGELIBRARY_EXPORT TrendStorageItem : public AsyncFieldStorageItem
{
    Q_OBJECT

public:
    TrendStorageItem();
    ~TrendStorageItem();

    /*! 
     *  时间过滤区间读取的文件帧数
     */
    static float fileCount() 
    {
        return m_fileCount;
    }

    /*! 
     * 保存趋势图缩略图文件
     * @return     操作成功返回true
     */
    bool saveThumbnail() const;

    /*! 
     * 从当前存储项的文件夹下载入趋势信息，缩略图信息
     * @return     操作成功返回true
     */
    bool loadThumbnail();

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
     * 自动存储Item子项每包数据保存item信息
     * @Return:   bool 存储成功返回true
     */
    bool autoSaveItemInfo() Q_DECL_OVERRIDE;

    /*!
     * 随时强制保存未保存的文件（尤其是StorageItemInfo和SessionItemInfo），避免意外掉电彻底损坏数据
     * @return     操作成功返回true
     */
    bool flush() Q_DECL_OVERRIDE;

};

#endif // TRENDSTORAGEITEM_H
/*********************************************************************************************************
** End of file
*********************************************************************************************************/

