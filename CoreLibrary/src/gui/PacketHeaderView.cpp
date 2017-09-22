/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  PacketHeaderView.cpp 
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

#include "PacketHeaderView.h"

PacketHeaderView::PacketHeaderView(QWidget * parent)
    : QHeaderView(Qt::Horizontal, parent)
{

}

PacketHeaderView::~PacketHeaderView()
{

}

/*! 
 * 绘制表头
 */
void PacketHeaderView::paintSection(QPainter * painter, const QRect & rect, int logicalIndex) const
{
    //绘制表头
    painter->drawText(rect.x(), rect.y(), rect.width()/7,
            rect.height(), Qt::AlignVCenter, "name");

    painter->drawText(rect.x() + (rect.width()/7), rect.y(), 
            rect.width()/7, rect.height(), Qt::AlignVCenter, "channelmask"); 

    painter->drawText(rect.x() + ((2*rect.width())/7), rect.y(), 
            rect.width()/7, rect.height(), Qt::AlignVCenter, "globaloffset");

    painter->drawText(rect.x() + ((3*rect.width())/7), rect.y(), 
            rect.width()/7, rect.height(), Qt::AlignVCenter, "localoffset");

    painter->drawText(rect.x() + ((4*rect.width())/7), rect.y(), 
            rect.width()/7, rect.height(), Qt::AlignVCenter, "display"); 

    painter->drawText(rect.x() + ((5*rect.width())/7), rect.y(), 
            rect.width()/7, rect.height(), Qt::AlignVCenter, "max");

    painter->drawText(rect.x() + ((6*rect.width())/7), rect.y(), 
            rect.width()/7, rect.height(), Qt::AlignVCenter, "min"); 
}


/*********************************************************************************************************
** End of file
*********************************************************************************************************/