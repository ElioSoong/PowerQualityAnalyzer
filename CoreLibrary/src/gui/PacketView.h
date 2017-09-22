/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  PacketView.h
** Latest Version:             V1.0.0
** Latest modified Date:       2016/1/12
** Modified by:                
** Descriptions:              
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 Zhang Zimo
** Created date:               2016/1/12
** Descriptions:               
** 
*********************************************************************************************************/
#pragma once

#include "corelibrary_global.h"
#include <QTreeView>
#include <QString>
//#include "F280View.h"
#include "FieldLibrary/FieldModel.h"


class CORELIBRARY_EXPORT PacketView : public QTreeView
{
    Q_OBJECT

public:

     PacketView(QWidget * parent = 0);
     PacketView(const PacketView &packetView);
     virtual ~PacketView();

};


/*********************************************************************************************************
** End of file
*********************************************************************************************************/