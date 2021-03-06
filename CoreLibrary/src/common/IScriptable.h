﻿/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  IScriptable.h
** Latest Version:             V1.0.0
** Latest modified Date:       2016/5/25
** Modified by:                
** Descriptions:               
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 
** Created date:               2016/5/25
** Descriptions:               IScriptable class  [Generated by VP-UML, IGT1.1]
** 
*********************************************************************************************************/
#ifndef __ISCRIPTABLE_H__
#define __ISCRIPTABLE_H__


#include "corelibrary_global.h"

class CORELIBRARY_EXPORT IScriptable 
{

public:
    /**
     * Constructor
     */
    IScriptable();

public: 
    /**
     * 注册当前可在脚本中使用的插件的全部类，从而保证Qt可以通过这些类的名称创建对象
     * @param[in] context    上下文，通常为一个JsonLoader对象，也可以为NULL
     * @return    全部注册成功返回true，否则返回false
     */
    virtual bool registerAllTypes(void* context = NULL) const = 0;

    /**
     * 获取当前可在脚本中使用的插件的全部类的名称（一个插件可能对应多个类）
     * @return       QStringList
     */
    virtual QStringList typeNames() = 0;

    /**
     * 
     * @param[in] typeName    
     * @return       QObject
     */
    static QObject* newInstance(const QString& typeName);

    /**
     * 
     * @param[in] instance    
     * @return       bool
     */
    static bool deleteInstance(QObject* instance);


};

Q_DECLARE_INTERFACE(IScriptable, "com.inno.IScriptable")


#endif
/*********************************************************************************************************
** End of file
*********************************************************************************************************/