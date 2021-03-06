/****************************************Copyright (c)****************************************************
**
**                             INNO Instrument(China) Co.Ltd.
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  ContainerFieldView.cpp
** Latest Version:             V1.0.0
** Latest modified Date:       2017/1/18
** Modified by:                
** Descriptions:               
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 
** Created date:               2017/1/13
** Descriptions:               ContainerFieldView class  [Generated by VP-UML, IGT1.2]
** 
*********************************************************************************************************/
#include <QWidget>
#include <QQuickItem>
#include <QMetaObject>

#include "includes.h"
#include "ContainerFieldView.h"
#include "FieldView.h"
#include "FieldWidgetFactory.h"


/**
 * Constructor
 */
ContainerFieldView::ContainerFieldView()
{
    // TODO: Not yet implemented
}

QObject* ContainerFieldView::createWidget()
{
    static int containerLevel = 0;

    // Create widget for this
    QObject* thisWidget = NULL;
    FieldWidgetFactory* factory = this->fieldWidgetFactory();
    if (factory)
    {
        thisWidget = factory->createContainer(containerLevel);
    }
    IWidgetContainer* thisContainer = qobject_cast<IWidgetContainer*>(thisWidget);
    QQuickItem* quickItem = qobject_cast<QQuickItem*>(thisWidget);
    if (!thisContainer && !quickItem)
    {
        return NULL;
    }

    ++containerLevel;
    foreach (FieldView* child, m_children)
    {
        if (child)
        {
            QObject* widgetObj = child->createWidget();
            QWidget* childWidget = qobject_cast<QWidget*>(widgetObj);
            if (thisContainer && childWidget)
            {
                thisContainer->addWidget(childWidget);
            }
            QQuickItem* childItem = qobject_cast<QQuickItem*>(widgetObj);
            if (quickItem && childItem)
            {
                if (!QMetaObject::invokeMethod(
                    quickItem,
                    "addItem",
                    Q_ARG(QVariant,
                    QVariant::fromValue<QQuickItem *>(childItem))))
                {
                    qDebug() << "Can't to invoke the method 'addItem()'.";
                }
            }
        }
    }
    --containerLevel;

    IField* field = this->field();
    if (thisContainer && field)
    {
        thisContainer->setTitle(field->text());
    }


    return thisWidget;
}

/*********************************************************************************************************
** End of file
*********************************************************************************************************/
