/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  HintMsgBox.cpp
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
#include "HintMsgBox.h"
#include <QVBoxLayout>
#include "includes.h"

HintMsgBox::HintMsgBox(QWidget *parent)
    : MsgBoxFrame(parent)
{
    setVisiblePos(MsgBoxFrame::Bottom);

    m_icon = new QLabel();
    m_title = new QLabel();
    m_text = new QLabel();
    setContentAttribute(m_icon);
    setContentAttribute(m_title);
    setContentAttribute(m_text);
    m_title->setFont(QFont("Arial Unicode MS",15));
    initCentralWidget();
    connect(buttonBox(), SIGNAL(clicked(QAbstractButton *)), this, SLOT(close()));
}

HintMsgBox::HintMsgBox(const QPixmap &icon, const QString & title, const QString & text, 
    MsgBoxFrame::StdButtons buttons, QWidget *parent):MsgBoxFrame(parent)
{
    m_icon = new QLabel();
    m_title = new QLabel();
    m_text = new QLabel();
    setContentAttribute(m_icon);
    setContentAttribute(m_title);
    setContentAttribute(m_text);
    m_icon->setPixmap(icon);
    m_title->setText(title);
    m_title->setFont(QFont("Arial Unicode MS",15));
    m_text->setText(text);

    addButtons(buttons);
    initCentralWidget();
    connect(buttonBox(), SIGNAL(clicked(QAbstractButton *)), this, SLOT(close()));
}

/*! 
 * 设置QLabel类型组件的属性
 * @param[in]  label：QLabel类型组件（图标，标题，提示信息）
 */
void HintMsgBox::setContentAttribute(QLabel *label)
{
    label->setAlignment(Qt::AlignCenter);
    label->setFont(QFont("Arial Unicode MS",12));
}

void HintMsgBox::showEvent(QShowEvent *event)
{
    if (!s_widgetStack.isEmpty())
    {
        //s_widgetStack.top()->hide();
    }
    
    MsgBoxFrame::showEvent(event);
}

HintMsgBox::~HintMsgBox()
{

}

/*! 
 * 初始化提示窗口的显示区域
 */
void HintMsgBox::initCentralWidget()
{
    m_centralWidget = new QWidget(this);
    QVBoxLayout *vLayout = new QVBoxLayout(m_centralWidget);
    vLayout->setContentsMargins(0, 10, 0, 0);

    vLayout->addWidget(m_icon);
    vLayout->addWidget(m_title);
    vLayout->addWidget(m_text);
    
    setCentralWidget(m_centralWidget);
}

QString HintMsgBox::text()
{
    return m_text->text();
}
QString HintMsgBox::title()
{
    return m_title->text();
}

void HintMsgBox::setText(const QString &text)
{
    m_text->setText(text);
}

void HintMsgBox::setTitle(const QString &title)
{
    m_title->setText(title);
}

 /*! 
  * 信息提示框
  * @param[in]  icon： 提示图标
  * @param[in]  title：提示框标题
  * @param[in]  text： 提示信息
  * @param[in]  buttons： 对提示做出的响应
  * @return    返回响应的按钮
  */
MsgBoxFrame::StdButton HintMsgBox::commonHint(const QPixmap & icon, const QString & title, 
    const QString & text, MsgBoxFrame::StdButtons buttons, QWidget *parent)
{
    HintMsgBox msgBox(icon, title, text, buttons, parent);
    msgBox.exec();
    return MsgBoxFrame::StdButton(msgBox.buttonBox()->standardButton(msgBox.clickedButton()));
}

MsgBoxFrame::StdButton HintMsgBox::errorHint(int code, const QString & title, const QString &hint, QWidget *parent)
{
    QString message = QString("code: %1\n%2").arg(code).arg(hint);
    HintMsgBox msgBox(ErrorIcon, title, message, MsgBoxFrame::Ok, NULL);
    msgBox.exec();

    return MsgBoxFrame::StdButton(msgBox.buttonBox()->standardButton(msgBox.clickedButton()));
}

MsgBoxFrame::StdButton HintMsgBox::showHint(const QString &hint)
{
    if (!m_icon->pixmap())
    {
        m_icon->setPixmap(ErrorIcon);
    }
    if (m_title->text().isEmpty())
    {
        m_title->setText("Error");
    }
    setText(hint);
    exec();

    return MsgBoxFrame::StdButton(buttonBox()->standardButton(clickedButton()));
}
/*********************************************************************************************************
** End of file
*********************************************************************************************************/