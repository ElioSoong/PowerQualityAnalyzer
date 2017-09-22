/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  GpsTestView.cpp
** Latest Version:             V1.0.0
** Latest modified Date:       2016/08/01
** Modified by:                
** Descriptions:               
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 
** Created date:               2016/08/01
** Descriptions:               
** 
*********************************************************************************************************/

#include "GpsTestView.h"
#include "includes.h"

GpsTestView *GpsTestView::s_gpsTestView = NULL;

GpsTestView::GpsTestView(QWidget *parent)
    : QWidget(parent),
      m_signal(0),
      m_satellites(0),
      m_latitude(0),
      m_longitude(0),
      m_x(GPSTESTVIEW_WIDHN / 2),
      m_y(GPSTESTVIEW_HEIGTH / 2),
      m_gpsStatus(false),
      m_timer(NULL)
{
    m_signalLabel = new QLabel;
    m_signalLabel->setText(tr("XX"));
    setFontColor(m_signalLabel);

    m_satellitesLable = new QLabel;
    m_satellitesLable->setText(tr("0"));
    setFontColor(m_satellitesLable);

    m_longitudeLable = new QLabel;
    m_longitudeLable->setText(QStringLiteral("?_ _°_ _._ _ _′"));
    setFontColor(m_longitudeLable);

    m_latitudeLable = new QLabel;
    m_latitudeLable->setText(QStringLiteral("?_ _°_ _._ _ _′"));
    setFontColor(m_latitudeLable);

    QFormLayout *formLayout = new QFormLayout;
    formLayout->setVerticalSpacing(5);
    //formLayout->setHorizontalSpacing(10);
    formLayout->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);
    formLayout->setFormAlignment(Qt::AlignLeft|Qt::AlignBottom);
    formLayout->setLabelAlignment(Qt::AlignRight);
    formLayout->addRow(new QLabel(tr("信号强度:")), m_signalLabel);
    formLayout->addRow(new QLabel(tr("卫星数量:")), m_satellitesLable);
    formLayout->addRow(new QLabel(tr("经度:")), m_longitudeLable);
    formLayout->addRow(new QLabel(tr("纬度:")), m_latitudeLable);

    m_timer = new QTimer;
    connect(m_timer, SIGNAL(timeout()), this, SLOT(onTimeout()));
    m_timer->start(10000);
    this->setLayout(formLayout);
}

GpsTestView::~GpsTestView()
{

}

void GpsTestView::paintEvent(QPaintEvent *event)
{
    QPainter backgroundPainter(this);
    QPixmap backgroundPixmap = QPixmap("resources/map/worldMap.png").scaled(this->size());
    backgroundPainter.drawPixmap(this->rect(), backgroundPixmap);

    paintCoordinate();
    if (!m_gpsStatus) 
    {
        paintHint();
    }
}

GpsTestView * GpsTestView::instance()
{
    if (!s_gpsTestView)
    {
        s_gpsTestView = new GpsTestView;
    }
    return s_gpsTestView;
}

void GpsTestView::setCustomFont(QWidget *widget)
{
    QFont font("Arial Unicode MS", 10);
    widget->setFont(font);
}

void GpsTestView::setFontColor(QWidget *widget)
{
    widget->setStyleSheet("color: rgb(85, 85, 255);");
}

void GpsTestView::paintCoordinate()
{
    QPainter coordinatePainter(this);
    coordinatePainter.setRenderHint(QPainter::Antialiasing);
    coordinatePainter.setPen(Qt::red);
    coordinatePainter.setBrush(Qt::red);
    coordinatePainter.drawEllipse(m_x - 5, m_y - 5, 10, 10);
}

void GpsTestView::paintHint()
{
    QPainter hintPainter(this);
    QPixmap hintIcon = QPixmap("resources/icon/msgBox/loading1.png");
    hintPainter.drawPixmap(QRect(300, 420, 20, 20), hintIcon);
    hintPainter.setPen(Qt::red);
    hintPainter.drawText(QRect(320, 420, 200, 20), Qt::AlignLeft | Qt::AlignCenter, tr("未检测到卫星设备..."));
}

void GpsTestView::onPositionChanged(const QGeoPositionInfo &positionInfo)
{
    m_gpsStatus = true;
    m_timer->start(10000);
    qDebug() << "gpsTestView onPositionChanged:" << positionInfo.timestamp();
    QGeoCoordinate coordinate = positionInfo.coordinate();
    qDebug() << "latitude:" << coordinate.latitude();
    qDebug() << "longitude:" << coordinate.longitude();
    m_latitude = coordinate.latitude();
    m_longitude = coordinate.longitude();
    latLongToXY(m_latitude, m_longitude, 1);
    QString str = coordinate.toString(QGeoCoordinate::DegreesMinutesWithHemisphere);
    QStringList strList = str.split(",");
    m_latitudeLable->setText(strList[0].trimmed());
    m_longitudeLable->setText(strList[1].trimmed());
    update();
    qDebug() << this->size();
}

void GpsTestView::onSatelliteChanged(const QList<QGeoSatelliteInfo> &satelliteInfo)
{
    m_gpsStatus = true;
    m_satellites = satelliteInfo.count();
    int signalSum = 0;
    for (int i = 0; i < m_satellites; i++) 
    {
        signalSum = satelliteInfo[i].signalStrength();
    }
    if (m_satellites != 0) 
    {
        m_signal = signalSum / m_satellites;
    }
    else
    {
        m_signal = 0;
    }
    m_signalLabel->setText(QString::number(m_signal, 10));
    m_satellitesLable->setText(QString::number(m_satellites, 10));
}

void GpsTestView::onTimeout()
{
    m_gpsStatus = false;
    m_signal = 0;
    m_satellites = 0;
    m_latitude = 0;
    m_longitude = 0;
    m_x = GPSTESTVIEW_WIDHN / 2;
    m_y = GPSTESTVIEW_HEIGTH / 2;
    m_latitudeLable->setText(QStringLiteral("?_ _°_ _._ _ _′"));
    m_longitudeLable->setText(QStringLiteral("?_ _°_ _._ _ _′"));
    m_signalLabel->setText(tr("XX"));
    m_satellitesLable->setText(tr("0"));
    update();
}

double GpsTestView::clip(double value, double minValue, double maxValue)
{
    return qMin(qMax(value, minValue), maxValue);
}

uint GpsTestView::mapSize(int levelOfDetail)
{
    return (uint)(256 << levelOfDetail);
}

void GpsTestView::latLongToXY(double latitude, double longitude, int levelOfDetail)
{
    latitude = clip(latitude, MIN_LATITUDE, MAX_LATITUDE);
    longitude = clip(longitude, MIN_LONGITUDE, MAX_LONGITUDE);

    double x = (longitude + 180) / 360;
    double sinLatitude = qSin(latitude * PI / 180);
    double y = 0.5 - qLn((1 + sinLatitude) / (1 - sinLatitude)) / (4 * PI);
    uint size = mapSize(levelOfDetail);
    qDebug() << "mapSize" << size;

    int pixelX = (int) clip(x * size + 0.5, 0, size - 1);
    int pixelY = (int) clip(y * size + 0.5, 0, size - 1);
    m_x = pixelX * GPSTESTVIEW_WIDHN / size;
    //地图采用百度世界地图，根据需求需要把地图下面的南极去掉，m_y需要按比例进行调整，
    //本来地图有803个像素，删除下面南极占用部分（230个像素），剩余573个像素
    m_y = pixelY * GPSTESTVIEW_HEIGTH /size * 803 / 573;
    qDebug() << "m_x" << m_x;
    qDebug() << "m_y" << m_y;
}

/*********************************************************************************************************
** End of file
*********************************************************************************************************/
