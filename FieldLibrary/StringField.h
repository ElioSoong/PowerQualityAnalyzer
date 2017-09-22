/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  StringField.h
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
#pragma once
#include "DataField.h"
#include "DataField_p.h"


DECLARE_DATA_FIELD_CLASS(QString, StringFieldBase)


class FIELDLIBRARY_EXPORT StringFieldIO : public FieldIO
{
public:
    StringFieldIO(IField* field = NULL) :FieldIO(field)
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


class FIELDLIBRARY_EXPORT StringField : public StringFieldBase
{
    Q_OBJECT
    Q_PROPERTY(int fixedLength READ fixedLength WRITE setFixedLength)

public:
    StringField();

    StringField(const StringField& other) : StringFieldBase(other), m_fixedLength(other.m_fixedLength)
    {
    }

    /*!  
     * Getter/Setter for fixedLength
     */
    int fixedLength() const 
    { 
        return m_fixedLength; 
    }
    void setFixedLength(int fixedLength) 
    { 
        m_fixedLength = fixedLength; 
    }

protected:
    /*! 
     * 获取当前字段的大小，不包括子字段（例如，绝大部分VirtualField的大小为0，而RealField大小为4）
     * @return     当前字段的大小，单位：字节
     */
    int size() const Q_DECL_OVERRIDE
    {
        // FIXME: 针对非固定长度的字符串，目前将返回0，应修正此问题（不过在F280中未使用此特性）
        return m_fixedLength;
    }

    FieldIO* createFieldIO() Q_DECL_OVERRIDE
    {
        FieldIO* theFieldIO = new StringFieldIO(this);
        return theFieldIO;
    }

    QVariant data(int row, int column, int role);

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

    /*! 
     * 将字段对应的数据类型T的数据转换成字符串，本应为静态函数，考虑到不同子类实现不同，此处设置为虚函数
     * @param[out] stringOut 转换输出的字符串
     * @param[in]  dataIn    转换输入的数据
     * @return     转换成功返回TRUE
     */
    bool toString(const QString& dataIn, QString& stringOut, int curChannel = -1) const Q_DECL_OVERRIDE
    {
        Q_UNUSED(curChannel);
        stringOut = dataIn;
        return true;
    }

    /*! 
     * 将字段对应的数据类型T的数据转换成字符串，本应为静态函数，考虑到不同子类实现不同，此处设置为虚函数
     * @param[in]  stringIn 转换输入的字符串
     * @param[out] dataOut  转换输出的数据
     * @return     转换成功返回TRUE
     */
    bool fromString(QString& dataOut, const QString& stringIn) const Q_DECL_OVERRIDE
    {
        dataOut = stringIn;
        return true;
    }

protected:
    int m_fixedLength;                      //!< 字符串的固定长度，0表示自动，默认为0
                                            // （非零时说明实际上字符串是以char[n]数组的形式载入的）
};
/*********************************************************************************************************
** End of file
*********************************************************************************************************/