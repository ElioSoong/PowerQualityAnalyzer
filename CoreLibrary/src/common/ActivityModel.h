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
#ifndef ACTIVITYMODEL_H
#define ACTIVITYMODEL_H

#include "corelibrary_global.h"
#include <QAbstractListModel>
#include <QtGui/QStandardItemModel>

#include "common/Activity.h"

/**
 *  @class ActivityModel
 *  @brief 记录了全部可执行的测量活动的数据模型，用于主页的功能显示
 */
class CORELIBRARY_EXPORT ActivityModel : public QAbstractListModel
{
    Q_OBJECT

    Q_ENABLE_COPY(ActivityModel)
    Q_PROPERTY(QList<Activity*> activities MEMBER m_activities)

public:
    ActivityModel(QObject *parent = 0) : QAbstractListModel(parent)
    {

    }

    /*!
    * 获取返回所有测量活动项
    */
    QList<Activity*> activitys() const
    {
        return m_activities;
    }

    /*! 
     * 获取模型索引对应的测量活动对象，仅用于MainWindow
     * @param[in]  index 模型索引
     * @return     对应的测量活动对象
     */
    Activity* activity(const QModelIndex& index) const
    {
        return activity(index.row(), 0);
    }

   /*!
    * 获取测量模式对应的测量活动对象
    * @param[in]  modemode 测量模式
    * @return     对应的测量活动对象
    */
    Activity* activity(int mode) const
    {
        int rowCount = m_activities.size();
        for (int row = 0; row < rowCount; ++row)
        {
            Activity* activity = m_activities.at(row);
            if (activity && activity->mode() == mode)
                return activity;
        }

        return NULL;
    }

    /*! 
     * 由活动名获得modelindex
     * @param[in]  QString 活动名称
     * @return     获得的modelindex
     */
    QModelIndex modelIndexFromName(const QString& activityName) const;

protected:   
    /*! 
     * 获取指定行号对应的测量活动对象
     * @param[in]  row 行号
     * @return     对应的测量活动对象
     */
    Activity* activity(int row, int column) const
    {
        if (row >= m_activities.size() || row < 0)
            return NULL;

        return m_activities[row];
    }

    /*! 
     * 重写Qt类库的部分虚函数（详见Qt帮助文档-QAbstractItemModel）：
     * [1] 生成模型索引
     * [2] 获取数据行数
     * [3] 获取数据
     */
    QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const Q_DECL_OVERRIDE;

    /*! 
     * 获取activityModel保存有多少个activity
     * @param[in]  parent 父模型索引
     * @return     返回测量活动的数量
     */
    int rowCount(const QModelIndex & parent = QModelIndex()) const Q_DECL_OVERRIDE
    {
        Q_UNUSED(parent);
        return m_activities.size();
    }

    /*! 
     * 获取各个测量活动的名称与图标，用于主页显示
     * @param[in]  index 模型索引
     * @param[in]  role  模型索引
     * @return     名称或图标
     */
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

private:
    QList<Activity*> m_activities;          //!< 测量活动列表

    friend class Test_ActivityModel;
    friend class TaskStorageItem;
};

#endif // ACTIVITYMODEL_H
/*********************************************************************************************************
** End of file
*********************************************************************************************************/
