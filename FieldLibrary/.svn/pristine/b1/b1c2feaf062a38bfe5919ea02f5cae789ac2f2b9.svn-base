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
#ifndef FRACTIONFIELD_H
#define FRACTIONFIELD_H

#include "fieldlibrary_global.h"
#include "DataField.h"
#include "DataField_p.h"

/**
 *  @class Fraction
 *  @brief 分数
 */
class FIELDLIBRARY_EXPORT Fraction
{
public:
    Fraction(int n = 1, int d = 1) : m_numerator(n), m_denominator(d)
    {

    }

    Fraction(const Fraction& other)
    {
        m_numerator = other.m_numerator;
        m_denominator = other.m_denominator;
    }

    void setNumerator(int n)
    {
        m_numerator = n;
    }
    int  numerator(void) const
    {
        return m_numerator;
    }

    void setDenominator(int n)
    {
        m_denominator = n;
    }
    int  denominator(void) const
    {
        return m_denominator;
    }

    /*! 
     * 获取当前分数对应的小数/比例
     * @return     当前分数对应的小数/比例
     */
    float ratio(void) const
    {
        return (float(m_numerator) / float(m_denominator));
    }

    /*! 
     * 获取当前分数对应的显示内容，格式为A:B
     * @return     当前分数对应的显示内容，格式为A:B
     */
    QString text(void) const
    {
        QString text;
        text.sprintf("%4d : %4d", m_numerator, m_denominator);
        return text;
    }

    bool operator < (const Fraction& b)
    {
        return ratio() < b.ratio();
    }

    Fraction& operator + (const Fraction& b)
    {
        return *this;
    }

protected:
    int m_numerator;                        //!< 分子
    int m_denominator;                      //!< 分母
};
Q_DECLARE_METATYPE(Fraction)

/**
 *  @class FractionItem
 *  @brief 用于在xml中载入分数的帮助类
 */
class FIELDLIBRARY_EXPORT FractionItem : public QObject, public Fraction//, public AttributeClass
{
    Q_OBJECT
    Q_PROPERTY(int numerator   READ numerator   WRITE setNumerator)
    Q_PROPERTY(int denominator READ denominator WRITE setDenominator)
public:
    FractionItem(int n = 1, int d = 1) : Fraction(n, d)
    {

    }

    void  *realType()
    {
        return static_cast<Fraction*>(this);
    }
    
    QVariant realValue()
    {
       m_list.push_back(*static_cast<Fraction*>(this)) ; 
       return QVariant::fromValue< QList<Fraction> >(m_list);
    }    

    FractionItem(const FractionItem& other)
    {
        m_numerator = other.m_numerator;
        m_denominator = other.m_denominator;
    }
    QList<Fraction> m_list ;
};


/**
 *  @class FractionField
 *  @brief 分数字段
 */
class FIELDLIBRARY_EXPORT FractionField : public DataField<Fraction>
{
    Q_OBJECT
    DECLARE_DATA_FIELD_PROPERTIES(Fraction)

public:
    FractionField();

    /**
     *  @enum  FractionFieldRole
     *  @brief 分数字段的扩展角色
     */
    enum FractionFieldRole
    {
        NumeratorRole = EndOfIFieldRole+1,  //!< 分子角色
        DemoninatorRole,                    //!< 分母角色
    };

    FieldIO* createFieldIO() Q_DECL_OVERRIDE;

    /*! 
     * 获取字段的数据，通常仅被FieldModel的同名函数遍历调用
     * @param[in]  与FieldModel的形参含义相同，详见FieldModel
     *
     * @note       注意IField以及其子类的flags|data|setData三个方法中的row和column参数不是给FieldModel使用的，
     *             而是给其子Field（通常是SubDataField）使用的。当FieldModel调用这三个方法时，需要传入-1。
     */
    QVariant data(int row, int column, int role) Q_DECL_OVERRIDE
    {
        return DataField<Fraction>::data(row, column, role);
    }

protected:
    /*! 
     * 获取当前数据字段的实际数据，一般用于setupModel迅速获取数据或者用于调试
     * @param[in]  element 通道号，所有配置数据字段均将数据存储于第0通道
     * @return     当前数据字段的实际数据
     */
    QVariant data(int element = 0) const Q_DECL_OVERRIDE
    {
        Q_ASSERT_X(
            m_data != NULL && element < m_channelCount, 
            "DataField::data(int)", "Failed to get setup-data in DataField."
            );
        return (float)m_data[0]->m_data.ratio();
    }

    /*! 
     * 将字段对应的数据类型T的数据转换成字符串，本应为静态函数，考虑到不同子类实现不同，此处设置为虚函数
     * @param[out] stringOut 转换输出的字符串
     * @param[in]  dataIn    转换输入的数据
     * @return     转换成功返回TRUE
     */
    bool toString(const Fraction& dataIn, QString& stringOut, int curElement = -1) const Q_DECL_OVERRIDE;

    /*! 
     * 将字段对应的数据类型T的数据转换成字符串，本应为静态函数，考虑到不同子类实现不同，此处设置为虚函数
     * @param[in]  stringIn 转换输入的字符串
     * @param[out] dataOut  转换输出的数据
     * @return     转换成功返回TRUE
     */
    bool fromString(Fraction& dataOut, const QString& stringIn) const Q_DECL_OVERRIDE;

    /*! 
     * 获取子字段的数据
     * @param[in]  child      子字段的childIndex
     * @param[in]  row&column 与FieldModel的形参含义相同，详见FieldModel
     * @param[in]  ok         是否处理成功，处理失败必须将ok置为false，否则置为true或者不做处理
     */
    QVariant childData(int child, int row, int column, int role, bool& ok) Q_DECL_OVERRIDE;

    /*! 
    * 设置子字段的数据
    * @param[in]  child      子字段的childIndex
    * @param[in]  row&column 与FieldModel的形参含义相同，详见FieldModel
    * @param[in]  ok         是否处理成功，处理失败必须将ok置为false，否则置为true或者不做处理
    */
    bool setChildData(int child, int row, int column, const QVariant& newData, int role, bool& ok) Q_DECL_OVERRIDE;
};

/**
 *  @class ScalingField
 *  @brief 比例字段
 */
class FIELDLIBRARY_EXPORT ScalingField : public FractionField
{
    Q_OBJECT
public:
    ScalingField()
    {
        //addCustomItem(Fraction(1000, 1));
        //addCustomItem(Fraction(100 , 1));
        //addCustomItem(Fraction(10  , 1));
        //addCustomItem(Fraction(1   , 1));
    }
};

class FIELDLIBRARY_EXPORT FractionFieldIO : public FieldIO
{
public:
    FractionFieldIO(IField* field = NULL) :FieldIO(field)
    {

    }

public:
    /*!
    * 写数据到文件IO
    * @param[in]  file          打开的文件指针
    * @param[in]  dataTypeFlags 内容类型
    * return   成功返回写入数据数目
    */
    quint64 write(QIODevice* file, int dataTypeFlags = -1, void *data = NULL) Q_DECL_OVERRIDE;

    /*!
    * 从文件IO中读取数据
    * @param[in]  file          打开的文件指针
    * @param[in]  dataTypeFlags 内容类型
    * return   成功返回读出的数据数目
    */
    quint64 read(QIODevice* file, int dataTypeFlags = -1, void *data = NULL) Q_DECL_OVERRIDE;
};

#endif // FRACTIONFIELD_H
/*********************************************************************************************************
** End of file
*********************************************************************************************************/
