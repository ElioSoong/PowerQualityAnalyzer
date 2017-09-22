/****************************************Copyright (c)****************************************************
**
**                             INNO Instrument(China) Co.Ltd.
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  MetaSystemHelper.cpp
** Latest Version:             V1.0.0
** Latest modified Date:       2017/04/28
** Modified by:                
** Descriptions:               
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 Chen Honghao
** Created date:               2017/04/28
** Descriptions:               MetaSystemHelper的默认实现，注册CoreLibrary中的公共对象类型
** 
*********************************************************************************************************/
#include "includes.h"

#include <QObject>
#include <QString>
#include <QDebug>
#include <QMetaType>
#include <QStack>
#include <QtWidgets/QWidget>

#include "MetaSystemHelper.h"
#include "common/Plugin.h"
#include "common/ActivityManager.h"
#include "common/Activity.h"
#include "common/ActivityModel.h"
#include "common/StorageModel.h"

#include "FieldLibrary/IField.h"
#include "FieldLibrary/VirtualField.h"
#include "FieldLibrary/DataField.h"
#include "FieldLibrary/DataField_p.h"
#include "FieldLibrary/RealField.h"
#include "FieldLibrary/IntegerField.h"
#include "FieldLibrary/EnumField.h"
#include "FieldLibrary/FractionField.h"
#include "FieldLibrary/StringField.h"
#include "FieldLibrary/FileField.h"
#include "FieldLibrary/FieldProxyModel.h"
#include "FieldLibrary/FieldModel.h"
//#include "common/Activity.h"

bool MetaSystemHelper::registerAllTypes(JsonLoader& loader)
{  
    // Basic types
    REGISTER_METATYPE(PluginList);

    REGISTER_METATYPE(ActivityManager);
    REGISTER_METATYPE(Activity);
    REGISTER_METATYPE(ActivityModel);
    REGISTER_METATYPE(View);
    REGISTER_METATYPE(StorageItem);
    REGISTER_METATYPE(ConfigItem);
    REGISTER_METATYPE(DataItem);
    REGISTER_METATYPE(MenuItem);
    REGISTER_METATYPE(EnumItem);
    REGISTER_METATYPE(FractionItem);

    // Models
    REGISTER_METATYPE(FieldProxyModel);
    REGISTER_METATYPE(FieldModel);

    // Fields
    REGISTER_METATYPE(IField);
    REGISTER_METATYPE(VirtualField);
    REGISTER_METATYPE(ReservedField);
    REGISTER_METATYPE(SymlinkField);
    REGISTER_METATYPE(BoolField);
    REGISTER_METATYPE(IntegerField);
    REGISTER_METATYPE(LongIntegerField);
    REGISTER_METATYPE(StringField);
    REGISTER_METATYPE(RealField);
    REGISTER_METATYPE(FractionField);
    REGISTER_METATYPE(EnumField);
    REGISTER_METATYPE(ScalingField);
    REGISTER_METATYPE(DirectStructField);
    REGISTER_METATYPE(IndirectStructField);
    REGISTER_METATYPE(ArrayField);
    REGISTER_METATYPE(TextFileField);

    // Views

    // Delegates

    // Lists
    REGISTER_GENERIC_LIST_PARSER(PluginList*, loader);
    REGISTER_GENERIC_LIST_PARSER(Activity*, loader);
    REGISTER_GENERIC_LIST_PARSER(View*, loader);
    REGISTER_GENERIC_LIST_PARSER(StorageItem*, loader);
    REGISTER_GENERIC_LIST_PARSER(bool, loader);
    REGISTER_GENERIC_LIST_PARSER(int, loader);
    REGISTER_GENERIC_LIST_PARSER(double, loader);
    REGISTER_GENERIC_LIST_PARSER(float, loader);
    REGISTER_GENERIC_LIST_PARSER(QString, loader);
    //REGISTER_GENERIC_LIST_PARSER(QObject*, loader);
    REGISTER_GENERIC_LIST_PARSER(IField*, loader);
    REGISTER_GENERIC_LIST_PARSER(EnumItem*, loader);
    REGISTER_GENERIC_LIST_PARSER(MenuItem*, loader);
    REGISTER_GENERIC_LIST_PARSER(ConfigItem*, loader);
    REGISTER_GENERIC_LIST_PARSER(DataItem*, loader);

    return true;
}

/*********************************************************************************************************
** End of file
*********************************************************************************************************/
