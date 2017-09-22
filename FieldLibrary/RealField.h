/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  RealField.h
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
#ifndef REALFIELD_H
#define REALFIELD_H

#include "DataField.h"
#include "DataField_p.h"


DECLARE_DATA_FIELD_CLASS(float,   FloatField)

class FIELDLIBRARY_EXPORT RealField : public FloatField
{
    Q_OBJECT
        Q_PROPERTY(int wordWidth READ wordWidth WRITE setWordWidth)
public:
    /*! 
     * 保护的带参构造函数，用于子类快速初始化
     */
    RealField(
        float max = 1E9f, float min = -1E9f, float def = 0, float step = 1,
        const QString& dimension = "",
        int digitals = 3,
        Qt::Alignment alignment = Qt::AlignCenter
        );

    RealField(const RealField& other);

    ~RealField();

    /*!
    * 将字段对应的数据类型T的数据转换成字符串，本应为静态函数，考虑到不同子类实现不同，此处设置为虚函数
    * @param[in]  stringIn 转换输入的字符串
    * @param[out] dataOut  转换输出的数据
    * @return     转换成功返回TRUE
    */
    bool fromString(float& dataOut, const QString& stringIn) const Q_DECL_OVERRIDE;

    /*! 
     * 将字段对应的数据类型T的数据转换成字符串，本应为静态函数，考虑到不同子类实现不同，此处设置为虚函数
     * @param[out] stringOut 转换输出的字符串
     * @param[in]  dataIn    转换输入的数据
     * @return     转换成功返回TRUE
     */
    bool toString(const float& dataIn, QString& stringOut, int curChannel = -1) const Q_DECL_OVERRIDE;
    /** 
     * 电流超限，电流超限是单对单的关系（A通道只影响A通道）
     * @param[in] channel 通道号
     * @return 超限返回true
     */
    virtual bool ampereOutOfLimit(int channel) const;
    /** 
     * 电流超限，电流超限是单对多的关系（A/B/C/N通道只要有一个通道超限，则所有的都超限）
     * @return 超限返回true
     */
    virtual bool voltageOutOfLimit() const;
    int wordWidth() const { return m_wordWidth; }
    void setWordWidth(int val) { m_wordWidth = val; }
private:
    int     m_wordWidth;    //!< 有效位数
};

#endif // REALFIELD_H
/*********************************************************************************************************
** End of file
*********************************************************************************************************/
