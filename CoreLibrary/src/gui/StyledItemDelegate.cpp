/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  StyledItemDelegate.h
** Latest Version:             V1.0.0
** Latest modified Date:       2015/08/24
** Modified by:                
** Descriptions:               
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 Chen Honghao
** Created date:               2015/08/24
** Descriptions:               对系统类QStyledItemDelegate进行简单封装，提供易用的XML接口
** 
*********************************************************************************************************/
#include "includes.h"
#include "gui/StyledItemDelegate.h"

/*! 
 * 系统库函数重写，获得所需要绘制数据的尺寸大小
 */
QSize StyledItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // 获取视图本身大小
    const QWidget* widget = option.widget;
    QRect rect = widget->geometry();
    // 计算每一Item的高度
    int height =rect.height() / m_rowCountPerPage;
    // 计算放大后的宽度大小
    int width = rect.width() / m_columnCountPerPage;

    return QSize(width, height);
}
/*********************************************************************************************************
** End of file
*********************************************************************************************************/