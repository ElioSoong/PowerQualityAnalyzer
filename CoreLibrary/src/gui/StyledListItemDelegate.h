/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  StyledListItemDelegate.h
** Latest Version:             V1.0.0
** Latest modified Date:       2015/06/25
** Modified by:                
** Descriptions:               
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 Han ZhenFei
** Created date:               2015/06/25
** Descriptions:               
** 
*********************************************************************************************************/
#pragma once

#include "corelibrary_global.h"
#include <QtWidgets/QStyledItemDelegate>

class CORELIBRARY_EXPORT StyledListItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT;
    Q_PROPERTY(int itemCountPerPage MEMBER m_itemCountPerPage READ itemCountPerPage)

public:
    StyledListItemDelegate()
    {

    }

    StyledListItemDelegate(const StyledListItemDelegate& other)
    {

    }
public:
    /*! 
     * 系统库函数重写，获得所需要绘制数据的尺寸大小
     */
 //   QSize sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const Q_DECL_OVERRIDE;

    /*! 
     * 获取当前节点下孩子的数量
     * @return 返回孩子数量
     */
    virtual  int childCount(const QModelIndex &index);

    /*! 
     * 获得指定节点的名称，默认实现返回字段名称
     */
    virtual  QString ItemName(const QModelIndex &index);

    /*! 
     * 默认实现为，指定item名称，来获取该名称下字段的数量
     * @return 返回当前列的数量
     */
 //   virtual  int itemColumnHint(const QString &name);

    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;

    int itemCountPerPage() const;
private:
    int m_itemCountPerPage;                             //!< 每页有多少个项目
};





/*********************************************************************************************************
** End of file
*********************************************************************************************************/