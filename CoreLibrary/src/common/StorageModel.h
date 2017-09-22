/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  StorageModel.h
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
#pragma once

#include "storagelibrary_global.h"
#include "ObjectListModel.h"

class Storage;

class STORAGELIBRARY_EXPORT StorageModel : public ObjectListModel
{
public:
    StorageModel(Storage* storage = NULL);

    void reset() Q_DECL_OVERRIDE;
};

/*********************************************************************************************************
** End of file
*********************************************************************************************************/
