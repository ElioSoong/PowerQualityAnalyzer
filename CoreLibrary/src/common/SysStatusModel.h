/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  SysStatusModel.h 
** Latest Version:             V1.0.0
** Latest modified Date:       2016/03/25
** Modified by:                
** Descriptions:               
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 Zhang Wenbo
** Created date:               2016/03/25
** Descriptions:            用於定時檢測系統狀態的Model
** 
*********************************************************************************************************/

#include "corelibrary_global.h"
#include "FieldLibrary/FieldModel.h"
#include "FieldLibrary/FileField.h"
#include "gui/BatteryWidget.h"
#include <QTimer>
#include <QDebug>

#define     POWER_SUPPLY_STATUS_UNKNOWN       "Unknown"          // 没电池
#define     POWER_SUPPLY_STATUS_CHARGING      "Charging"        // 正在充电
#define     POWER_SUPPLY_STATUS_DISCHARGING   "Discharging"     // 未充电，使用中
#define     POWER_SUPPLY_STATUS_NOT_CHARGING  "NotCharging"     // 没有电池
#define     POWER_SUPPLY_STATUS_FULL          "Full"            // 充满电

class CORELIBRARY_EXPORT SysStatusModel : public FieldModel
{
    Q_OBJECT
private: 
    SysStatusModel()
    {
        m_timer.start(1000);
        connect(&m_timer, &QTimer::timeout, this, &SysStatusModel::onSysStatusChanged);
    };

public:
    static SysStatusModel * instance();

private slots:
    void onSysStatusChanged()
    {
        emit this->rawDataUpdated();
    }

private:
    QTimer                  m_timer;
    static SysStatusModel*  s_sysModel;
};

class F280BatteryStatusModel : FieldModel
{
public:
    F280BatteryStatusModel()
    {
        m_sysStatusModel = SysStatusModel::instance();
    }

    F280BatteryStatusModel(const F280BatteryStatusModel& other)
    {

    }

    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE
    {
        IField *field = reinterpret_cast<IField*>(index.internalPointer());
        TextFileField *fileField = static_cast<TextFileField *>(field);
        if (fileField)
        {
            QString fileString = fileField->data().toString();
            bool ok;
            int capcity = fileString.toInt(&ok);
            if (ok)
            {
                //qDebug() << "sysStatusCapcity:" << capcity;
                return capcity;
            }
            else
            {
                //qDebug() << "sysStatusFileString:" << fileString;
                return fileStringtoEnum(fileString.trimmed());
            }
        }
        return QVariant();
    }

private:
    /*! 
     *  根据文件中读取的String字符转换为对应显示枚举
     * @param [in]   fileString 文件中读取的字符
     * @return   返回界面显示控制的枚举int  
     */
    int fileStringtoEnum(const QString & fileString) const
    {
        if (POWER_SUPPLY_STATUS_CHARGING == fileString)
        {
            return BatteryWidget::ChargeStatus;
        }
        else if (POWER_SUPPLY_STATUS_DISCHARGING == fileString)
        {
            return BatteryWidget::NormalStatus;
        }
        else if (POWER_SUPPLY_STATUS_FULL == fileString)
        {
            return BatteryWidget::ChargeCompleteStatus;
        }
        else
        {
            return BatteryWidget::NoChargeStatus;
        }
    }

private:
    SysStatusModel* m_sysStatusModel;
};

/*********************************************************************************************************
** End of file
*********************************************************************************************************/
