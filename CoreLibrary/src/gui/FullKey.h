/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  FullKey.h 
** Latest Version:             V1.0.0
** Latest modified Date:       2015/08/04
** Modified by:                
** Descriptions:               全键盘控件
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 Zhang Wenbo
** Created date:               2015/08/04
** Descriptions:               
** 
*********************************************************************************************************/
#pragma once
#include "qwidget.h"
#include "ui_fullKey.h"

namespace Ui {
    class FullKey;
}

class FullKey : public QWidget
{
    Q_OBJECT

public:

    //!<  构造函数和析构函数
    FullKey(QWidget *parent = 0);
    ~FullKey();
signals:
    void btnClicked(const QString &text);
    void enterClicked(const QString &text);
public slots:
    void onSymbolClicked();
    void onCharBtnClicked(const QString &text);
    void onBacksapceClicked();
    void onEnterClicked();

private:
    Ui::fullKey *ui;
    QString m_labelText;
};

/*********************************************************************************************************
** End of file
*********************************************************************************************************/

