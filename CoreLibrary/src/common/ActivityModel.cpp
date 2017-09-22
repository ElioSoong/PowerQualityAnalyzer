/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  ActivityModel.h
** Latest Version:             V1.0.0
** Latest modified Date:       2016/03/11
** Modified by:                
** Descriptions:               
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 Chen Honghao
** Created date:               2015/08/21
** Descriptions:               记录了全部可执行的测量活动的数据模型，用于主页的功能显示
** 
*********************************************************************************************************/
#include "includes.h"
#include "common/ActivityModel.h"


QModelIndex ActivityModel::index(int row, int column, const QModelIndex & parent) const
{
    Activity* activity = this->activity(row, 0);
    if (activity == NULL)
        return QModelIndex();

    Q_UNUSED(parent);

    return createIndex(row, column, activity);
}

/*! 
 * 获取各个测量活动的名称与图标，用于主页显示
 * @param[in]  index 模型索引
 * @param[in]  role  模型索引
 * @return     名称或图标
 */
QVariant ActivityModel::data(const QModelIndex & index, int role) const
{
    Activity* activity = reinterpret_cast<Activity*>(index.internalPointer());
    if (activity == NULL)
        return QVariant();

    switch (role)
    {
    case Qt::DisplayRole:
        return QVariant(activity->name());
        break;

    case Qt::DecorationRole:
        return QVariant(activity->icon());
        break;

    default:
        //qDebug("Reading unsupported role %d @ActivityModel[r=%d, c=%d].\n", role, index.row(), index.column());
        break;
    }

    return QVariant();
}

/*! 
 * 由活动名获得modelindex
 * @param[in]  QString 活动名称
 * @return     获得的modelindex
 */
QModelIndex ActivityModel::modelIndexFromName(const QString& activityName) const
{
    int rowCount = m_activities.size();
    for (int row = 0; row < rowCount; ++row)
    {
        if (m_activities.at(row)->objectName() == activityName)
            return index(row, 0);
    }

    return QModelIndex();
}

/*********************************************************************************************************
** End of file
*********************************************************************************************************/
