/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  FractionField.h
** Latest Version:             V1.0.0
** Latest modified Date:       2015/09/15
** Modified by:                
** Descriptions:               
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 Chen Honghao
** Created date:               2015/09/15
** Descriptions:               分数字段及其子类
** 
*********************************************************************************************************/
#include "FractionField.h"

FractionField::FractionField()
{

}

FieldIO* FractionField::createFieldIO()
{
    FieldIO* theFieldIO = new FractionFieldIO(this);
    return theFieldIO;
}

/*!
 * 将字段对应的数据类型T的数据转换成字符串，本应为静态函数，考虑到不同子类实现不同，此处设置为虚函数
 * @param[out] stringOut 转换输出的字符串
 * @param[in]  dataIn    转换输入的数据
 * @return     转换成功返回TRUE
 */
bool FractionField::toString(const Fraction& dataIn, QString& stringOut, int curElement) const
{
    stringOut = dataIn.text();
    return true;
}

/*! 
 * 将字段对应的数据类型T的数据转换成字符串，本应为静态函数，考虑到不同子类实现不同，此处设置为虚函数
 * @param[in]  stringIn 转换输入的字符串
 * @param[out] dataOut  转换输出的数据
 * @return     转换成功返回TRUE
 */
bool FractionField::fromString(Fraction& dataOut, const QString& stringIn) const
{
    // 实现: 注：stringIn仅支持1:1格式的字符串
    dataOut = Fraction(stringIn.split(":").at(0).simplified().toInt(),stringIn.split(":").at(1).simplified().toInt());
    return true;
    //return false;
}

/*! 
 * 获取子字段的数据
 * @param[in]  child      子字段的childIndex
 * @param[in]  row&column 与FieldModel的形参含义相同，详见FieldModel
 * @param[in]  ok         是否处理成功，处理失败必须将ok置为false，否则置为true或者不做处理
 */
QVariant FractionField::childData(int child, int row, int column, int role, bool& ok)
{
    if (role > EndOfIFieldRole)
    {
        if (role == NumeratorRole)
        {
            return m_data[child]->m_data.numerator();
        }
        else if (role == DemoninatorRole)
        {
            return m_data[child]->m_data.denominator();
        }
        else
            return QVariant();
    }
    
    return DataField<Fraction>::childData(child, row, column, role, ok);
}

/*! 
 * 设置子字段的数据
 * @param[in]  child      子字段的childIndex
 * @param[in]  row&column 与FieldModel的形参含义相同，详见FieldModel
 * @param[in]  ok         是否处理成功，处理失败必须将ok置为false，否则置为true或者不做处理
 */
bool FractionField::setChildData(int child, int row, int column, const QVariant& newData, int role, bool& ok)
{
    if (role > EndOfIFieldRole)
    {
        if (role == DemoninatorRole)
        {
            m_data[child]->m_data.setDenominator(newData.toInt());
            return true;
        }
        else if (role == NumeratorRole)
        {
            m_data[child]->m_data.setNumerator(newData.toInt());
            return true;
        }
        else
            return false;
    }

    return DataField<Fraction>::setChildData(child, row, column, newData, role, ok);
}

/*********************************************************************************************************
** FractionFieldIO
*********************************************************************************************************/
quint64 FractionFieldIO::write(QIODevice* file, int dataTypeFlags /*= -1*/, void *data /*= NULL*/)
{
    Q_UNUSED(dataTypeFlags);
    Q_UNUSED(data);
    Q_ASSERT(m_field);
    FractionField *dataField = static_cast<FractionField *>(m_field);
    Q_ASSERT(dataField);
    int channelCount = dataField->channelCount();
    int countLenth = 0;
    for (int i = 0; i < channelCount; ++i)
    {
        int theNumerator = dataField->data(i, 0, FractionField::NumeratorRole).toInt();
        int theDemoninator = dataField->data(i, 0, FractionField::DemoninatorRole).toInt();
        countLenth += file->write((char *)(&theNumerator), sizeof(int));
        countLenth += file->write((char *)(&theDemoninator), sizeof(int));
    }
    return countLenth;
}

quint64 FractionFieldIO::read(QIODevice* file, int dataTypeFlags /*= -1*/, void *data /*= NULL*/)
{
    Q_UNUSED(dataTypeFlags);
    Q_UNUSED(data);
    Q_ASSERT(m_field);
    FractionField *dataField = static_cast<FractionField *>(m_field);
    Q_ASSERT(dataField);
    int channelCount = dataField->channelCount();
    int countLenth = 0;
    for (int i = 0; i < channelCount; ++i)
    {
        int theNumerator = 0;
        int theDemoninator = 0;
        countLenth += file->read((char *)(&theNumerator), sizeof(int));
        countLenth += file->read((char *)(&theDemoninator), sizeof(int));
        m_field->setData(i, 0, theNumerator, FractionField::NumeratorRole);
        m_field->setData(i, 0, theDemoninator, FractionField::DemoninatorRole);
    }
    return countLenth;
}

/*********************************************************************************************************
** End of file
*********************************************************************************************************/
