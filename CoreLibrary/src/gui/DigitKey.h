/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  DigitKey.h 
** Latest Version:             V1.0.0
** Latest modified Date:       2015/08/04
** Modified by:                
** Descriptions:               数字键盘控件
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 Zhang Wenbo
** Created date:               2015/08/05
** Descriptions:               
** 
*********************************************************************************************************/
#pragma once
#include "qwidget.h"
#include <QButtonGroup>
#include "ui_digitKey.h"
#include <QTimer>
#include <QPainter>

namespace Ui {
    class DigitKey;
}
class DigitKey : public QWidget
{
    Q_OBJECT

public:
    //!<  构造函数和析构函数
    DigitKey(QWidget *parent = 0);
    ~DigitKey();

#if 1
    void showEvent(QShowEvent *) Q_DECL_OVERRIDE 
    {
        ui->lineEdit->activateWindow();
        ui->lineEdit->setFocus();
        ui->lineEdit->selectAll();
    }
#endif

    /*
     * 当接收到sigOkPressSignal()信号后，可以调用该函数，获取当前文本框中的文字
     */
    QString getInputStr();

    /*
     * 设置显示风格
     */
    void setDefaultStyle();

    /*! 
     * 设置文本框的内容
     */
    void setshow(QString str);

    bool eventFilter(QObject *target, QEvent *event)
    {
        if (target == ui->pushButton_Cancle && event->type() == QEvent::MouseButtonPress)
        {
            if (m_timer && !m_timer->isActive())
            {
                m_timer->start(1500);
            }
        }

        if (target == ui->pushButton_Cancle && event->type() == QEvent::MouseButtonRelease)
        {
            if (m_timer && m_timer->isActive())
            {
                m_timer->stop();
            }
        }

        if (target == ui->lineEdit)
        {
            if (event->type() == QEvent::FocusOut)
            {
                return true;
            }
        }
        return QWidget::eventFilter(target, event);
    };

signals:
    /*
     * enter 、确认、下押时发送该信号
     */
    void sigOkPressSignal();

private slots:
    /*
     * 数字按键Button点击响应槽
     */
    void onGroupDigitClicked(int id);

    /*
     * 删除退格按键响应槽
     */
    void onCancleButtonPressed();

    void onCancleButtonReleased();

    /*
     * 按钮长按事件响应
     */
    void onButtonLongPressed()
    {
        ui->lineEdit->clear();
    };

    void onCursorPositionChange(int old, int newPos);

private:
    Ui::digitKey *ui;
    QButtonGroup* groupDigit;       //!< 统一管理数字按键Button
    QString m_inputStr;             //!< 存放输入的内容
    QTimer * m_timer;               // 用于判断是否为为长按事件的定时器
};

/*********************************************************************************************************
** End of file
*********************************************************************************************************/

