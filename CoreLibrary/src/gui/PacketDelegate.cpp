/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  PacketDelegate.cpp 
** Latest Version:             V1.0.0
** Latest modified Date:       2016/1/16
** Modified by:                
** Descriptions:               
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 Zhang Zimo
** Created date:               2016/1/16
** Descriptions:               
** 
*********************************************************************************************************/

#include "PacketDelegate.h"
#include "IField.h"
#include <QPainter>
#include <QDebug>

PacketDelegate::PacketDelegate()
{

}

PacketDelegate::~PacketDelegate()
{

}

/*! 
 * 系统库函数重写，绘制条目
 */
void PacketDelegate::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
    if(index.column() > 0)
        return QStyledItemDelegate::paint(painter, option, index);

    painter->setRenderHint(QPainter::TextAntialiasing);

    //获取显示条目的内容宽度和高度
    int subItemHeight = option.rect.height();
    int subItemWidth = (option.rect.width())/7;

    //获取数据
    QString nameTag = index.data(FieldTextRole).toString();

    IField * field = reinterpret_cast<IField*>(index.internalPointer());
    QString channelMask = field->channelMask();

    int globalDataOffset = field->globalDataOffset();
    QString globalOffset = QObject::tr("%1").arg(globalDataOffset); 

    int localDataOffset = field->localDataOffset();
    QString localOffset = QObject::tr("%1").arg(localDataOffset);

    QString display = index.data(Qt::DisplayRole).toString();

    QString max = index.data(MaxValueTextRole).toString();
    QString min = index.data(MinValueTextRole).toString();

    //绘制条目
    painter->drawText(option.rect.x(), option.rect.y(), 
            subItemWidth, subItemHeight, Qt::AlignVCenter, nameTag);

    painter->drawText(option.rect.x() + subItemWidth, option.rect.y(), 
            subItemWidth, subItemHeight, Qt::AlignVCenter, channelMask);
    
    painter->drawText(option.rect.x() + 2*subItemWidth, option.rect.y(),
            subItemWidth, subItemHeight, Qt::AlignVCenter, globalOffset);

    painter->drawText(option.rect.x() + 3*subItemWidth, option.rect.y(),
            subItemWidth, subItemHeight, Qt::AlignVCenter, localOffset);

    painter->drawText(option.rect.x() + 4*subItemWidth, option.rect.y(),
            subItemWidth, subItemHeight, Qt::AlignVCenter, display);

    painter->drawText(option.rect.x() + 5*subItemWidth, option.rect.y(),
            subItemWidth, subItemHeight, Qt::AlignVCenter, max);

    painter->drawText(option.rect.x() + 6*subItemWidth, option.rect.y(), 
            subItemWidth, subItemHeight, Qt::AlignVCenter, min);

}

/*! 
 * 系统库函数重写，获得所需要绘制条目的尺寸大小
 */
QSize PacketDelegate::sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const
{
    //设置每页显示的字段数量，即Item数量
    int fieldCount = 10;

    QRect rect = option.widget->geometry();
    int itemWidth = rect.width() - 15;
    int y = rect.height() - 12;
    int itemHeight = y / fieldCount;

    return QSize(itemWidth, itemHeight);
}


/*********************************************************************************************************
** End of file
*********************************************************************************************************/
