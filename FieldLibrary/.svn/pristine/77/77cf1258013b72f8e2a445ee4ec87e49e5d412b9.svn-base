/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  IntegerField.h
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
#ifndef INTEGERFIELDEX_H
#define INTEGERFIELDEX_H

#include "DataField.h"
#include "DataField_p.h"

DECLARE_DATA_FIELD_CLASS(bool,    BoolFieldBase)
DECLARE_DATA_FIELD_CLASS(int,     IntegerFieldBase)
DECLARE_DATA_FIELD_CLASS(quint64, LongIntegerFieldBase)


/**
 *  @class BoolField
 *  @brief 逻辑值字段，例如是否开启演示模式
 */
class FIELDLIBRARY_EXPORT BoolField : public BoolFieldBase
{
    Q_OBJECT

public:
    BoolField() {}


    /*! 
     * 将字段对应的数据类型T的数据转换成字符串，本应为静态函数，考虑到不同子类实现不同，此处设置为虚函数
     * @param[out] stringOut 转换输出的字符串
     * @param[in]  dataIn    转换输入的数据
     * @return     转换成功返回TRUE
     */
    bool toString(const bool& dataIn, QString& stringOut, int curChannel = -1) const Q_DECL_OVERRIDE;
    
    /*! 
     * 将字段对应的数据类型T的数据转换成字符串，本应为静态函数，考虑到不同子类实现不同，此处设置为虚函数
     * @param[in]  stringIn 转换输入的字符串
     * @param[out] dataOut  转换输出的数据
     * @return     转换成功返回TRUE
     */
    bool fromString(bool& dataOut, const QString& stringIn) const Q_DECL_OVERRIDE;
};

class FIELDLIBRARY_EXPORT IntegerFieldIO : public FieldIO
{
public:
    IntegerFieldIO(IField* field = NULL) :FieldIO(field)
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


/**
 *  @class IntegerField
 *  @brief 整数字段，例如串口波特率
 */
class FIELDLIBRARY_EXPORT IntegerField : public IntegerFieldBase
{
    Q_OBJECT
    Q_PROPERTY(int base READ base WRITE setBase)
    Q_PROPERTY(int bitWidth READ bitWidth WRITE setBitWidth)
    Q_PROPERTY(bool isUnsigned READ isUnsigned WRITE setUnsigned)

public:
    IntegerField();
    IntegerField(int base, int bitWidth, bool isUnsigned);
    IntegerField(const IntegerField& other);

    /*!  
     * Getter/Setter for bitWidth
     */
    int bitWidth() const 
    { 
        return m_bitWidth; 
    }
    void setBitWidth(int bitWidth) 
    { 
        m_bitWidth = bitWidth; 
    }

    /*!  
     * Getter/Setter for base
     */
    int base() const 
    { 
        return m_base; 
    }
    void setBase(int base) 
    { 
        m_base = base; 
    }

    /*!  
     * Getter/Setter for unsigned
     */
    bool isUnsigned() const 
    { 
        return m_unsigned; 
    }
    void setUnsigned(bool b) 
    { 
        m_unsigned = b; 
    }

protected:
    /*! 
     * 获取当前字段的大小，不包括子字段（例如，绝大部分VirtualField的大小为0，而RealField大小为4）
     * @return     当前字段的大小，单位：字节
     */
    int size() const Q_DECL_OVERRIDE
    {
        return m_bitWidth > 0 ? m_bitWidth/8 : sizeof(int);
    }

    FieldIO* createFieldIO() Q_DECL_OVERRIDE
    {
        FieldIO* theFieldIO = new IntegerFieldIO(this);
        return theFieldIO;
    }

    /*! 
     * 复位当前字段，主要指清除缓存操作，例如globalDataOffset
     */
    void reset() Q_DECL_OVERRIDE;

    /*! 
     * 将字段对应的数据类型T的数据转换成字符串，本应为静态函数，考虑到不同子类实现不同，此处设置为虚函数
     * @param[out] stringOut 转换输出的字符串
     * @param[in]  dataIn    转换输入的数据
     * @return     转换成功返回TRUE
     */
    bool toString(const int& dataIn, QString& stringOut, int curChannel = -1) const Q_DECL_OVERRIDE;
    
    /*! 
     * 将字段对应的数据类型T的数据转换成字符串，本应为静态函数，考虑到不同子类实现不同，此处设置为虚函数
     * @param[in]  stringIn 转换输入的字符串
     * @param[out] dataOut  转换输出的数据
     * @return     转换成功返回TRUE
     */
    bool fromString(int& dataOut, const QString& stringIn) const Q_DECL_OVERRIDE;

    /*! 
     *  更新当前字段所对应的原始数据（RawData=>FieldTree）
     * @param[in]  basePtr   原始数据包的基地址（字段对应的原始数据的基地址=数据包基地址+字段全局数据偏移量）
     * @param[in]  channel   原始数据包对应的接线单元的序号，通常为0-7
     */
    bool updateRawData(const void* basePtr, int channel) Q_DECL_OVERRIDE;

    /*! 
     *  提交当前字段所对应的原始数据（RawData=>FieldTree）
     * @param[in]  basePtr   原始数据包的基地址（字段对应的原始数据的基地址=数据包基地址+字段全局数据偏移量）
     * @param[in]  channel   原始数据包对应的接线单元的序号，通常为0-7
     */
    bool commitRawData(void* basePtr, int channel) const Q_DECL_OVERRIDE;

protected:
    int  m_base;                            //!< 进制（8/10/16），0代表自动
    int  m_bitWidth;                        //!< 位宽（8/16/32），默认为8
    bool m_unsigned;                        //!< 是否为无符号数，默认为false，但十六进制数默认为true
};

class U8Field : public IntegerField
{
public:
    U8Field() : IntegerField(10, 8, true) {}
};

class U16Field : public IntegerField
{
public:
    U16Field() : IntegerField(10, 16, true) {}
};

class U32Field : public IntegerField
{
public:
    U32Field() : IntegerField(10, 32, true) {}
};

class I8Field : public IntegerField
{
public:
    I8Field() : IntegerField(10, 8, false) {}
};

class I16Field : public IntegerField
{
public:
    I16Field() : IntegerField(10, 16, false) {}
};

class I32Field : public IntegerField
{
public:
    I32Field() : IntegerField(10, 32, false) {}
};

/**
 *  @class LongIntegerField
 *  @brief 长整数字段
 */
class FIELDLIBRARY_EXPORT LongIntegerField : public LongIntegerFieldBase
{
    Q_OBJECT

public:
    LongIntegerField();
};

#endif // INTEGERFIELDEX_H
/*********************************************************************************************************
** End of file
*********************************************************************************************************/

