/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  IntegerField.cpp
** Latest Version:             V1.0.0
** Latest modified Date:       2017/2/16
** Modified by:                ZhangWenBo
** Descriptions:               
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 Chen Honghao
** Created date:               2015/08/18
** Descriptions:               整数/长整数字段的各个子类，PqaTimeField
** 
*********************************************************************************************************/
#include "IntegerField.h"

/*! 
 * 将字段对应的数据类型T的数据转换成字符串，本应为静态函数，考虑到不同子类实现不同，此处设置为虚函数
 * @param[out] stringOut 转换输出的字符串
 * @param[in]  dataIn    转换输入的数据
 * @return     转换成功返回TRUE
 */
bool BoolField::toString(const bool& dataIn, QString& stringOut, int curChannel) const
{
    Q_UNUSED(curChannel);
    if (dataIn)
    {
        stringOut = "ON";
    }
    else
    {
        stringOut = "OFF";
    }

    return true;
}

/*! 
 * 将字段对应的数据类型T的数据转换成字符串，本应为静态函数，考虑到不同子类实现不同，此处设置为虚函数
 * @param[in]  stringIn 转换输入的字符串
 * @param[out] dataOut  转换输出的数据
 * @return     转换成功返回TRUE
 */
bool BoolField::fromString(bool& dataOut, const QString& stringIn) const
{
    if (stringIn == QLatin1String("ON") || stringIn == QLatin1String("TRUE")) 
    {
        dataOut = true;
        return true;
    }
    else 
    {
        dataOut = false;
        return true;
    }
}


quint64 IntegerFieldIO::write(QIODevice* file, int dataTypeFlags /*= -1*/, void *data /*= NULL*/)
{
    Q_ASSERT(m_field);
    IntegerField *dataField = static_cast<IntegerField *>(m_field);
    Q_ASSERT(dataField);
    int bitWidth = dataField->bitWidth();
    int channelCount = dataField->channelCount();
    int length = 0;
    for (int i = 0; i < channelCount; i++)
    {
        int theData = m_field->data(i, 0, FieldDataRole).toInt();
        length += file->write((char*)&theData, bitWidth / 8);
    }

    Q_UNUSED(data);
    Q_UNUSED(dataTypeFlags);

    return length;
}

quint64 IntegerFieldIO::read(QIODevice* file, int dataTypeFlags /*= -1*/, void *data /*= NULL*/)
{
    Q_ASSERT(m_field);
    IntegerField *dataField = static_cast<IntegerField *>(m_field);
    Q_ASSERT(dataField);
    int bitWidth = dataField->bitWidth();
    int channelCount = dataField->channelCount();
    int length = 0;
    for (int i = 0; i < channelCount; i++)
    {
        int theData = 0;
        length += file->read((char*)&theData, bitWidth / 8);
        if (!dataField->isUnsigned())
        {
            // 符号扩展移位
            if (theData & (1u << (bitWidth - 1)))
            {
                theData |= ~((1u << bitWidth) - 1);
            }
        }
        m_field->setData(i, 0, theData, FieldDataRole);
    }

    Q_UNUSED(data);
    Q_UNUSED(dataTypeFlags);

    return length;
}


IntegerField::IntegerField() : IntegerFieldBase(), m_base(10), m_bitWidth(sizeof(int)*8), m_unsigned(false)
{
    Q_ASSERT(m_spec);
    m_spec->m_step = 1;
    m_spec->m_default = 0;
}

IntegerField::IntegerField(int base, int bitWidth, bool isUnsigned) : 
    IntegerFieldBase(), 
    m_base(base), 
    m_bitWidth(bitWidth), 
    m_unsigned(isUnsigned)
{

}

IntegerField::IntegerField(const IntegerField& other) : 
    IntegerFieldBase(other), 
    m_base(other.m_base), 
    m_bitWidth(other.m_bitWidth),
    m_unsigned(other.m_unsigned)
{

}



/*! 
 * 复位当前字段，主要指清除缓存操作，例如globalDataOffset
 */
void IntegerField::reset()
{
    IntegerFieldBase::reset();
}

/*! 
 * 将字段对应的数据类型T的数据转换成字符串，本应为静态函数，考虑到不同子类实现不同，此处设置为虚函数
 * @param[out] stringOut 转换输出的字符串
 * @param[in]  dataIn    转换输入的数据
 * @return     转换成功返回TRUE
 */
bool IntegerField::toString(const int& dataIn, QString& stringOut, int curChannel) const
{
    Q_UNUSED(curChannel);
    if (m_base == 0 && m_bitWidth == 32)
    {
        // 直接通过QVariant转换即可
        return IntegerFieldBase::toString(dataIn, stringOut);
    }

    if (m_base == 16) 
    {
        // 十六进制数需要填充以及前导的0x
        stringOut = QString("0x%1").arg(dataIn, m_bitWidth/4, m_base, QChar('0'));
    } 
    else if (m_unsigned)
    {
        stringOut = QString("%1").arg((uint)dataIn, 0, m_base, QChar('0'));
    }
    else
    {
        stringOut = QString("%1").arg((int)dataIn, 0, m_base, QChar('0'));
    }

    return true;
}

/*! 
 * 将字段对应的数据类型T的数据转换成字符串，本应为静态函数，考虑到不同子类实现不同，此处设置为虚函数
 * @param[in]  stringIn 转换输入的字符串
 * @param[out] dataOut  转换输出的数据
 * @return     转换成功返回TRUE
 */
bool IntegerField::fromString(int& dataOut, const QString& stringIn) const
{
    bool ok = false;
    dataOut = stringIn.trimmed().toInt(&ok, m_base);
    // 负数截断
    if (m_unsigned && dataOut < 0) 
    {
        dataOut = 0;
    }
    return ok;
}

/*! 
 *  更新当前字段所对应的原始数据（RawData=>FieldTree）
 * @param[in]  basePtr   原始数据包的基地址（字段对应的原始数据的基地址=数据包基地址+字段全局数据偏移量）
 * @param[in]  channel   原始数据包对应的接线单元的序号，通常为0-7
 */
bool IntegerField::updateRawData(const void* basePtr, int channel)
{
    if (!IntegerFieldBase::updateRawData(basePtr, channel))
    {
        return false;
    }

    // 截断
    if (m_bitWidth > 0 && m_bitWidth < sizeof(int)*8)
    {
        for (int i = 0; i < m_channelCount; i++)
        {
            m_data[i]->m_data &= ((1u << m_bitWidth) - 1);
            if (!m_unsigned)
            {
                // 符号扩展移位
                if (m_data[i]->m_data & (1u << (m_bitWidth-1)))
                {
                    m_data[i]->m_data |= ~((1u << m_bitWidth) - 1);
                }
            }
        }
    }

    return true;
}

/*! 
 *  提交当前字段所对应的原始数据（RawData=>FieldTree）
 * @param[in]  basePtr   原始数据包的基地址（字段对应的原始数据的基地址=数据包基地址+字段全局数据偏移量）
 * @param[in]  channel   原始数据包对应的接线单元的序号，通常为0-7
 */
bool IntegerField::commitRawData(void* basePtr, int channel) const
{
    if (!IntegerFieldBase::commitRawData(basePtr, channel)) 
    {
        return false;
    }

    return true;
}

LongIntegerField::LongIntegerField()
{
    Q_ASSERT(m_spec);
    m_spec->m_default = 0;
}

/*********************************************************************************************************
** End of file
*********************************************************************************************************/