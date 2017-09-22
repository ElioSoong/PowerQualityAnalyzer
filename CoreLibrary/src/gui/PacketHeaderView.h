/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  PacketHeaderView.h 
** Latest Version:             V1.0.0
** Latest modified Date:       2016/1/15
** Modified by:                
** Descriptions:               
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 Zhang Zimo
** Created date:               2016/1/15
** Descriptions:               PacketView的表头
** 
*********************************************************************************************************/
#pragma once

#include "corelibrary_global.h"
#include <QHeaderView>
#include <QPainter>
#include <QRect>

class CORELIBRARY_EXPORT PacketHeaderView : public QHeaderView
{
public:

    PacketHeaderView(QWidget * parent = 0);
    ~PacketHeaderView();

    /*! 
     * 绘制表头
     */
    void paintSection(QPainter * painter, const QRect & rect, int logicalIndex) const;
};


/*********************************************************************************************************
** End of file
*********************************************************************************************************/