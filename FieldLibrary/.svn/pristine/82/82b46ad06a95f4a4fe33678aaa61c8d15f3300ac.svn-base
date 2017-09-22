/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  QuickFieldItem.cpp
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

#include "QuickFieldItem.h"
#include <QDebug>

QuickFieldItem::QuickFieldItem(QQuickItem *parent)
    : QQuickItem(parent)
	, m_field(Q_NULLPTR)
{
}

bool QuickFieldItem::findField(QString path, QVariant rootField)
{
	IField* root = rootField.value<IField*>();
    int n = path.indexOf('/');
    QString name = path.left(n);
    if (root->name() == name) {
        m_field = root;
        emit fieldChanged();
        return true;
    } else {
        QList<IField *>childen = root->children();
        for(int index=1; index<childen.count(); ++index) {
            QString childPath = path.right(n+1);
			if (findField(childPath, QVariant::fromValue<IField*>(childen.at(index)))) {
                return true;
            }
        }
        return false;
    }
}

void QuickFieldItem::setField(QVariant field)
{
	IField* point = field.value<IField*>();
	if (m_field != point) {
		m_field = point;
        emit fieldChanged();
    }
}

QVariant QuickFieldItem::field()const
{
	return QVariant::fromValue<IField*>(m_field);
}

/*********************************************************************************************************
** End of file
*********************************************************************************************************/
