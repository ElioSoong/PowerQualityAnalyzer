/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  FieldWidgetFactory.h
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
#ifndef QUICKFIELDWIDGETFACTORY_H
#define QUICKFIELDWIDGETFACTORY_H

#include "FieldWidgetFactory.h"
#include <QQmlEngine>
#include <QQuickItem>
#include <QStringList>

class FIELDLIBRARY_EXPORT QuickFieldWidgetFactory : public FieldWidgetFactory
{
    Q_OBJECT
    Q_PROPERTY(QQmlEngine* engine READ engine WRITE setEngine FINAL)
	Q_PROPERTY(QString path READ path WRITE setPath FINAL)

public:
    QuickFieldWidgetFactory();

    virtual QObject* createWidget(
            IField* field,
            Qt::Orientation direction = Qt::Horizontal,
			bool createLabel = true) override;
    virtual QObject* createContainer(
            int level = 0,
			QLayout* parentLayout = Q_NULLPTR) override;

    QQmlEngine* engine() const;
    void setEngine(QQmlEngine* engine);
	QString path() const;
	void setPath(QString path);

private:
    QQuickItem* createItemFromQml(QString qml);
    QQuickItem* createContainerFromName(QString name);
    QQuickItem* createControlFromName(QString name);
    QQuickItem* createControlFromField(IField* field);

private:
    QQmlEngine *m_engine;
	QString m_path;
};

#endif // QUICKFIELDWIDGETFACTORY_H

/*********************************************************************************************************
** End of file
*********************************************************************************************************/

