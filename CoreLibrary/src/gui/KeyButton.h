/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  KeyButton.h
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
#pragma once
#ifndef KEYBUTTON_H
#define KEYBUTTON_H

#include <QPushButton>

class KeyButton : public QPushButton
{
    Q_OBJECT

public:
    KeyButton(QWidget *parent);
    ~KeyButton();

public slots:
    void on_clicked();

signals:
    void keyClicked(const QString &text);

private:
    
};

#endif // KEYBUTTON_H

/*********************************************************************************************************
** End of file
*********************************************************************************************************/
