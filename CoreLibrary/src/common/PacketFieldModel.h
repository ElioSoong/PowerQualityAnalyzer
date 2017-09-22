/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  PacketFieldModel.h 
** Latest Version:             V1.0.0
** Latest modified Date:       2016/1/15
** Modified by:                
** Descriptions:               
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 Zhang Zimo
** Created date:               2016/1/15
** Descriptions:               
** 
*********************************************************************************************************/
#pragma once

#include "corelibrary_global.h"
#include "FieldModel.h"
#include <QModelIndex>

class IField;

class CORELIBRARY_EXPORT PacketFieldModel : public FieldModel
{
public:

    PacketFieldModel(IField * rootField = NULL, const QString& rootPath = "");
    ~PacketFieldModel();


    /*! 
     * 根据指定字段的XPath来生成QModelIndex
     */
    QModelIndex index(const QString & fieldPath) const;


    /*! 
     * 参见Qt manual
     */
    QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const Q_DECL_OVERRIDE;
    
};


/*********************************************************************************************************
** End of file
*********************************************************************************************************/