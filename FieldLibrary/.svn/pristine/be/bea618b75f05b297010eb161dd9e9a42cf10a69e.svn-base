/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  VirtualField.h
** Latest Version:             V1.0.0
** Latest modified Date:       2017/2/16
** Modified by:                ZhangWenBo
** Descriptions:               虚拟字段及其子类
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 Han ZhenFei
** Created date:               2015/06/25
** Descriptions:               
** 
*********************************************************************************************************/
#pragma once

#include "IField.h"

/**
 *  @class VirtualField
 *  @brief 虚拟字段，通常用于维护字段树结构，相当于文件系统的“文件夹”
 */
class FIELDLIBRARY_EXPORT VirtualField : public IField
{
    Q_OBJECT

public:
    VirtualField() : IField()
    {
    }

    VirtualField(const QString& name, IField* parent = NULL)
    {
        setName(name);
        //setParent(parent);
        m_parent = parent;
    }

    /*! 
     * 获取当前字段的大小，不包括子字段（例如，绝大部分VirtualField的大小为0，而RealField大小为4）
     * @return     当前字段的大小，单位：字节
     */
    int size() const Q_DECL_OVERRIDE;

    /*! 
     * 在此方法中复位趋势、缩略图等大型缓冲区，分配内存，并初始化
     */
    void resetBuffer(void* data = NULL) Q_DECL_OVERRIDE;
};

class FIELDLIBRARY_EXPORT ReservedField : public IField
{
    Q_OBJECT
    Q_PROPERTY(int size MEMBER m_size)

public:
    ReservedField(int size = sizeof(int)) : IField(), m_size(size)
    {

    }

    /*! 
     * 获取当前字段的大小，不包括子字段（例如，绝大部分VirtualField的大小为0，而RealField大小为4）
     * @return     当前字段的大小，单位：字节
     */
    int size() const Q_DECL_OVERRIDE
    {
        return m_size;
    }

    void setSize(int size)
    {
        m_size = size;
    }

protected:
    int m_size;
};

/**
 *  @class DirectStructField
 *  @brief 无间接引用（指针or直接指定偏移量）的结构体字段的基类
 */
class FIELDLIBRARY_EXPORT DirectStructField : public VirtualField
{
    Q_OBJECT

public:
    DirectStructField()
    {

    }
};

/**
 *  @class IndirectStructField
 *  @brief 有间接引用（指针or直接指定偏移量）的结构体字段的基类
 */
class FIELDLIBRARY_EXPORT IndirectStructField : public VirtualField
{
    Q_OBJECT

public:
    IndirectStructField()
    {

    }
};

/**
 *  @class ArrayField
 *  @brief 数组字段，支持结构体数组
 */
class FIELDLIBRARY_EXPORT ArrayField : public DirectStructField
{
    Q_OBJECT
    Q_PROPERTY(QObject* size READ sizeField WRITE setSizeField)

public:
    ArrayField();

    /*! 
     * 获取数组每一个元素的大小
     * @return     数组每一个元素的大小，单位：字节
     */
    virtual int channelSize() const;

    /*! 
     * 获取当前字段的大小，不包括子字段（例如，绝大部分VirtualField的大小为0，而RealField大小为4）
     * @return     当前字段的大小，单位：字节
     */
    virtual int size() const
    {
        return channelSize() * childCount();
    }

    /*! 
     * 获取指定当前数组的大小的字段
     */
    IField* sizeField() const
    {
        return m_sizeField;
    }

    /*! 
     * 设置指定当前数组的大小的字段
     */
    void setSizeField(QObject* field)
    {
        m_sizeField = static_cast<IField*>(field);
    }

    /*! 
     * 根据子字段的名称获取子字段对象
     * @param[in]  name 子字段的名称
     * @return     子字段对象
     */
    virtual IField* child(const QString& name) const;

    /*! 
     * 获取子字段个数
     * @return 子字段个数
     */
    virtual int childCount() const;

    /*! 
     *  更新当前字段所对应的原始数据（RawData=>FieldTree）
     * @param[in]  basePtr   原始数据包的基地址（字段对应的原始数据的基地址=数据包基地址+字段全局数据偏移量）
     * @param[in]  channel   原始数据包对应的接线单元的序号，通常为0-7
     */
    virtual bool updateRawData(const void* basePtr, int channel);

    /*! 
     *  提交当前字段所对应的原始数据（RawData=>FieldTree）
     * @param[in]  basePtr   原始数据包的基地址（字段对应的原始数据的基地址=数据包基地址+字段全局数据偏移量）
     * @param[in]  channel   原始数据包对应的接线单元的序号，通常为0-7
     */
    virtual bool commitRawData(void* basePtr, int channel) const;

protected:
    IField* m_sizeField;                    //!< 指定当前数组的大小的字段，实际上必然为IntegerField
};

class FIELDLIBRARY_EXPORT PointerField : public IndirectStructField
{
    Q_OBJECT

public:
    PointerField()
    {

    }

    /*! 
     * 计算间接引用的地址的偏移量
     */
    virtual int childDataOffset() const
    {
        // TODO: 根据当前字段的位置与与当前字段指向的指针的偏差计算childDataOffset
        return 0;
    }
};

class FIELDLIBRARY_EXPORT SymlinkField : public VirtualField
{
    Q_OBJECT
    Q_PROPERTY(QString linkPath READ linkPath WRITE setLinkPath)

public:
    SymlinkField() : VirtualField(), m_linkPath(), m_linkField(NULL)
    {

    }

    SymlinkField(const SymlinkField& other) :
        VirtualField(other),
        m_linkField(other.m_linkField),
        m_linkPath(other.m_linkPath)
    {

    }

    QString linkPath(void) const
    {
        return m_linkPath;
    }

    bool setLinkPath(const QString& linkPath)
    {
        m_linkPath = linkPath;

        return linkField() != NULL;
    }

    virtual IField* linkField(void) const
    {
        if (m_linkField)
            return m_linkField;

        m_linkField = IField::at(m_linkPath);
        return m_linkField;
    }

    FieldIO* createFieldIO() Q_DECL_OVERRIDE;

    /*!
    * 复位当前字段，主要指清除缓存操作，例如globalDataOffset
    */
    void reset() Q_DECL_OVERRIDE
    {
        m_linkField = NULL;
        return IField::reset();
    }

    void resetBuffer(void* data = NULL) Q_DECL_OVERRIDE
    {
        IField* field = linkField();
        Q_ASSERT_X(field != NULL, "SymlinkField::linkField()", m_linkPath.toLocal8Bit().data());
        return field->resetBuffer(data);
    }

    /*!
    * 将默认实现从true改为false
    */
    bool isVirtual() const Q_DECL_OVERRIDE
    {
#if 0
        IField* field = linkField();
        Q_ASSERT_X(field != NULL, "SymlinkField::linkField()", m_linkPath.toLocal8Bit().data());
        return field->isVirtual();
#endif
        return false;
    }

    /*!
    * 字段本身的数据行数（不包括子字段，用于某些表格类字段的扩展，默认返回0）
    * @return     字段本身的数据行数
    */
    int rowCount() const Q_DECL_OVERRIDE
    {
        IField* field = linkField();
        Q_ASSERT_X(field != NULL, "SymlinkField::linkField()", m_linkPath.toLocal8Bit().data());
        return field->rowCount();
    }

    /*!
    * 字段本身的数据列数（不包括子字段，用于某些表格类字段的扩展，默认返回0）
    * @return     字段本身的数据列数
    */
    int columnCount() const Q_DECL_OVERRIDE
    {
        IField* field = linkField();
        Q_ASSERT_X(field != NULL, "SymlinkField::linkField()", m_linkPath.toLocal8Bit().data());
        return field->columnCount();
    }

    /*!
    * 计算间接引用的地址的偏移量
    */
    int childDataOffset() const Q_DECL_OVERRIDE
    {
        IField* field = linkField();
        Q_ASSERT_X(field != NULL, "SymlinkField::linkField()", m_linkPath.toLocal8Bit().data());
        return field->childDataOffset();
    }

    /*!
    * 获取字段的flags，通常仅被FieldModel的同名函数遍历调用
    * @param[in]  与FieldModel的形参含义相同，详见FieldModel
    *
    * @note       注意IField以及其子类的flags|data|setData三个方法中的row和column参数不是给FieldModel使用的，
    *             而是给其子Field（通常是SubDataField）使用的。当FieldModel调用这三个方法时，需要传入-1。
    */
    Qt::ItemFlags flags(int row, int column) Q_DECL_OVERRIDE
    {
        IField* field = linkField();
        Q_ASSERT_X(field != NULL, "SymlinkField::linkField()", m_linkPath.toLocal8Bit().data());
        return field->flags(row, column);
    }

    /*!
    * 获取当前数据字段的实际数据，一般用于setupModel迅速获取数据或者用于调试
    * @param[in]  channel 通道号，所有配置数据字段均将数据存储于第0通道
    * @return     当前数据字段的实际数据
    */
    QVariant data(int channel = 0) const Q_DECL_OVERRIDE
    {
        IField* field = linkField();
        Q_ASSERT_X(field != NULL, "SymlinkField::linkField()", m_linkPath.toLocal8Bit().data());
        return field->data(channel);
    }

    /*!
    * 获取字段的数据，通常仅被FieldModel的同名函数遍历调用
    * @param[in]  与FieldModel的形参含义相同，详见FieldModel
    *
    * @note       注意IField以及其子类的flags|data|setData三个方法中的row和column参数不是给FieldModel使用的，
    *             而是给其子Field（通常是SubDataField）使用的。当FieldModel调用这三个方法时，需要传入-1。
    */
    QVariant data(int row, int column, int role) Q_DECL_OVERRIDE
    {
        if (role == FieldTextRole)
            return text();
        if (role == FieldNameRole)
            return name();
        IField* field = linkField();
        Q_ASSERT_X(field != NULL, "SymlinkField::linkField()", m_linkPath.toLocal8Bit().data());
        return field->data(row, column, role);
    }

    /*!
    * 设置字段的数据，通常仅被FieldModel的同名函数遍历调用
    * @param[in]  与FieldModel的形参含义相同，详见FieldModel
    *
    * @note       注意IField以及其子类的flags|data|setData三个方法中的row和column参数不是给FieldModel使用的，
    *             而是给其子Field（通常是SubDataField）使用的。当FieldModel调用这三个方法时，需要传入-1。
    */
    bool setData(int row, int column, const QVariant& newData, int role) Q_DECL_OVERRIDE
    {
        IField* field = linkField();
        Q_ASSERT_X(field != NULL, "SymlinkField::linkField()", m_linkPath.toLocal8Bit().data());
        return field->setData(row, column, newData, role);
    }

protected:
    QString         m_linkPath;             //!< 超链接路径
    mutable IField* m_linkField;            //!< 超链接到的字段
};

class FIELDLIBRARY_EXPORT SymlinkFieldIO : public FieldIO
{
public:
    SymlinkFieldIO(IField* field = NULL) : FieldIO(field)
    {

    }

public:
    /*!
     * 写数据到文件IO
     * @param[in]  file          打开的文件指针
     * @param[in]  dataTypeFlags 内容类型
     * return   成功返回写入数据数目
     */
    quint64 write(QIODevice* file, int dataTypeFlags = -1, void *data = NULL) Q_DECL_OVERRIDE
    {
        Q_ASSERT(m_field);
        SymlinkField *dataField = qobject_cast<SymlinkField *>(m_field);
        Q_ASSERT(dataField);
        return dataField->linkField()->write(file, dataTypeFlags, data);
    }

    /*!
     * 从文件IO中读取数据
     * @param[in]  file          打开的文件指针
     * @param[in]  dataTypeFlags 内容类型
     * return   成功返回读出的数据数目
     */
    quint64 read(QIODevice* file, int dataTypeFlags = -1, void *data = NULL) Q_DECL_OVERRIDE
    {
        Q_ASSERT(m_field);
        SymlinkField *dataField = qobject_cast<SymlinkField *>(m_field);
        Q_ASSERT(dataField);
        return dataField->linkField()->read(file, dataTypeFlags, data);
    }

    /*!
     * 当前字段的指定类型的内容是否可写
     * @param[in]  dataTypeFlags 内容类型
     * @param[in]  channel 通道号
     * @return     可写
     */
    bool isWritable(int dataTypeFlags = -1, int channel = 0)
    {
        Q_ASSERT(m_field);
        SymlinkField *dataField = qobject_cast<SymlinkField *>(m_field);
        Q_ASSERT(dataField);
        return dataField->linkField()->isWritable(dataTypeFlags, channel);
    }
};

/**
*  @class ChannelField
*  @brief 通道对应的虚拟字段，其实际数据存储于父字段，且其自身也是父字段自动创建并维护的
*/
class FIELDLIBRARY_EXPORT ChannelField : public VirtualField
{
    Q_OBJECT

public:
    ChannelField(const QString& name = QString(), IField* parent = NULL, int initialChildIndex = -1);
    ChannelField(const ChannelField& other);

    /*!
    * 设置当前通道字段的通道名和通道编号，用于在不重新创建对象的条件下改变ChannelField对应的通道信息
    * @param[in]  channelName          通道名，例如A、B、Total等
    * @param[in]  initialChildIndex    通道编号，对应的DSP数据通道，在F280中为0-4
    * @return     操作成功返回true
    */
    virtual bool setChannelName(const QString& channelName, int initialChildIndex = -1);


    /*!
    * 在此方法中复位趋势、缩略图等大型缓冲区，分配内存，并初始化
    */
    void resetBuffer(void *data = NULL) Q_DECL_OVERRIDE;

    /*!
    * 获取当前节点在父节点的所有子节点中的序号
    * @return     当前节点在父节点的所有子节点中的序号
    */
    int childIndex() const Q_DECL_OVERRIDE
    {
        if (m_childIndex < 0)
        {
            m_childIndex = VirtualField::childIndex();
        }

        return m_childIndex;
    }

protected:
    /*!
    * 获取字段的flags，通常仅被FieldModel的同名函数遍历调用
    * @param[in]  与FieldModel的形参含义相同，详见FieldModel
    *
    * @note       注意IField以及其子类的flags|data|setData三个方法中的row和column参数不是给FieldModel使用的，
    *             而是给其子Field（通常是SubDataField）使用的。当FieldModel调用这三个方法时，需要传入-1。
    */
    Qt::ItemFlags flags(int row, int column) Q_DECL_OVERRIDE;

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
    mutable int m_childIndex;
};


/*********************************************************************************************************
** End of file
*********************************************************************************************************/
