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
#pragma once

#include "corelibrary_global.h"
#include <QtWidgets/QStyledItemDelegate>

/**
 *  @class StyledItemDelegate
 *  @brief 对系统类QStyledItemDelegate进行简单封装，提供易用的XML接口
 */
class CORELIBRARY_EXPORT StyledItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
    Q_PROPERTY(int rowCountPerPage    MEMBER m_rowCountPerPage   )
    Q_PROPERTY(int columnCountPerPage MEMBER m_columnCountPerPage)

public:
    StyledItemDelegate()
    {
    }
    StyledItemDelegate(const StyledItemDelegate& other)
    {
    }

    /*! 
     * 系统库函数重写，获得所需要绘制数据的尺寸大小
     */
    QSize sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const Q_DECL_OVERRIDE;

protected:
    int m_rowCountPerPage;                  //!< 每行有多少个项目
    int m_columnCountPerPage;               //!< 每列有多少个项目
};





/*********************************************************************************************************
** End of file
*********************************************************************************************************/