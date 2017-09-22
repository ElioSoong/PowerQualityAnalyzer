/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  EnumField.h
** Latest Version:             V1.0.0
** Latest modified Date:       2017/2/16
** Modified by:                ZhangWenBo
** Descriptions:               
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 Chen Honghao
** Created date:               2015/09/14
** Descriptions:               枚举类型的字段，通过扩展的EnumTypeRole来获取枚举的类型（仅支持预定的内容）
** 
*********************************************************************************************************/
#ifndef ENUMFIELD_H
#define ENUMFIELD_H

#include "fieldlibrary_global.h"
#include "IField.h"


/**
 *  @class EnumItem
 *  @brief 用于在xml中载入枚举字段的choices的帮助类
 */
class FIELDLIBRARY_EXPORT EnumItem : public QObject, public QPair<int, QString>
{
    Q_OBJECT
    Q_PROPERTY(int enumerator WRITE setEnum READ enumerator)
    Q_PROPERTY(QString texts WRITE setText READ texts)

public:
    EnumItem() {}
    explicit EnumItem(const EnumItem& a) : QPair<int, QString>(a) {}

public:
    void setEnum(int v)
    {
        first = v;
    }
    int enumerator() const
    {
        return first;
    }
    void setText(const QString& text)
    {
        second = text;
    }
    QString texts() const
    {
        return second;
    }
};

Q_DECLARE_METATYPE(EnumItem);
Q_DECLARE_METATYPE(QList<EnumItem *>)

class FIELDLIBRARY_EXPORT EnumFieldIO : public FieldIO
{
public:
    EnumFieldIO(IField* field = NULL) :FieldIO(field)
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
 *  @class EnumField
 *  @brief 枚举类型字段
 */
class FIELDLIBRARY_EXPORT EnumField : public IField
{
    Q_OBJECT
    Q_PROPERTY(QList<EnumItem *> choices READ choices WRITE setChoices);
    Q_PROPERTY(int default READ getDefault WRITE setDefault)
public:
   
    EnumField();

    /*! 
     * GET & SET
     */
    QList<EnumItem *> choices() const
    {
        return m_choices;
    }

    void setChoices(const QList<EnumItem *> & items)
    {
        m_choices = items;
    }

    int getDefault() const
    {
        return m_default;
    }

    void setDefault(int value)
    {
        m_default = value;
    }

    FieldIO* createFieldIO() Q_DECL_OVERRIDE
    {
        FieldIO* theFieldIO = new EnumFieldIO(this);
        return theFieldIO;
    }

    /*!
    * 将默认实现从true改为false
    */
    bool isVirtual() const Q_DECL_OVERRIDE
    {
        return false;
    }

    /*!
    * 获取当前字段的数据类型，如果该字段为virtual，则返回UnknownType
    */
    QMetaType::Type type() const Q_DECL_OVERRIDE
    {
        return QMetaType::User;
    }

    /*! 
     * 获取当前数据字段的实际数据，一般用于setupModel迅速获取数据或者用于调试
     * @param[in]  channel 通道号，所有配置数据字段均将数据存储于第0通道
     * @return     当前数据字段的实际数据
     */
    QVariant data(int channel = 0) const Q_DECL_OVERRIDE;

    /*! 
     * 获取字段的数据，通常仅被FieldModel的同名函数遍历调用
     * @param[in]  与FieldModel的形参含义相同，详见FieldModel
     *
     * @note       注意IField以及其子类的flags|data|setData三个方法中的row和column参数不是给FieldModel使用的，
     *             而是给其子Field（通常是SubDataField）使用的。当FieldModel调用这三个方法时，需要传入-1。
     */
    QVariant data(int row, int column, int role) Q_DECL_OVERRIDE;

    /*! 
     * 设置字段的数据，通常仅被FieldModel的同名函数遍历调用
     * @param[in]  与FieldModel的形参含义相同，详见FieldModel
     *
     * @note       注意IField以及其子类的flags|data|setData三个方法中的row和column参数不是给FieldModel使用的，
     *             而是给其子Field（通常是SubDataField）使用的。当FieldModel调用这三个方法时，需要传入-1。
     */
    bool setData(int row, int column, const QVariant& newData, int role) Q_DECL_OVERRIDE;

protected:

    /*! 
     * 根据枚举值（int），获取界面显示的枚举字串
     * @param[in]  枚举值（int）
     * @return     text 界面显示的（human readable）枚举字串
     */
    QString enumValueToText(int value);

    /*! 
     * 根据界面显示的枚举字串，获取枚举值（int）
     * @param[in]  text 界面显示的（human readable）枚举字串
     * @return     枚举值（int）
     */
    int enumTextToValue(const QString & text);

protected:
    QList<EnumItem *>       m_choices;
    int                     m_default;
};   
#endif // ENUMFIELD_H
