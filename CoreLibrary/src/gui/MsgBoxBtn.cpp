/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  MsgBoxBtn.cpp
** Latest Version:              
** Latest modified Date:       
** Modified by:                
** Descriptions:               
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 Song Yunhua
** Created date:               2016/02/17 
** Descriptions:               
** 
*********************************************************************************************************/
#include "MsgBoxBtn.h"

MsgBoxBtn::MsgBoxBtn(QWidget *parent)
    : QPushButton(parent)
{
    setSelectedState(false);
}

MsgBoxBtn::MsgBoxBtn(const QIcon &icon, const QString &text, QWidget *parent)
    : QPushButton(parent)
{
    this->setIcon(icon);
    this->setText(text);
    this->setSelectedState(false);
}

MsgBoxBtn::~MsgBoxBtn()
{

}

void MsgBoxBtn::setSelectedState(bool state)
{
    QString styleStr = QString::fromLocal8Bit("font: 11pt; font-family: \"Arial Unicode MS\";\
        border-image: url(resources/icon/buttons/unselected.png);");
    isSelected = state;
    if (state)
    {
        styleStr.append("background: black;");

    }
    this->setStyleSheet(styleStr);
}

bool MsgBoxBtn::selectedState()
{
    return isSelected;
}

void MsgBoxBtn::pressedState(bool state)
{
    QString styleStr = QString::fromLocal8Bit("font: 11pt; font-family: \"Arial Unicode MS\";");
    if (state)
    {
        styleStr.append("border-image: url(resources/icon/buttons/selected.png);");
    }
    else
    {
        styleStr.append("border-image: url(resources/icon/buttons/unselected.png);");
    }

    if (selectedState() || hasFocus())
    {
        styleStr.append("background: black;");
    }
    this->setStyleSheet(styleStr);
}

void MsgBoxBtn::mousePressEvent(QMouseEvent *e)
{
    pressedState(true);
}

void MsgBoxBtn::mouseReleaseEvent(QMouseEvent *e)
{
    pressedState(false);
    emit clicked();
}

/*********************************************************************************************************
** End of file
*********************************************************************************************************/