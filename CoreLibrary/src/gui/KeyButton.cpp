/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  KeyButton.cpp
** Latest Version:             V1.0.0
** Latest modified Date:       2017/02/07
** Modified by:                
** Descriptions:               
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 Zhang WenBo
** Created date:               2017/02/07
** Descriptions:               END
** 
*********************************************************************************************************/

#include "KeyButton.h"

KeyButton::KeyButton(QWidget *parent)
    : QPushButton(parent)
{
    connect(this, SIGNAL(clicked()), this, SLOT(on_clicked()));
}

KeyButton::~KeyButton()
{

}

void KeyButton::on_clicked()
{
    emit keyClicked(text());
}

/*********************************************************************************************************
** End of file
*********************************************************************************************************/

