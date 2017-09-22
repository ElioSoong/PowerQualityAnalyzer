/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  QuickFieldView.h
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

#ifndef QUICKFIELDVIEW_H
#define QUICKFIELDVIEW_H

#include <QQuickItem>
#include "FieldView.h"


class FIELDLIBRARY_EXPORT QuickFieldView : public FieldView
{
    Q_OBJECT
		Q_PROPERTY(QList<FieldView *> children READ children WRITE setChildren)

public:
    QuickFieldView();

    QList<FieldView *> children() const
    {
        return m_children;
    }
    void setChildren(const QList<FieldView *>& children)
    {
        m_children = children;
    }


    virtual QObject* createWidget() Q_DECL_OVERRIDE;

private:
    QList<FieldView*> m_children;
};

#endif // QUICKFIELDVIEW_H


/*********************************************************************************************************
** End of file
*********************************************************************************************************/
