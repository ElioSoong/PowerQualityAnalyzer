/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  GpsTestView.h
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

#ifndef GPSTESTVIEW_H
#define GPSTESTVIEW_H

#include <QWidget>
#include <QLabel>
#include <QtPositioning/QGeoPositionInfo>
#include <QtPositioning/QGeoSatelliteInfo>
#include <QLineEdit>
#include <QPainter>
#include <QFormLayout>
#include <QDebug>
#include <QFont>
#include <QTimer>
#include <QtMath>

#define GPSTESTVIEW_WIDHN 800                                     //!< gpsTestView 宽度
#define GPSTESTVIEW_HEIGTH 449                                    //!< gpsTestView 高度
#define MAX_LONGITUDE 180                                         //!< 经度范围最大值
#define MIN_LONGITUDE -180                                        //!< 经度最小值
#define MAX_LATITUDE 85.05112878                                  //!< 纬度范围最大值
#define MIN_LATITUDE -85.05112878                                 //!< 纬度最小值   
#define PI 3.14159265358979

class GpsTestView : public QWidget
{
    Q_OBJECT

public:
    /*! 
     * 单例模式
     */
    static GpsTestView *instance();
    /*! 
     * 重写paintEvent()函数
     */
    void paintEvent(QPaintEvent *event);

signals:

protected slots:
    /*! 
     * 接收GPS位置信息
     * @param[in] positionInfo位置信息
     */
    void onPositionChanged(const QGeoPositionInfo &positionInfo);
    /*! 
     * 接收卫星信息
     * @param[in] satelliteInfo 卫星信息列表
     */
    void onSatelliteChanged(const QList<QGeoSatelliteInfo> &satelliteInfo);
    /*! 
     * 定时器超时
     */
    void onTimeout();

private:
    GpsTestView(QWidget *parent = NULL);
    ~GpsTestView();
    /*! 
     * 设置控件字体
     * @return widget 需要设置的控件
     */
    void setCustomFont(QWidget *widget);
    /*! 
     * 设置控件字体颜色
     * @return widget 需要设置的控件
     */
    void setFontColor(QWidget *widget);
    /*! 
     * 在地图上标记坐标
     */
    void paintCoordinate();
    /*! 
     * 画提示信息 GPS未打开
     */
    void paintHint();
    /*! 
     * 限定值在指定的最大值与最小值之间
     * @param[in] value被限定值
     * @param[in] minValue 最小值
     * @param[in] maxValue 最大值
     * @return 返回在指定范围的一个值
     */
    double clip(double value, double minValue, double maxValue);
    /*! 
     * 确定地图的高度和宽度在指定的放大级别
     * @param[in] levelOfDetail 放大级别
     * @return 地图的大小
     */
    uint mapSize(int levelOfDetail);
    /*! 
     * 经纬度转换为地图上的一个点
     * @param[in] latitude 纬度
     * @param[in] longitude 经度
     * @param[in] levelOfDetail 放大级别
     * @return 
     */
    void latLongToXY(double latitude, double longitude, int levelOfDetail);
private:
    static GpsTestView *s_gpsTestView;                            
    QLabel *m_signalLabel;
    QLabel *m_satellitesLable;
    QLabel *m_longitudeLable;
    QLabel *m_latitudeLable;
    bool    m_gpsStatus;
    int     m_signal;
    int     m_satellites;
    double  m_longitude;
    double  m_latitude;
    int     m_x;
    int     m_y;
    QTimer *m_timer;
};

#endif // GPSTESTVIEW_H
