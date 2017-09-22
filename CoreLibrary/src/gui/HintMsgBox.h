/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  HintMsgBox.h
** Latest Version:             
** Latest modified Date:       
** Modified by:                
** Descriptions:               
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 Song Yunhua
** Created date:               2016/03/24 
** Descriptions:               
** 
*********************************************************************************************************/
#ifndef HINTMSGBOX_H
#define HINTMSGBOX_H

#include "corelibrary_global.h"
#include "MsgBoxFrame.h"
#include <QLabel>

#define ErrorIcon QPixmap("resources/icon/msgBox/warning.png")

class CORELIBRARY_EXPORT HintMsgBox : public MsgBoxFrame
{
    Q_OBJECT

public:
    HintMsgBox(QWidget *parent = 0);
    HintMsgBox(const QPixmap &icon, const QString & title,
        const QString & text, MsgBoxFrame::StdButtons buttons = MsgBoxFrame::NoButton, QWidget *parent = NULL);
    ~HintMsgBox();
    
    /*! 
     * 信息提示框
     * @param[in]  icon： 提示图标
     * @param[in]  title：提示框标题
     * @param[in]  text： 提示信息
     * @param[in]  buttons： 对提示做出的响应
     * @return    返回响应的按钮
     */
    static MsgBoxFrame::StdButton commonHint(const QPixmap & icon, const QString & title, 
        const QString & text, MsgBoxFrame::StdButtons buttons = MsgBoxFrame::Ok, QWidget *parent = NULL);

    static MsgBoxFrame::StdButton errorHint(int code, const QString & title, const QString &hint, QWidget *parent = NULL);

public:
    /*! 
     * 获取提示信息
     */
    QString text();
    QString title();

public slots:
    MsgBoxFrame::StdButton showHint(const QString &hint);

    void setText(const QString &text);
    void setTitle(const QString &title);

protected:
    /*! 
     * 初始化提示窗口的显示区域
     */
    void initCentralWidget();

    /*! 
     * 设置QLabel类型组件的属性
     * @param[in]  label：QLabel类型组件（图标，标题，提示信息）
     */
    void setContentAttribute(QLabel *label);

    void showEvent(QShowEvent *event) Q_DECL_OVERRIDE;

private:
    QLabel *m_icon;
    QLabel *m_title;
    QLabel *m_text;
    QWidget *m_centralWidget;       //!< 用来放m_icon, m_title, m_text的容器
};

#endif // HINTMSGBOX_H
/*********************************************************************************************************
** End of file
*********************************************************************************************************/

