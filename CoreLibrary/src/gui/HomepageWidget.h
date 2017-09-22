/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  HomepageWidget.h
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
#ifndef HOMEPAGEWIDGET_H
#define HOMEPAGEWIDGET_H

#include "corelibrary_global.h"
#include <QListView>
#include <QTapAndHoldGesture>
#include <QKeyEvent>
#include <QTimer>
#include <QPoint>

class CORELIBRARY_EXPORT HomepageWidget : public QListView
{
    Q_OBJECT

public:
    HomepageWidget(QWidget *parent = NULL);
    ~HomepageWidget();

    /*
     * Getter/Setter for m_rowCountPerPage
     */
    int  rowCountPerPage() const 
    { 
        return m_rowCountPerPage; 
    }
    void setRowCountPerPage(int val) 
    { 
        m_rowCountPerPage = val; 
    }

    /*
     * Getter/Setter for m_columnCountPerPage
     */
    int columnCountPerPage() const 
    { 
        return m_columnCountPerPage; 
    }
    void setColumnCountPerPage(int val) 
    { 
        m_columnCountPerPage = val; 
    }

    void keyPressEvent(QKeyEvent *event);

    void setIsHasTimer(bool value);

signals:
    void indexIsTouchedAndHeld(const QModelIndex index);

private slots:
    /*! 
     * 当水平滑条的值改变时候会调用的slot
     * @param[in]  当前水平滑条的值
     * @return     void
     */
    void onValueChanged(int);

    void handleTapAndHold();

protected:
    bool resize();
    void resizeEvent(QResizeEvent * event);
    bool viewportEvent(QEvent *event) Q_DECL_OVERRIDE;
    void handleTapAndHoldGesture(QTapAndHoldGesture *tap);

protected:
    int m_rowCountPerPage;
    int m_columnCountPerPage;
    QTimer  m_tapAndHoldTimer;
    QPoint  m_tapPoint;
    bool    m_hasTimer;
    QPoint  m_startPos;
    QModelIndex m_startIndex;

private:
    void setVerticalItem(int orientation);                        //!< 光标的移动方向，向上为正1，下为负1
    void setHorizontalItem(int orientation);                      //!< 光标的移动方向，向右为正1，左为负1
};

#endif // HOMEPAGEWIDGET_H
/*********************************************************************************************************
** End of file
*********************************************************************************************************/
