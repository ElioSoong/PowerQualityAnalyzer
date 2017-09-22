/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  EnumField.cpp
** Latest Version:             V1.0.0
** Latest modified Date:       2017/2/16
** Modified by:                ZhangWenBo
** Descriptions:               
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 Zhang WenBo
** Created date:               2017/02/14
** Descriptions:               END
** 
*********************************************************************************************************/
#include "EnumField.h"
#include <QDebug>


quint64 EnumFieldIO::write(QIODevice* file, int dataTypeFlags /*= -1*/, void *data /*= NULL*/)
{
    Q_UNUSED(data);
    Q_UNUSED(dataTypeFlags);
    Q_ASSERT(m_field);
    EnumField *dataField = static_cast<EnumField *>(m_field);
    Q_ASSERT(dataField);
    int countLenth = 0;
    int theData = dataField->data(0, 0, FieldDataRole).toInt();
    countLenth += file->write((char *)(&theData), sizeof(int));
    return countLenth;
}

quint64 EnumFieldIO::read(QIODevice* file, int dataTypeFlags /*= -1*/, void *data /*= NULL*/)
{
    Q_UNUSED(data);
    Q_UNUSED(dataTypeFlags);
    Q_ASSERT(m_field);
    EnumField *dataField = static_cast<EnumField *>(m_field);
    Q_ASSERT(dataField);
    int countLenth = 0;
    int theData = 0;
    countLenth += file->read((char *)(&theData), sizeof(int));
    m_field->setData(0, 0, theData, FieldDataRole);
    return countLenth;
}

EnumField::EnumField()
{

}

QVariant EnumField::data(int row, int column, int role)
{
    switch (role)
    {
    case IsCustomChoicesRole:
        return !m_choices.isEmpty();
    case CustomChoicesRole:
        return QVariant::fromValue<QList<EnumItem *> >(m_choices);
    case CustomChoiceTextsRole:
    {
        QStringList choicesList;
        foreach(EnumItem* item, m_choices)
        {
            choicesList.append(item->texts());
        }
        return choicesList;
    }
    case IsInvalidRole:
        // 执行合法性检测
        return !isValid();
    case Qt::DisplayRole:
    case FieldDataTextRole:
    case Qt::EditRole:
        return enumValueToText(m_default);
    case FieldDataRole:
        return m_default;
    default:
        break;
    }
    return IField::data(row, column, role);
}

QVariant EnumField::data(int element /*= 0*/) const 
{
    Q_UNUSED(element);
    return m_default;
}

bool EnumField::setData(int row, int column, const QVariant& newData, int role)
{
    switch (role)
    {
    case CustomChoicesRole:
        m_choices = newData.value<QList<EnumItem *> >();
        return true;
    case Qt::DisplayRole:
    case FieldDataTextRole:
    case Qt::EditRole:
        m_default = enumTextToValue(newData.toString());
        return true;
    case FieldDataRole:
        m_default = newData.toInt();
        return true;
    default:
        break;
    }

    return IField::setData(row, column, newData, role);
};

QString EnumField::enumValueToText(int value)
{
    QString text;
    foreach(EnumItem *item, m_choices)
    {
        if (value == item->enumerator())
        {
            text = item->texts();
            break;
        }
    }
    if (text.isEmpty())
    {
        qDebug() << "enumValueToText Error: Can not find the item!!";
    }
    return text;
}

int EnumField::enumTextToValue(const QString & text)
{
    int val = -1;
    foreach(EnumItem *item, m_choices)
    {
        if (text == item->texts())
        {
            val = item->enumerator();
            break;
        }
    }
    if (val < 0)
    {
        qDebug() << "enumTextToValue Error: Can not find the item!!";
    }
    return val;
}

/*********************************************************************************************************
** End of file
*********************************************************************************************************/
