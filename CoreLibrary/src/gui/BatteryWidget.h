/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  BatteryWidget.h 
** Latest Version:             V1.0.0
** Latest modified Date:       2015/10/27
** Modified by:                
** Descriptions:               
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 Zhang Wenbo
** Created date:               2015/10/27
** Descriptions:               
** 
*********************************************************************************************************/

#ifndef BATTERYWIDGET_H
#define BATTERYWIDGET_H

#include "corelibrary_global.h"
#include <QWidget>
#include <QAbstractItemModel>
#include "FieldLibrary/FieldModel.h"
//#include "common/F280Application.h"

#define BATTERY_NUM 255

class CORELIBRARY_EXPORT BatteryWidget : public QWidget
{
    Q_OBJECT
    Q_ENUMS(BatteryStatus)

public:
    BatteryWidget(QWidget *parent = 0);
    BatteryWidget(const BatteryWidget &){};
    ~BatteryWidget();

    enum BatteryStatus {
        InvalidStatus = -1,
        NormalStatus,           //!< 未充电，使用中
        ChargeStatus,           //!< 正在充电
        ChargeCompleteStatus,   //!< 充电完成
        NoChargeStatus          //!< 没有电池
    };

    /*! 
     * 设置数据模型,并将模型中的数据转化为本类的数据格式设置到对应相位角和相色中
     * @param[in]  model 数据模型，实际为 QAbstractItemModel*
     */
    void setModel (QAbstractItemModel *model);

    /*! 
    * 设置电池状态模式
    * @param[in] BatteryStatus 电池状态
    */
    void setBatteryStatus(enum BatteryStatus status);

    /*! 
    * 设置电量大小
    * @param[in] num 当前电量值
    */
    void setElectricity(int num);

    void paintEvent(QPaintEvent *);
    

signals:
    void adaptorSatatusChanged();
private slots:

    void modelDataChanged(const QModelIndex & changedIndex1, const QModelIndex & changedIndex2, QVector<int>roles);

private:
    /*! 
     * 根据设备电池管理状态判断是否显示电池和适配器
     * @param[in] battery是否显示电池，true显示，否则不显示
     * @param[in] adapter是否显示适配器，true显示，false不显示
     */
    void paint(bool battery, bool adapter);
    enum BatteryStatus m_status;                //!< 当前电池状态模式
    int m_electricityVal;                       //!< 当前电池电量值
    FieldModel * m_model;                       //!< 保存数据源Model指针
    bool m_hasAdaptor;
};
Q_DECLARE_METATYPE(BatteryWidget)
Q_DECLARE_METATYPE(BatteryWidget::BatteryStatus)

#endif // BATTERYWIDGET_H

/*********************************************************************************************************
** End of file
*********************************************************************************************************/
