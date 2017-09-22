/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  SysStatusModel.h 
** Latest Version:             V1.0.0
** Latest modified Date:       2016/03/25
** Modified by:                
** Descriptions:               
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 Zhang Wenbo
** Created date:               2016/03/25
** Descriptions:            用於定時檢測系統狀態的Model
** 
*********************************************************************************************************/

#include "common/SysStatusModel.h"

SysStatusModel* SysStatusModel::s_sysModel = NULL;
SysStatusModel* SysStatusModel::instance()
{
    if (s_sysModel == NULL)
    {
        s_sysModel = new SysStatusModel;
    }
    return s_sysModel;
}

/*********************************************************************************************************
** End of file
*********************************************************************************************************/