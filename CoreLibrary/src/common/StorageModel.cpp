/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  StorageModel.cpp
** Latest Version:             V1.0.0
** Latest modified Date:       2017/03/07
** Modified by:                
** Descriptions:               
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 Zhang WenBo
** Created date:               2017/03/07
** Descriptions:               END
** 
*********************************************************************************************************/

#include "StorageModel.h"
#include "IStorage.h"

StorageModel::StorageModel(Storage* storage)
{

}


void StorageModel::reset()
{
    QList<StorageItem *> theItems = Storage::instance()->items();
    QObjectList objects;
    foreach(StorageItem* item, theItems)
    {
        objects.append(item);
    }
    setObjectList(objects);

    return ObjectListModel::reset();
}

/*********************************************************************************************************
** End of file
*********************************************************************************************************/
