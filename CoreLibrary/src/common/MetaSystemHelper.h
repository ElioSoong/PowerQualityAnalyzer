/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  MetaSystemHelper.h
** Latest Version:             V1.0.0
** Latest modified Date:       2015/06/30
** Modified by:                
** Descriptions:               根据类名，动态生成对象
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 Han ZhenFei
** Created date:               2015/06/30
** Descriptions:               
** 
*********************************************************************************************************/
#pragma once

#include "corelibrary_global.h"
#include "JsonLoader/JsonLoader.h"

/**
*  @class MetaSystemHelper
*  @brief 用于注册各个元类型的项目相关文件，可直接在此文件内追加任意需要注册的类
*/
class CORELIBRARY_EXPORT MetaSystemHelper
{

public:
    /*! 
    * 注册类型,需要单独放在一个函数里
    * @param[in]
    */
    virtual bool registerAllTypes(JsonLoader& loader);
};
