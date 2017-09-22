/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  RealField.cpp
** Latest Version:             V1.0.0
** Latest modified Date:       2017/2/16
** Modified by:                ZhangWenBo
** Descriptions:               实数字段及其各个子类，例如VoltageField/CurrentField
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 Chen Honghao
** Created date:               2015/08/14
** Descriptions:               
** 
*********************************************************************************************************/
#include "RealField.h"
#include <math.h>
#include <float.h>
#include <qmath.h>


class ScaledReal
{
    typedef QList< QPair<QChar, float> > ScalarList;

    QChar m_scalar;
    float m_scalarRatio;
    float m_scaledValue;

public:
    ScaledReal() : m_scalar(' '), m_scalarRatio(1.0f), m_scaledValue(0)
    {

    }

    ScaledReal(const QString& scaledRealString) : m_scalar(' '), m_scalarRatio(1.0f), m_scaledValue(0)
    {
        setString(scaledRealString);
    }

    ScaledReal(float rawReal) : m_scalar(' '), m_scalarRatio(1.0f), m_scaledValue(0)
    {
        setReal(rawReal);
    }

    float toFloat() const
    {
        return m_scalarRatio * m_scaledValue;
    }

    QChar scalar() const
    {
        return m_scalar;
    }

    float scaledValue() const
    {
        return m_scaledValue;
    }

    bool setString(const QString& scaledRealString)
    {
        const ScalarList& list = scalarList();
        QString str  = scaledRealString;
        bool    ok   = false;
        int   length = str.length();
        QChar scalar = str.at(length - 1);

        if (scalar.isDigit() || scalar == ' ')
        {
            m_scalar = ' ';
            m_scalarRatio = 1.0f;
            ok = true;
        }
        else
        {
            for (ScalarList::const_iterator it = list.cbegin(); it != list.cend(); it++)
            {
                if (it->first == scalar)
                {
                    m_scalar = it->first;
                    m_scalarRatio = it->second;
                    str = scaledRealString.left(length - 1).trimmed();
                    ok = true;
                    break;
                }
            }
        }

        if (ok)
        {
            float   v = str.toFloat(&ok);
            m_scaledValue = v;
        }

        return ok;
    }

    bool setReal(float f, int sr = -1)
    {
        m_scalar = ' ';

        if (sr != -1)
        {
            float ratio = pow(10.0f, sr);
            f /= ratio;
        }
        else
        {
            const ScalarList& list = scalarList();
            for (ScalarList::const_iterator it = list.cbegin(); it != list.cend(); it++)
            {
                m_scalarRatio = it->second;
                if (0 == fmod(f,m_scalarRatio))
                {
                    f /= m_scalarRatio;
                    m_scalar = it->first;
                    break;
                }
            }
        }

        m_scaledValue = f;
        return true;
    }

protected:
    static const ScalarList& scalarList()
    {
        static ScalarList list;
        if (list.size() == 0)
        {
            list 
                //<< QPair<QChar, float>(QChar('μ'), 1E-6f)
                << QPair<QChar, float>(QChar('m'), 1E-3f)
                << QPair<QChar, float>(QChar('k'), 1E+3f)
                << QPair<QChar, float>(QChar('M'), 1E+6f)
                << QPair<QChar, float>(QChar('G'), 1E+9f)
                << QPair<QChar, float>(QChar('T'), 1E+12f);
        }

        return list;
    }
};



RealField::RealField(
    float max, float min, float f_default, float step,
    const QString& dimension,
    int digitals, 
    Qt::Alignment alignment
    ) 
{
    setDefault(f_default);
    setStep(step);
    setMin(min);
    setMax(max);
    setDigitals(digitals);
    setDimension(dimension);
    setAlignment(alignment);
    setWordWidth(1024);
    //reset();
}

RealField::RealField(const RealField& other) : 
    FloatField(other)
{

}

RealField::~RealField()
{

}

/*!
* 将字段对应的数据类型T的数据转换成字符串，本应为静态函数，考虑到不同子类实现不同，此处设置为虚函数
* @param[in]  stringIn 转换输入的字符串
* @param[out] dataOut  转换输出的数据
* @return     转换成功返回TRUE
*/
bool RealField::fromString(float& dataOut, const QString& stringIn) const
{
    if (m_spec == NULL)
        return false;

    if (stringIn == "N/A" || stringIn == "nan")
    {
        dataOut = qQNaN();
        return true;
    }

    // 记录初始版本
    QString str = stringIn;

    // 去掉量纲（V/A/Hz/...）
    if (!m_spec->m_dimension.isEmpty())
    {
        int dimensionOffset = str.lastIndexOf(m_spec->m_dimension);
        if (dimensionOffset >= 0)
        {
            str = str.left(dimensionOffset).trimmed();
        }
    }
    if (str.length() <= 0)
        return false;

    // 去掉单位（u/m/k/M）
    ScaledReal sr;
    if (!sr.setString(str))
        return false;

    // 乘以单位
    dataOut = sr.toFloat();

    return true;
}

/*! 
 * 将字段对应的数据类型T的数据转换成字符串，本应为静态函数，考虑到不同子类实现不同，此处设置为虚函数
 * @param[out] stringOut 转换输出的字符串
 * @param[in]  dataIn    转换输入的数据
 * @return     转换成功返回TRUE
 */
bool RealField::toString(const float& dataIn, QString& stringOut, int curChannel) const
{
    if (m_spec == NULL)
        return false;
    int   digitals = m_spec->m_digitals;

    if (qIsNaN(dataIn))
    {
        return true;
    }


    QString format;
    QString result;
    ScaledReal sr;
    sr.setReal(dataIn, m_spec->m_unit);
    float scaledValue = sr.scaledValue();

    // 三种超限：字符长度超限，通道超限，自定义上下限超限 [6/20/2016 CHENHONGHAO]
    // TODO: 通道超限尚未实现 [6/20/2016 CHENHONGHAO]
    float scaledValueAbs = fabs(scaledValue);

    if (name().contains("VCF") || name().contains("ACF"))
    {
        m_spec->m_max = qSqrt(3);
    }
    if (dataIn > m_spec->m_max || dataIn < m_spec->m_min || scaledValueAbs >= 1E5f)
#if !defined(NDEBUG) && 0
    {
        format.sprintf("%%.%de", digitals);
    }
#else
    {
        // 发型版本需要开启OL显示功能
        stringOut = "OL";
        return true;
    }
#endif
    else if (scaledValueAbs < 1E-4f)
#if !defined(NDEBUG) && 0
    {
        format.sprintf("%%.%de", digitals);
    }
#else
    {
        // 发型版本需要开启消零功能
        scaledValue = 0;
        format.sprintf("%%.%df", digitals);
    }
#endif
    else
    {
        format.sprintf("%%.%df", digitals);
    }
    result.sprintf(format.toLatin1(), scaledValue);

    // 测试功能：如果显示长度超出了xxxx.xxx，则裁减 [6/20/2016 CHENHONGHAO]
    int lengthOfResult = result.length();
    if (lengthOfResult > 8)
    {
        int indexOfDot = result.indexOf(QLatin1Char('.'));
        int acceptableLength = qMax(indexOfDot, 8);
        result = result.left(acceptableLength);
    }
    stringOut = result;

    if (name().contains("DPF") || name().contains("Cos") || name().contains("Hpol"))
    {
        if (voltageOutOfLimit())
        {
            stringOut = "_.__";
        }
        else if (ampereOutOfLimit(curChannel))
        {
            stringOut = "_.__";
        }
    }
    return true;
}

bool RealField::ampereOutOfLimit(int channel) const
{
    Q_UNUSED(channel);
    return false;
}

bool RealField::voltageOutOfLimit() const
{
    return false;
}

/*********************************************************************************************************
** End of file
*********************************************************************************************************/
