/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  .cpp
** Latest Version:             V1.0.0
** Latest modified Date:       2015/06/25
** Modified by:                
** Descriptions:               测试用例生成器的主界面
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 Han ZhenFei
** Created date:               2015/06/25
** Descriptions:               
** 
*********************************************************************************************************/
#include "StyledListItemDelegate.h"
#include "IField.h"
int StyledListItemDelegate::childCount(const QModelIndex &index)
{
    return 0;
}

QString StyledListItemDelegate::ItemName(const QModelIndex &index)
{
    return "";
}


int StyledListItemDelegate::itemCountPerPage() const
{
  return m_itemCountPerPage;
}

QSize StyledListItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // 获取视图本身大小
    QRect rect =option.rect;
    // 获得每一页显示几个
    int num = this->itemCountPerPage();
    // 计算每一Item的高度
    int height =rect.height() / num;
    // 计算放大后的宽度大小
    int width = rect.width();

    return QSize(width, height);
}
/*********************************************************************************************************
** End of file
*********************************************************************************************************/
