/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  PacketDelegate.h 
** Latest Version:             V1.0.0
** Latest modified Date:       2016/1/15
** Modified by:                
** Descriptions:               
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 Zhang Zimo
** Created date:               2016/1/15
** Descriptions:               packet视图的条目代理 
** 
*********************************************************************************************************/
#pragma once

#include "corelibrary_global.h"
#include <QStyledItemDelegate>

class CORELIBRARY_EXPORT PacketDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:

    PacketDelegate();
    ~PacketDelegate();

    /*! 
     * 系统库函数重写，绘制条目
     */
    void paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const Q_DECL_OVERRIDE;

    /*! 
     * 系统库函数重写，获得所需要绘制条目的尺寸大小
     */
    QSize sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const Q_DECL_OVERRIDE;

};


/*********************************************************************************************************
** End of file
*********************************************************************************************************/
