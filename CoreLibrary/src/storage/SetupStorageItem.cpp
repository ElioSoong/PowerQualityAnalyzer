/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  SetupStorageItem.cpp
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
#include "SetupStorageItem.h"
#include "IField.h"

SetupStorageItem::SetupStorageItem()
    : SyncFieldStorageItem()
{

}

SetupStorageItem* SetupStorageItem::s_instance = NULL;
SetupStorageItem* SetupStorageItem::instance()
{
    if (!s_instance)
    {
        s_instance = new SetupStorageItem;
    }
    return s_instance;
}

/*********************************************************************************************************
** End of file
*********************************************************************************************************/

