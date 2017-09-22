/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  QuickFieldItem.h
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
#ifndef QUICKFIELDITEM_H
#define QUICKFIELDITEM_H

#include <QQuickItem>
#include <QObject>
#include <QTimer>
#include "IField.h"

class FIELDLIBRARY_EXPORT QuickFieldItem : public QQuickItem
{
    Q_OBJECT
		Q_PROPERTY(QVariant field READ field WRITE setField NOTIFY fieldChanged FINAL)

public:
	QuickFieldItem(QQuickItem *parent = Q_NULLPTR);

	Q_INVOKABLE bool findField(QString path, QVariant rootField);
	Q_INVOKABLE void setField(QVariant field);
	Q_INVOKABLE QVariant field()const;

signals:
    void fieldChanged();

private:
	Q_DISABLE_COPY(QuickFieldItem);

    IField* m_field;
};

QML_DECLARE_TYPE(QuickFieldItem)
#endif // QUICKFIELDITEM_H

/*********************************************************************************************************
** End of file
*********************************************************************************************************/
