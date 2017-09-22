/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  BatteryWidget.cpp 
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
#include "Application.h"
#include "BatteryWidget.h"
#include "SysStatusModel.h"
#include <QPainter>
#include <QDebug>

#define BATTERY_STATUS_XPATH          "/sys/setup/userPreferenceSet/batteryStatus"
#define BATTERY_ELECTRICITY_XPATH     "/sys/setup/userPreferenceSet/batteryElectricity"

BatteryWidget::BatteryWidget(QWidget *parent)
    : QWidget(parent)
    , m_status(NormalStatus)
    , m_electricityVal(255)
    , m_hasAdaptor(true)
{
    setAutoFillBackground(true);
    QPalette p = this->palette();
    p.setColor(QPalette::Background, QColor(1,12,32));
    setPalette(p);

    connect(this, SIGNAL(adaptorSatatusChanged()), Application::instance(), SLOT(resetTimer()));
}

BatteryWidget::~BatteryWidget()
{

}

void BatteryWidget::setModel(QAbstractItemModel *model)
{
    connect(SysStatusModel::instance(), SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),
            this, SLOT(modelDataChanged(QModelIndex,QModelIndex,QVector<int>)));
    FieldModel *m = static_cast<FieldModel*>(model);

    // 保存数据源Model指针
    m_model = m;
}

void BatteryWidget::setBatteryStatus(enum BatteryStatus status)
{
    m_status = status;
    bool hasAdaptor = false;
    if (m_status == NormalStatus)
    {
        hasAdaptor = false;
    }
    else
    {
        hasAdaptor = true;
    }
    if (m_hasAdaptor != hasAdaptor)
    {
        emit adaptorSatatusChanged();
        m_hasAdaptor = hasAdaptor;
    }
    //emit batterySatatusChanged(status);
    
    //qDebug() << "batteryStatus" << m_status;
    update();
}

void BatteryWidget::setElectricity(int num)
{
    m_electricityVal = BATTERY_NUM - num;
    //qDebug() << "batteryCapacity:" << m_electricityVal;
    update();
}

void BatteryWidget::modelDataChanged(const QModelIndex & changedIndex1, const QModelIndex & changedIndex2, QVector<int>roles)
{
    // 刷新数据，重新获取model中电池状态以及电量信息并设置此电池控件
    QModelIndex indexStatus = m_model->index(BATTERY_STATUS_XPATH);
    QModelIndex indexNum = m_model->index(BATTERY_ELECTRICITY_XPATH);
    if (!indexNum.isValid() || !indexStatus.isValid())
        return;
    bool ok;
    int curStatus = indexStatus.data(Qt::DisplayRole).toInt(&ok);
    //qDebug() << "batteryWidgetCurStatus:" << curStatus;
    if (ok && curStatus >= 0)
    {

        setBatteryStatus((BatteryStatus)curStatus);
    }
    else
    {
        qDebug() << "getBatteryStatus from system Failed!!";
    }

    int curNum = indexNum.data(Qt::DisplayRole).toInt(&ok);
    //qDebug() << "batteryWidgetCurNum:" << curNum;
    if (ok && curNum >= 0)
    {
        setElectricity(curNum);
    }
    else
    {
        qDebug() << "getElectricityNumeber from system Failed!!";
    }
}

void BatteryWidget::paintEvent(QPaintEvent *)
{
    if (m_status == NormalStatus) 
    {
        //qDebug() << "enumBatteryStatus NormalStatus";
        QPainter dischargingPainter(this);
        QColor dischargingColor = QColor(255, 255, 255);
        QRect r = QRect(1, 1, (width() - 3) * m_electricityVal / 255, height() - 2);
        dischargingPainter.fillRect(r, dischargingColor);
        paint(true, false);
    }
    else if (m_status == ChargeStatus)
    {
        //qDebug() << "enumBatteryStatus ChargeStatus:";
        QPainter chargingPainter(this);
        QColor chargingColor = QColor(0, 200, 0);
        QRect r = QRect(1, 1, (width()-3) * m_electricityVal / 255, height() - 2);
        chargingPainter.fillRect(r, chargingColor);
        paint(true, true);
    }
    else if (m_status == ChargeCompleteStatus) 
    {
        QPainter fullPainter(this);
        QColor fullColor = QColor(0, 200, 0);
        QRect r = QRect(1, 1, (width() - 3), (height() - 2));
        fullPainter.fillRect(r, fullColor);
        paint(true, false);
        //qDebug() << "enumBatteryStatus ChargeCompleteStatus:";
    }
    else
    {
        //qDebug() << "enumBatteryStatus NoChargeStatus:";
        paint(false, true);
        //Do nothing
    }
}

void BatteryWidget::paint(bool battery, bool adapter)
{
    int h = height();
    int w = width();
    QPainter painter(this);
    painter.setRenderHint(QPainter::HighQualityAntialiasing);
    QColor displayColor = QColor(255, 255, 255);
    if (battery) 
    {
        // 绘制电池边框
        QRect rLeft = QRect(0, 1, 1, h - 2);
        QRect rTop = QRect(1, 0, w - 3, 1);
        QRect rRight1 = QRect(w - 2, 1, 1, h - 2);
        QRect rRight2 = QRect(w - 2,(h - 2) / 4 + 1 , 2, (h - 2) / 2);
        QRect rBottom = QRect(1, h - 1, w - 3, 1);
        painter.fillRect(rLeft, displayColor);
        painter.fillRect(rTop, displayColor);
        painter.fillRect(rRight1, displayColor);
        painter.fillRect(rRight2, displayColor);
        painter.fillRect(rBottom, displayColor);
    }
    
    if (adapter) 
    {
        painter.setRenderHint(QPainter::SmoothPixmapTransform);
        painter.drawPixmap(0, 0, width(), height(), QPixmap("resources/icon/status/adapter"));
    }
}

/*********************************************************************************************************
** End of file
*********************************************************************************************************/
