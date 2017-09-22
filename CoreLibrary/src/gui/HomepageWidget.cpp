/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  HomepageWidget.cpp
** Latest Version:             V1.0.0
** Latest modified Date:       2016/03/16
** Modified by:                
** Descriptions:               
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 Chen Honghao
** Created date:               2016/03/16
** Descriptions:               主页控件，使用MVC，使用该类作为代码模板，可快速实现九宫格样式
** 
*********************************************************************************************************/
#include <QApplication>
#include <QScrollBar>

#include "includes.h"
#include "HomepageWidget.h"

HomepageWidget::HomepageWidget(QWidget *parent)
    : QListView(parent), m_rowCountPerPage(1)
    , m_columnCountPerPage(1), m_hasTimer(false)
{
    QPalette homepagePalette = palette();
    homepagePalette.setColor(QPalette::Window, QColor("transparent"));
    homepagePalette.setColor(QPalette::Base, QColor("transparent"));
    homepagePalette.setColor(QPalette::Text, QColor("white"));
    setPalette(homepagePalette);
    setStyleSheet("border-image: url(resources/texture/homepage.png);color:white;");
    setFlow(QListView::TopToBottom);
    setViewMode(QListView::IconMode);
    setUniformItemSizes(true);
    setResizeMode(QListView::Fixed);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setMouseTracking(false);
    
    QFont font;
    font.setFamily("Arial Unicode MS");
    font.setPointSize(10);
    setFont(font);
    resize();
    setViewportMargins(25, 17,0,0);
    horizontalScrollBar()->setValue(0);
    connect(horizontalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(onValueChanged(int)));

    connect(&m_tapAndHoldTimer, &QTimer::timeout, this, &HomepageWidget::handleTapAndHold);
    QTapAndHoldGesture::setTimeout(500);
    viewport()->grabGesture(Qt::TapAndHoldGesture);
}

HomepageWidget::~HomepageWidget()
{

}

void HomepageWidget::keyPressEvent(QKeyEvent *event)
{
    int key = event->key();
    
    qDebug() << m_columnCountPerPage;
    qDebug() << m_rowCountPerPage;
    if (key == Qt::Key_Up) 
    {
        setVerticalItem(1);
        return;
    }
    else if (key == Qt::Key_Down)
    {
        setVerticalItem(-1);
        return;
    }
    else if (key == Qt::Key_Right)
    {
        setHorizontalItem(1);
        return;
    }
    else if (key == Qt::Key_Left)
    {
        setHorizontalItem(-1);
        return;
    }
    return QListView::keyPressEvent(event);
}

void HomepageWidget::setIsHasTimer(bool value)
{
    m_hasTimer = value;
}

/*!
 * 当水平滑条的值改变时候会调用的slot
 * @param[in]  当前水平滑条的值
 * @return     void
 */
void HomepageWidget::onValueChanged(int value)
{
    horizontalScrollBar()->setValue(0);
}

bool HomepageWidget::resize()
{
    int w = viewport()->width();
    int h = viewport()->height();

    int iw = w / m_columnCountPerPage + 2;
    int ih = h / m_rowCountPerPage - 10;

    setGridSize(QSize(iw ,ih));
    setIconSize(QSize(iw, ih - 0));

    return true;
}

void HomepageWidget::resizeEvent( QResizeEvent * event )
{
    resize();
}

bool HomepageWidget::viewportEvent(QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent *e = static_cast<QMouseEvent *>(event);
        m_tapPoint = e->pos();
        m_startIndex = indexAt(m_tapPoint);
        m_tapAndHoldTimer.start(500);
    }
    else if (event->type() == QEvent::MouseButtonRelease)
    {
        if (m_hasTimer)
        {
            return true;
        }
        m_tapAndHoldTimer.stop();
    }
    else if (event->type() == QEvent::MouseMove)
    {
        QMouseEvent *e = static_cast<QMouseEvent *>(event);
        QModelIndex curIndex = indexAt(e->pos());
        if (curIndex != m_startIndex)
        {
            m_tapAndHoldTimer.stop();
        }
    }
    return QListView::viewportEvent(event);
}

void HomepageWidget::handleTapAndHoldGesture(QTapAndHoldGesture *tap)
{
    qDebug() << "tap and hold";
    QModelIndex index = indexAt(tap->position().toPoint());
    if (index.isValid())
    {
        emit indexIsTouchedAndHeld(index);
    }
}

void HomepageWidget::handleTapAndHold()
{
    m_hasTimer = true;
    m_tapAndHoldTimer.stop();
    qDebug() << "tap and hold";
    QModelIndex index = indexAt(m_tapPoint);
    if (index.isValid())
    {
        emit indexIsTouchedAndHeld(index);
    }
}

void HomepageWidget::setVerticalItem(int orientation)
{
    if (m_rowCountPerPage <= 0 || m_columnCountPerPage <= 0)
    {
        return;
    }
    QModelIndex index = currentIndex();
    int row = index.row();
    int column = index.column();
    int remainder = 0;
    if (orientation > 0)
    {
        remainder = row % m_rowCountPerPage;
    }
    else
    {
        remainder = (row + 1) % m_rowCountPerPage;
    }
    
    if (remainder == 0)
    {
        row = row + (m_rowCountPerPage - 1) * orientation;
    }
    else
    {
        row = row - 1 * orientation;
    }
    QModelIndex nextItemIndex = model()->index(row, 0);
    setCurrentIndex(nextItemIndex);
}

void HomepageWidget::setHorizontalItem(int orientation)
{
    if (m_rowCountPerPage <= 0 || m_columnCountPerPage <= 0)
    {
        return;
    }
    QModelIndex index = currentIndex();
    int row = index.row();
    int column = index.column();
    int  boundary = 0;
    if (orientation > 0)
    {
        boundary = (row + 1) - m_rowCountPerPage * (m_columnCountPerPage - 1);
    }
    else
    {
        boundary = m_rowCountPerPage - row;
    }
    if (boundary > 0)
    {
        row = row - m_rowCountPerPage * (m_columnCountPerPage - 1) * orientation;
    }
    else
    {
        row = row + m_rowCountPerPage * orientation;
    }
    QModelIndex nextItemIndex = model()->index(row, 0);
    setCurrentIndex(nextItemIndex);
}

/*********************************************************************************************************
** End of file
*********************************************************************************************************/
