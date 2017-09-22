/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  PacketFieldModel.cpp 
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


#include "PacketFieldModel.h"
#include "IField.h"


PacketFieldModel::PacketFieldModel(IField * rootField/* = NULL*/, const QString& rootPath/* = ""*/)
    : FieldModel(rootField, rootPath)
{

}


PacketFieldModel::~PacketFieldModel()
{

}


/*! 
 * 根据指定字段的XPath来生成QModelIndex
 */
QModelIndex PacketFieldModel::index(const QString& fieldPath) const
{
    IField* field = IField::at(fieldPath, m_rootField);
    if (field == NULL)
        return QModelIndex();

    // TODO: 1010 此处如果createIndex(-1, -1, field)，则将导致创建的QModelIndex无效
    return createIndex(0x7FFFFFFF, 0x7FFFFFFF, field);
}


/*! 
 *  参见Qt manual
 */
QModelIndex	PacketFieldModel::index(int row, int column, const QModelIndex & parent/* = QModelIndex()*/) const
{

    if (!m_rootField || row < 0 || column < 0) 
        return QModelIndex(); 

    IField *p = parent.isValid() ? reinterpret_cast<IField*>(parent.internalPointer()) : m_rootField;
    if (p)
    {

        IField *child = p->child(row);
        if (child)
        {
            return createIndex(row, 0, child);
        }
    }

    return QModelIndex();
}


/*********************************************************************************************************
** End of file
*********************************************************************************************************/