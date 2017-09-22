/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  QuickFieldWidgetFactory.cpp
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
#include <QVariant>
#include "IField.h"
#include "StringField.h"
#include "QuickFieldWidgetFactory.h"

QuickFieldWidgetFactory::QuickFieldWidgetFactory()
	: m_engine(Q_NULLPTR)
   , m_path(":/resources/")
{
}

QObject *QuickFieldWidgetFactory::createWidget(
        IField *field,
        Qt::Orientation /*direction*/,
        bool /*createLabel*/
        )
{
	if (m_engine != Q_NULLPTR && field != Q_NULLPTR)
	{
        return createControlFromField(field);
    }
	else 
	{
		return Q_NULLPTR;
    }
}


QObject* QuickFieldWidgetFactory::createContainer(
        int level,
        QLayout *parentLayout
        )
{
    Q_UNUSED(parentLayout);

    QStringList m_containerWidgetTypes = this->property("containerWidgetTypes").toStringList();
	if (m_engine != Q_NULLPTR && level >= 0
            && level < m_containerWidgetTypes.count())
    {
        QString containerType = m_containerWidgetTypes[level];
        if (containerType.contains(".qml")) 
		{
			return createItemFromQml(m_path + containerType);
        } 
		else
		{
            return createContainerFromName(containerType);
        }
    }

	return Q_NULLPTR;
}

QQmlEngine *QuickFieldWidgetFactory::engine() const
{
    return m_engine;
}

void QuickFieldWidgetFactory::setEngine(QQmlEngine *engine)
{
    m_engine = engine;
}

QString QuickFieldWidgetFactory::path() const
{
	return m_path;
}


void QuickFieldWidgetFactory::setPath(QString path)
{
	if (path != m_path)
	{
		m_path = path;
	}
}

QQuickItem* QuickFieldWidgetFactory::createItemFromQml(QString qml)
{
    QQmlComponent component(m_engine, QUrl::fromLocalFile(qml));
    QObject *item = component.create();
    if (item != nullptr)
    {
        return qobject_cast<QQuickItem *>(item);
    }
    else
    {
        qDebug() << "Failed to create item from " << qml;
        return Q_NULLPTR;
    }
    return NULL;
}

QQuickItem* QuickFieldWidgetFactory::createContainerFromName(QString name)
{
    QQmlComponent component(m_engine);
    QString data = QString("import QtQuick 2.7\nQtQuick.Layouts 1.1\n  %1{anchors:parent}")
            .arg(name);
    component.setData(data.toLatin1(),QUrl());
    return qobject_cast<QQuickItem *>(component.create());
}

QQuickItem* QuickFieldWidgetFactory::createControlFromName(QString name)
{
    QQmlComponent component(m_engine);
    QString data = QString("import QtQuick 2.7\nimport QtQuick.Controls 2.1\n  %1{}")
            .arg(name);
    component.setData(data.toLatin1(),QUrl());
    return qobject_cast<QQuickItem *>(component.create());
}

QQuickItem* QuickFieldWidgetFactory::createControlFromField(IField* field)
{
    QString editType = field->editorType();
	QQuickItem* item = Q_NULLPTR;
    if (!editType.isEmpty()) 
	{
		QString qml = m_path + editType;
		item = createItemFromQml(qml);
    }

	if (item != Q_NULLPTR)
	{
        item->setProperty("field",QVariant::fromValue<IField*>(field));
    }
    return item;
}


/*********************************************************************************************************
** End of file
*********************************************************************************************************/
