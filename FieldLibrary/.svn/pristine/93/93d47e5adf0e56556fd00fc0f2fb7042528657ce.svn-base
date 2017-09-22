/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  StringField.cpp
** Latest Version:             V1.0.0
** Latest modified Date:       2017/2/16
** Modified by:                ZhangWenBo
** Descriptions:               
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 Chen Honghao
** Created date:               2016/04/13
** Descriptions:               字符串字段
** 
*********************************************************************************************************/
#include "StringField.h"


quint64 StringFieldIO::write(QIODevice* file, int dataTypeFlags /*= -1*/, void *data /*= NULL*/)
{
    Q_ASSERT(m_field);
    StringField *dataField = static_cast<StringField *>(m_field);
    Q_ASSERT(dataField);
    int fixedLength = dataField->fixedLength();
    int channelCount = dataField->channelCount();

    int countLenth = 0;
    for (int i = 0; i < channelCount; ++i)
    {
        QString theData = m_field->data(i, 0, FieldDataRole).toString();
        if (fixedLength > 0)
        {
            // 指定长度时，说明在读写数组，使用另一种完全不同的写入方法 [4/13/2016 CHENHONGHAO]
            QByteArray data = theData.toLocal8Bit();
            data.resize(fixedLength);
            file->write(data);
            continue;
        }

        // 获取QString的长度并将长度大小写入文件
        int length = theData.size();
        char *tempData1 = reinterpret_cast<char *>(&length);
        if (tempData1)
        {
            file->write(tempData1, sizeof(length));
        }

        // 获取QString字段内容，并将内容写入文件
        const QChar * dataStr = theData.constData();
        const char *tempData2 = reinterpret_cast<const char *>(dataStr);
        if (tempData2)
        {
            file->write(tempData2, sizeof(QChar) * length);
        }
        countLenth += length;
    }

    Q_UNUSED(data);
    Q_UNUSED(dataTypeFlags);

    return sizeof(QChar) * countLenth;
}

quint64 StringFieldIO::read(QIODevice* file, int dataTypeFlags /*= -1*/, void *data /*= NULL*/)
{
    Q_ASSERT(m_field);
    StringField *dataField = static_cast<StringField *>(m_field);
    Q_ASSERT(dataField);
    int fixedLength = dataField->fixedLength();
    int channelCount = dataField->channelCount();

    int countLenth = 0;
    for (int i = 0; i < channelCount; ++i)
    {
        if (fixedLength > 0)
        {
            QByteArray data = file->read(fixedLength);
            // 指定长度时，说明在读写数组，使用另一种完全不同的读取方法 [4/13/2016 CHENHONGHAO]
            QString theData = QString::fromLocal8Bit(data);
            m_field->setData(i, 0, theData, FieldDataRole);
            continue;
        }

        // 读取QString的长度
        int length = 0;
        char *tempData1 = reinterpret_cast<char *>(&length);
        if (tempData1)
        {
            file->read(tempData1, sizeof(length));

            QString theData;
            // 重新分配获取到的String长度
            theData.reserve(length);
            QChar * dataStr = theData.data();
            char *tempData2 = reinterpret_cast<char *>(dataStr);
            if (tempData2)
                file->read(tempData2, sizeof(QChar) * length);
            theData.resize(length);
            m_field->setData(i, 0, theData, FieldDataRole);
        }
        countLenth += length;
    }

    Q_UNUSED(data);
    Q_UNUSED(dataTypeFlags);

    return sizeof(QChar) * countLenth;
}

StringField::StringField() : StringFieldBase(), m_fixedLength(0)
{

}

QVariant StringField::data(int row, int column, int role)
{
    if (CustomChoiceTextsRole == role)
    {
        if (m_spec == NULL || m_spec->m_choices == NULL)
        {
            return QVariant();
        }

        QStringList *list = (QStringList*)m_spec->m_choices;
        return QVariant(*list);
    }
    return StringFieldBase::data(row, column, role);
}

/*!
 *  更新当前字段所对应的原始数据（RawData=>FieldTree）
 * @param[in]  basePtr   原始数据包的基地址（字段对应的原始数据的基地址=数据包基地址+字段全局数据偏移量）
 * @param[in]  channel   原始数据包对应的接线单元的序号，通常为0-7
 */
bool StringField::updateRawData(const void* basePtr, int channel)
{
    if (m_fixedLength <= 0)
    {
        // 目前尚不支持m_fixedLength = 0的自动长度字符串
        return StringFieldBase::updateRawData(basePtr, channel);
    }

    int offset = globalDataOffset();
    const char* d = reinterpret_cast<const char*>(reinterpret_cast<const char*>(basePtr) + offset);
    if (m_data == NULL || channel < 0 || channel >= m_channelCount)
        return false;

    QByteArray data(d, m_fixedLength);
    m_data[channel]->m_data = QString::fromLocal8Bit(data);
    return true;
}

/*! 
 *  提交当前字段所对应的原始数据（RawData=>FieldTree）
 * @param[in]  basePtr   原始数据包的基地址（字段对应的原始数据的基地址=数据包基地址+字段全局数据偏移量）
 * @param[in]  channel   原始数据包对应的接线单元的序号，通常为0-7
 */
bool StringField::commitRawData(void* basePtr, int channel) const
{
    if (m_fixedLength <= 0)
    {
        // 目前尚不支持m_fixedLength = 0的自动长度字符串
        return StringFieldBase::commitRawData(basePtr, channel);
    }

    int offset = globalDataOffset();
    char* d = reinterpret_cast<char*>(reinterpret_cast<char*>(basePtr) + offset);
    if (m_data == NULL || channel < 0 || channel >= m_channelCount)
        return false;

    QByteArray array = m_data[channel]->m_data.toLocal8Bit();
    const char* data = array.constData();
    memcpy(d, data, m_fixedLength);

    return true;
}

/*********************************************************************************************************
** End of file
*********************************************************************************************************/