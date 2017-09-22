/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  QuickFieldView.cpp
** Latest Version:             V1.0.0
** Latest modified Date:       2017/05/27
** Modified by:
** Descriptions:
**
**--------------------------------------------------------------------------------------------------------
** Created by:
** Created date:               2017/05/06
** Descriptions:               END
**
*********************************************************************************************************/

#include "QuickFieldView.h"
#include "QuickFieldWidgetFactory.h"

QuickFieldView::QuickFieldView()
    : m_children()
{

}

QObject *QuickFieldView::createWidget()
{
	QQuickItem* thisWidget = Q_NULLPTR;
    FieldWidgetFactory* factory = this->fieldWidgetFactory();
	IField* field = this->field();
	if (factory && field)
    {
        thisWidget = qobject_cast<QQuickItem*>(factory->createWidget(field));
    }

    if (!thisWidget) {
        return Q_NULLPTR;
    }

    thisWidget->setProperty("title", this->title());

    foreach (FieldView* child, m_children)
    {
        if (child)
        {
			field->addChild(child->field());
            QQuickItem* childWidget = qobject_cast<QQuickItem*>(child->createWidget());
            if (childWidget)
            {
				if (!QMetaObject::invokeMethod(
					thisWidget,
					"addItem",
					Q_ARG(QVariant,
					QVariant::fromValue<QQuickItem *>(childWidget))))
				{
					qDebug() << "Can't invoke the method 'addItem()'.";
				}
            }
        }
    }
    return thisWidget;
}


/*********************************************************************************************************
** End of file
*********************************************************************************************************/
