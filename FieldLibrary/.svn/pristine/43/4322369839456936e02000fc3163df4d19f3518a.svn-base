/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  VirtualField.cpp
** Latest Version:             V1.0.0
** Latest modified Date:       2017/2/16
** Modified by:                ZhangWenBo
** Descriptions:               虚拟字段及其子类
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 
** Created date:               
** Descriptions:               
** 
*********************************************************************************************************/
#include "VirtualField.h"
#include "IntegerField.h"
#include <QStack>

//Q_DECLARE_METATYPE(LoopedFrameBuffer<STAT_DATA>)

/*********************************************************************************************************
** VirtualField
*********************************************************************************************************/
/*! 
 * 获取当前字段的大小，不包括子字段（例如，绝大部分VirtualField的大小为0，而RealField大小为4）
 * @return     当前字段的大小，单位：字节
 */
int VirtualField::size() const
{
    int result = 0;

#if 1
    QList<IField*>::const_iterator it = m_children.cbegin();
    int childrenCount = m_children.size();
    while (childrenCount--)
    {
        result += (*it++)->size();
    }
#endif

    return result;
}

/*! 
 * 在此方法中复位趋势、缩略图等大型缓冲区，分配内存，并初始化
 */
void VirtualField::resetBuffer(void * data)
{
    QStack<IField*> stack;
    IField* field = this;

    stack.push(field);
    while(!stack.empty()) 
    {
        field = stack.pop();
        if (field == NULL)
            continue;

        if (!field->isVirtual())
        {
            field->resetBuffer(data);
        }
        else
        {
            int childCount = field->childCount();
            for (int i = 0; i < childCount; ++i)
                stack.push(field->child(i));
        }
    }
}


/*********************************************************************************************************
** ArrayField
*********************************************************************************************************/
ArrayField::ArrayField() : m_sizeField(NULL)
{

}

/*! 
 * 获取数组每一个元素的大小
 * @return     数组每一个元素的大小，单位：字节
 */
int ArrayField::channelSize() const
{
    if (m_children.size() == 0)
        return 0;

    IField* back = m_children.back();
    Q_ASSERT(back);

    int size = back->localDataOffset() + back->size();
    return size;
}

/*! 
 * 根据子字段的名称获取子字段对象
 * @param[in]  name 子字段的名称
 * @return     子字段对象
 */
IField* ArrayField::child(const QString& name) const
{
    Q_UNUSED(name);
#if 0
    bool ok = false;
    int index = name.toInt(&ok);
    if (!ok)
        return NULL;

    // TODO:
#endif
    return NULL;
}

/*! 
 * 获取子字段个数
 * @return 子字段个数
 */
int ArrayField::childCount() const
{
    if (m_sizeField == NULL)
        return 0;

    IntegerField* field = static_cast<IntegerField*>(m_sizeField);
    Q_ASSERT(field);

    // TODO: 此处默认使用(0, 0)，即首行首列所记录的childCount，其他通道的计数值将被忽略
    return field->data(0, 0, FieldDataRole).toInt();
}

/*! 
 *  更新当前字段所对应的原始数据（RawData=>FieldTree）
 * @param[in]  basePtr   原始数据包的基地址（字段对应的原始数据的基地址=数据包基地址+字段全局数据偏移量）
 * @param[in]  channel   原始数据包对应的接线单元的序号，通常为0-7
 */
bool ArrayField::updateRawData(const void* basePtr, int channel)
{
    Q_UNUSED(basePtr);
    Q_UNUSED(channel);
    return true;
}

/*! 
 *  提交当前字段所对应的原始数据（RawData=>FieldTree）
 * @param[in]  basePtr   原始数据包的基地址（字段对应的原始数据的基地址=数据包基地址+字段全局数据偏移量）
 * @param[in]  channel   原始数据包对应的接线单元的序号，通常为0-7
 */
bool ArrayField::commitRawData(void* basePtr, int channel) const
{
    Q_UNUSED(basePtr);
    Q_UNUSED(channel);
    return true;
}

/*********************************************************************************************************
** SymlinkField
*********************************************************************************************************/

FieldIO* SymlinkField::createFieldIO()
{
    FieldIO* theFieldIO = new SymlinkFieldIO(this);
    return theFieldIO;
}

/*********************************************************************************************************
** ChannelField
*********************************************************************************************************/
ChannelField::ChannelField(const QString& name, IField* parent, int initialChildIndex)
    : VirtualField(name, parent)
    , m_childIndex(initialChildIndex)
{
    Q_ASSERT(m_parent);
}

ChannelField::ChannelField(const ChannelField& other)
    : VirtualField(other.m_name, NULL)
    , m_childIndex(other.m_childIndex)
{

}

/*!
* 设置当前通道字段的通道名和通道编号，用于在不重新创建对象的条件下改变ChannelField对应的通道信息
* @param[in]  channelName          通道名，例如A、B、Total等
* @param[in]  initialChildIndex    通道编号，对应的DSP数据通道，在F280中为0-4
* @return     操作成功返回true
*/
bool ChannelField::setChannelName(const QString& channelName, int initialChildIndex /*= -1*/)
{
    Q_UNUSED(initialChildIndex);
    m_name = channelName;
    return true;
}

/*!
* 在此方法中复位趋势、缩略图等大型缓冲区，分配内存，并初始化
*/
void ChannelField::resetBuffer(void *data)
{
    if (m_parent)
    {
        m_parent->resetBuffer(data);
    }
}

/*!
* 获取字段的flags，通常仅被FieldModel的同名函数遍历调用
* @param[in]  与FieldModel的形参含义相同，详见FieldModel
*
* @note       注意IField以及其子类的flags|data|setData三个方法中的row和column参数不是给FieldModel使用的，
*             而是给其子Field（通常是SubDataField）使用的。当FieldModel调用这三个方法时，需要传入-1。
*/
Qt::ItemFlags ChannelField::flags(int row, int column)
{
    Q_UNUSED(row);
    Q_UNUSED(column);
    Q_ASSERT(m_parent);
    return m_parent->flags(childIndex(), 0);
}

/*!
* 获取字段的数据，通常仅被FieldModel的同名函数遍历调用
* @param[in]  与FieldModel的形参含义相同，详见FieldModel
*
* @note       注意IField以及其子类的flags|data|setData三个方法中的row和column参数不是给FieldModel使用的，
*             而是给其子Field（通常是SubDataField）使用的。当FieldModel调用这三个方法时，需要传入-1。
*/
QVariant ChannelField::data(int row, int column, int role)
{
    // 首先尝试由IField处理公共角色，例如FieldPathRole
    QVariant result = IField::data(row, column, role);
    if (result.isValid())
        return result;

    // 最后尝试解析标准DataField角色
    Q_ASSERT(m_parent);
    return m_parent->data(childIndex(), 0, role);
}

/*!
* 设置字段的数据，通常仅被FieldModel的同名函数遍历调用
* @param[in]  与FieldModel的形参含义相同，详见FieldModel
*
* @note       注意IField以及其子类的flags|data|setData三个方法中的row和column参数不是给FieldModel使用的，
*             而是给其子Field（通常是SubDataField）使用的。当FieldModel调用这三个方法时，需要传入-1。
*/
bool ChannelField::setData(int row, int column, const QVariant& newData, int role)
{
    Q_UNUSED(row);
    Q_UNUSED(column);
    Q_ASSERT(m_parent);

    return m_parent->setData(childIndex(), 0, newData, role);
}

/*********************************************************************************************************
** End of file
*********************************************************************************************************/
