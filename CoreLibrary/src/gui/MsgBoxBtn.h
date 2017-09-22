/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  MsgBoxBtn.h
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
#ifndef MSGBOXBTN_H
#define MSGBOXBTN_H

#include <QPushButton>
#include <QPainter>
#include <QKeyEvent>

class MsgBoxBtn : public QPushButton
{
    Q_OBJECT

public:
    MsgBoxBtn(QWidget *parent);
    MsgBoxBtn(const QIcon &icon, const QString &text, QWidget *parent = 0);
    ~MsgBoxBtn();

    void setSelectedState(bool state);
    bool selectedState();

    void pressedState(bool state);

protected:
    void mousePressEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *e) Q_DECL_OVERRIDE;

private:
    bool isSelected;
};

#endif // MSGBOXBTN_H
/*********************************************************************************************************
** End of file
*********************************************************************************************************/
