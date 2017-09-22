/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  SetupStorageItem.h
** Latest Version:             V1.0.0
** Latest modified Date:       2017/03/07
** Modified by:                ZhangWenBo
** Descriptions:               
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 Chen Honghao
** Created date:               2015/12/01
** Descriptions:               Setup存储类型
** 
*********************************************************************************************************/
#ifndef SETUPSTORAGEITEM_H
#define SETUPSTORAGEITEM_H

#include "storagelibrary_global.h"
#include "FieldTreeStorageItem.h"

#define SYS_SETUP_XPATH         "/sys/setup"

/**
 *  @class SetupStorageItem
 *  @brief Setup存储类型
 */
class STORAGELIBRARY_EXPORT SetupStorageItem : public SyncFieldStorageItem
{
    Q_OBJECT

public:

    /*!
     * instance  返回配置存储项指针,方便全局配置项的保存
     * @return:   SetupStorageItem*
     */
    static SetupStorageItem* instance();

    static QString setupXPath()
    {
        return SYS_SETUP_XPATH;
    }

    SetupStorageItem();

protected:
    static SetupStorageItem* s_instance;
};

#endif // SETUPSTORAGEITEM_H
/*********************************************************************************************************
** End of file
*********************************************************************************************************/

