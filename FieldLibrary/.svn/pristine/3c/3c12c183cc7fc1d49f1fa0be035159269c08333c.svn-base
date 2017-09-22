/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  IField.h
** Latest Version:             V1.0.0
** Latest modified Date:       2017/2/16
** Modified by:                ZhangWenBo
** Descriptions:               字段类，添加未实现的需求，优化性能，修正bug
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 Han ZhenFei
** Created date:               2015/07/09
** Descriptions:               实际存放数据的通用接口
** 
*********************************************************************************************************/
#pragma once

#include <QObject>
#include <QVariant>
#include <QMutex>
#include <QIODevice>
#include "fieldlibrary_global.h"

class IField;

/**
 *  @enum  FieldItemDataRole 为了使得此枚举类型能够在Json脚本里面可以使用，此枚举FieldItemDataRole从全局拷贝一份在IField类内，
 *                           如需修改，请把全局FieldItemDataRole(在Field.h文件内)和IField类内的同时修改
 *  @brief 字段类型的数据模型Item的角色
 */
enum FieldItemDataRole 
{
    FieldDisplayRole = Qt::DisplayRole,
    FieldDecorationRole,
    FieldEditRole,
    FieldToolTipRole,
    FieldStatusTipRole,
    FieldWhatsThisRole,
    FieldSizeHintRole = Qt::SizeHintRole,
    FieldNameRole = Qt::UserRole + 16,      //!< 字段名称
    FieldTextRole,                          //!< 字段显示在界面上的名称
    FieldPathRole,                          //!< 字段路径（完整xpath）
    IsEnabledRole,                          //!< 字段是否使能
    FieldMetaTypeRole,                      //!< 获取字段对应的MetaType，如果是VirtualField则返回QMetaType::Void

    MaxValueRole,                           //!< 字段（通常是SetupField）允许的最大值
    MaxValueTextRole,                       //!< 字段允许的最大值归一格式化之后的字符串（不带Unit和Dimension），例如1234对应1.234，此时Unit=K
    MinValueRole,                           //!< 字段（通常是SetupField）允许的最小值
    MinValueTextRole,                       //!< 字段允许的最小值归一格式化之后的字符串（不带Unit和Dimension），例如1234对应1.234，此时Unit=K
    DefaultValueRole,                       //!< 字段（通常是SetupField）的默认值
    StepValueRole,                          //!< 字段（通常是SetupField）配置过程中的步进值
    UnitRole,                               //!< 字段目前所使用的数值单位（无/K/M/G），按照F280的需求，只有电压、电流、功率三类字段使用
    DimensionRole,                          //!< 字段（通常为RealField）数值所对应的量纲，例如W/A/Hz等，注意此处量纲不包含m/k/M等数值单位
    DimensionDataRole = DimensionRole,      //!< Alias of DimensionRole
    DigitalsRole,                           //!< 字段的小数点后位数（显示精度），0代表不显示小数点
    IsCustomChoicesRole,                    //!< 字段（通常是SetupField）是否是自定义可设置值的，例如串口波特率仅支持一组标准波特率列表
    CustomChoicesRole,                      //!< 字段（通常是SetupField）的自定义设置值列表
    CustomChoiceTextsRole,                  //!< 字段（通常是SetupField）的自定义设置选项列表，返回文本（QStringList），方便界面显示
    CustomChoiceSelectionsRole,             //!< 自定义设置值列表的选中状况，返回QList<bool>
    EditorTypeRole,                         //!< 字段（通常是SetupField）的编辑控件类型提示

    FieldDataRole,                          //!< 字段本身所对应的数据，不同类型的字段的数据类不同（例如RealField的数据类型为float）
    FieldDataTextRole,                      //!< 字段本身所对应的数据归一格式化之后的字符串（不带Unit和Dimension），例如1234对应1.234，此时Unit=K
    TrendDataRole,                          //!< 字段本身数据的趋势
    ThumbnailDataRole,                      //!< 字段的趋势缩略图
    IsOverloadedRole,                       //!< 字段是否超限
    IsInvalidRole,                          //!< 字段是否无效

    PhaseColorRole,                         //!< 字段对应的接线相（phase）的颜色（QColor）
    ViewColumnHintRole,                     //!< 当前Item在视图中的第几列的提示信息，仅供参考（例如Meter视图中，B项目位于第二列，而Total位于第四列）

    CursorPositionRole,
    MarkerDataRole,

    CursorVisibleRole,

    FrameSizeRole,
    PointSizeRole,
    ChannelCountRole,
    FlushNumberRole,

    EndOfIFieldRole                         //!< 用于子类扩展，外部禁止调用
};

/**
 *  @enum  FieldDataTypeFlags
 *  @brief 读写Field时指定的内容类型
 */
enum FieldDataTypeFlags
{
    NoDataType = 0,                         //!< 不指定数据类型（非法）

    FieldSpecDataType  = 0x00000001,        //!< 字段配置
    FieldDataType      = 0x00000002,        //!< 字段当前值，例如VoltageField的当前电压值（qreal）
    TrendDataType      = 0x00000004,        //!< 趋势数据
    ThumbnailDataType  = 0x00000008,        //!< 缩略图数据
    WaveDataType       = 0x00000010,        //!< wave数据
    HcRmsDataType      = 0x00000020,        //!< 半波有效值数据
    
    FlushBeforeClosing = 0x80000000,        //!< 附加标记，表示当前的I/O请求已经是最后一次，文件即将关闭
                                            //!< 因此未完成的缓冲区（例如趋势）也应被写入文件

    AllDataTypes       = 0x7FFFFFFF         //!< 全部数据类型
};

/**
 *  @class FieldVisitor
 *  @brief 字段对象的访问器接口，用于执行字段树的遍历算法等
 */
class FIELDLIBRARY_EXPORT FieldVisitor
{
public:
    FieldVisitor()
    {

    }


public:
    /*! 
     * 访问指定字段，对该字段执行某个操作
     * @param[in]  field 指定字段
     * @return     操作成功返回true，否则返回false。注意仅当返回true时，IField才会遍历当前字段的子字段。
     */
    virtual bool visit(IField* field) = 0;
};

class FIELDLIBRARY_EXPORT ResetVisitor : public FieldVisitor
{
public:
    bool visit(IField* field) Q_DECL_OVERRIDE;
};


class FIELDLIBRARY_EXPORT FieldIOVisitor : public FieldVisitor
{
public:
    bool visit(IField* field) Q_DECL_OVERRIDE;
};

/**
*  @class FieldIO
*  @brief 用于管理Field相关的IO操作
*/
class FIELDLIBRARY_EXPORT FieldIO
{
public:
    FieldIO(IField* field = NULL)
        : m_field(field)
    {

    }

    void setField(IField* field)
    {
        m_field = field;
    }

public:
    /*! 
     * 写数据到文件IO
     * @param[in]  file          打开的文件指针
     * @param[in]  dataTypeFlags 内容类型
     * return   成功返回写入数据数目
     */
    virtual quint64 write(QIODevice* file, int dataTypeFlags = -1, void *data = NULL)
    {
        Q_UNUSED(file);
        Q_UNUSED(dataTypeFlags);
        Q_UNUSED(data);
        return 0;
    }

    /*! 
     * 从文件IO中读取数据
     * @param[in]  file          打开的文件指针
     * @param[in]  dataTypeFlags 内容类型
     * return   成功返回读出的数据数目
     */
    virtual quint64 read(QIODevice* file, int dataTypeFlags = -1, void *data = NULL)
    {
        Q_UNUSED(file);
        Q_UNUSED(dataTypeFlags);
        Q_UNUSED(data);
        return 0;
    }

    /*! 
     * 当前字段的指定类型的内容是否可写
     * @param[in]  dataTypeFlags 内容类型
     * @param[in]  channel 通道号
     * @return     可写
     */
    virtual bool isWritable(int dataTypeFlags = -1, int channel = 0)
    {
        Q_UNUSED(dataTypeFlags);
        Q_UNUSED(channel);
        return false;
    }

    /*! 
     * 当前字段的指定类型的内容是否可读
     * @param[in]  dataTypeFlags 内容类型
     * @param[in]  channel 通道号
     * @return     可读
     */
    virtual bool isReadable(int dataTypeFlags = -1, int channel = 0)
    {
        Q_UNUSED(dataTypeFlags);
        Q_UNUSED(channel);
        return true;
    }

protected:
    IField* m_field;         //!<设置当前进行IO操作的字段
};

/**
*  @class ChannelFactory
*  @brief 用于管理Field相关的通道创建
*/
class FIELDLIBRARY_EXPORT ChannelFactory
{
public:
    ChannelFactory()
    {

    }

public:

    /*!
    * 设置通道掩码 ,设置当前字段使能了哪些channel，通常该字段必须为DataField。
    * @param[in]  IField* field
    * @return     操作成功返回true
    */
    virtual bool createChannel(IField* field);

    /*!
    * resetChannel  重置通道相关信息
    * @return:   void
    */
    virtual void resetChannel()
    {

    }

protected:

    /*!
    * 创建通道Field
    * @return     操作成功返回ChannelField
    */
    virtual IField* createChannelField(const QString& name = QString(), IField* parent = NULL, int initialChildIndex = -1);
};

class FIELDLIBRARY_EXPORT FieldReseter
{
public:
    FieldReseter()
    {

    }

    virtual void resetAll()
    {

    }
};

/**
*  @class IField
*  @brief 字段类：实际存放、操作数据的通用接口
*/
class FIELDLIBRARY_EXPORT IField : public QObject
{
    Q_OBJECT;
    Q_PROPERTY(QString id  READ name WRITE setName);
    Q_PROPERTY(QString name  READ name WRITE setName);
    Q_PROPERTY(QString text  READ text WRITE setText NOTIFY textChanged);
    Q_PROPERTY(QString path  READ mountPath WRITE setMountPath);
    Q_PROPERTY(bool enabled  READ enabled WRITE setEnabled);
    Q_PROPERTY(QList<IField*> children READ children WRITE setChildren)
    Q_PROPERTY(QString channelMask READ channelMask WRITE setChannelMask);
    Q_PROPERTY(QString offset READ relativeOffset  WRITE setRelativeOffset);
    Q_PROPERTY(QObject* valid READ validField  WRITE setValidField);
    Q_PROPERTY(FieldIO* fieldIO READ fieldIO  WRITE setFieldIO);
    Q_PROPERTY(QString notifySlot READ notifySlot  WRITE setNotifySlot);
    Q_PROPERTY(QString editorType READ editorType WRITE setEditorType)
    Q_ENUMS(FieldItemDataRole)
    

public:
    IField();
    IField(const IField& other);
    virtual  ~IField();


    /**
     *  @enum  FieldItemDataRole 为了使得此枚举类型能够在Json脚本里面可以使用，此枚举FieldItemDataRole从全局拷贝一份在IField类内，
     *                           如需修改，请把全局FieldItemDataRole(在Field.h文件内)和IField类内的同时修改
     *  @brief 字段类型的数据模型Item的角色
     */
    enum FieldItemDataRole 
    {
        FieldDisplayRole = Qt::DisplayRole,
        FieldDecorationRole,
        FieldEditRole,
        FieldToolTipRole,
        FieldStatusTipRole,
        FieldWhatsThisRole,
        FieldSizeHintRole = Qt::SizeHintRole,
        FieldNameRole = Qt::UserRole + 16,      //!< 字段名称
        FieldTextRole,                          //!< 字段显示在界面上的名称
        FieldPathRole,                          //!< 字段路径（完整xpath）
        IsEnabledRole,                          //!< 字段是否使能
        FieldMetaTypeRole,                      //!< 获取字段对应的MetaType，如果是VirtualField则返回QMetaType::Void

        MaxValueRole,                           //!< 字段（通常是SetupField）允许的最大值
        MaxValueTextRole,                       //!< 字段允许的最大值归一格式化之后的字符串（不带Unit和Dimension），例如1234对应1.234，此时Unit=K
        MinValueRole,                           //!< 字段（通常是SetupField）允许的最小值
        MinValueTextRole,                       //!< 字段允许的最小值归一格式化之后的字符串（不带Unit和Dimension），例如1234对应1.234，此时Unit=K
        DefaultValueRole,                       //!< 字段（通常是SetupField）的默认值
        StepValueRole,                          //!< 字段（通常是SetupField）配置过程中的步进值
        UnitRole,                               //!< 字段目前所使用的数值单位（无/K/M/G），按照F280的需求，只有电压、电流、功率三类字段使用
        DimensionRole,                          //!< 字段（通常为RealField）数值所对应的量纲，例如W/A/Hz等，注意此处量纲不包含m/k/M等数值单位
        DimensionDataRole = DimensionRole,      //!< Alias of DimensionRole
        DigitalsRole,                           //!< 字段的小数点后位数（显示精度），0代表不显示小数点
        IsCustomChoicesRole,                    //!< 字段（通常是SetupField）是否是自定义可设置值的，例如串口波特率仅支持一组标准波特率列表
        CustomChoicesRole,                      //!< 字段（通常是SetupField）的自定义设置值列表
        CustomChoiceTextsRole,                  //!< 字段（通常是SetupField）的自定义设置选项列表，返回文本（QStringList），方便界面显示
        CustomChoiceSelectionsRole,             //!< 自定义设置值列表的选中状况，返回QList<bool>
        EditorTypeRole,                         //!< 字段（通常是SetupField）的编辑控件类型提示

        FieldDataRole,                          //!< 字段本身所对应的数据，不同类型的字段的数据类不同（例如RealField的数据类型为float）
        FieldDataTextRole,                      //!< 字段本身所对应的数据归一格式化之后的字符串（不带Unit和Dimension），例如1234对应1.234，此时Unit=K
        TrendDataRole,                          //!< 字段本身数据的趋势
        ThumbnailDataRole,                      //!< 字段的趋势缩略图
        IsOverloadedRole,                       //!< 字段是否超限
        IsInvalidRole,                          //!< 字段是否无效

        PhaseColorRole,                         //!< 字段对应的接线相（phase）的颜色（QColor）
        ViewColumnHintRole,                     //!< 当前Item在视图中的第几列的提示信息，仅供参考（例如Meter视图中，B项目位于第二列，而Total位于第四列）

        CursorPositionRole, 
        MarkerDataRole, 

        EndOfIFieldRole                         //!< 用于子类扩展，外部禁止调用
    };

public:

    FieldIO* fieldIO() const
    {
        return m_fieldIO;
    }

    void setFieldIO(FieldIO * io)
    {
        m_fieldIO = io;
    }

    static void setChannelFactory(ChannelFactory * channelFactory)
    {
        s_channelFactory = channelFactory;
    }

    static void setFieldReseter(FieldReseter* fieldRest)
    {
        s_fieldReset = fieldRest;
    }

    void setObjectName(const QString & name)
    {
        m_name = name;
        QObject::setObjectName(name);
    }

    /*! 
     * 获取当前字段的数据类型，如果该字段为virtual，则返回UnknownType
     */
    virtual QMetaType::Type type() const
    {
        return QMetaType::Void;
    }

    QList<IField *> children() const
    {
        return m_children;
    }

    /*! 
     * 获取使能了的直系子字段列表
     * @return     使能了的直系子字段列表
     */
    QList<IField *> enabledChildren() const;

    void setChildren(QList<IField*>& childs)
    {    
        foreach(IField * child, childs)
        {
            this->addChild(child);
        }
        m_children = childs;
    }

    /*! 
     * 在当前字段下挂载一个新的子字段对象
     * @param[in]  child 子字段对象
     * @return     挂载后的当前字段的子字段个数，操作失败则返回-1
     */
    int addChild(IField* child);
    /*!
     * 在当前字段下解除索引index的子字段的挂载关系，并返回该索引index子字段
     * @param[in]  child 子字段对象的索引
     * @return     解除关系前索引index字段
     */
    IField* takeAt(int index);

    /*! 
     * 在当前字段下解除某个子字段的挂载关系
     * @param[in]  child 子字段对象
     * @return     解除挂载后的当前字段的子字段个数，操作失败则返回-1
     */
    int removeChild(IField* child);

    void removeAllChildren();

    /*! 
     * 设置当前字段的父字段
     * @param[in]  parent 父字段
     */
    void setParent(IField* parent);

    /*! 
     * 返回根字段
     * @return     根字段
     */
    inline static IField* root(void)
    {
        return s_root;
    }

    /*! 
     * 设置根字段
     * @param[in]  root     根字段
     */
    inline static void setRoot(IField* root)
    {
        s_root = root;
    }

    /*! 
     * 获取指定当前字段是否有效的字段
     */
    IField* validField() const
    {
        return m_validField;
    }

    /*! 
     * 设置指定当前字段是否有效的字段
     */
    void setValidField(QObject* field)
    {
        m_validField = static_cast<IField*>(field);
    }

    /*! 
     * 判断字段是否有效，默认有效
     * @param[in] channel 判断的通道
     * @return    有效则返回true
     * @note      若字段无效，则此时界面应该有所变化，例如显示变灰
     */
    virtual bool isValid(int channel = 0) const;

    /*! 
     * 字段本身的数据行数（不包括子字段，用于某些表格类字段的扩展，默认返回0）
     * @return     字段本身的数据行数
     */
    virtual int rowCount() const
    {
        return 0;
    }

    /*! 
     * 字段本身的数据列数（不包括子字段，用于某些表格类字段的扩展，默认返回0）
     * @return     字段本身的数据列数
     */
    virtual int columnCount() const
    {
        return 0;
    }

    /*! 
     * 表明当前字段是否包含数据
     * @return 返回false表示包含数据
     */
    virtual bool isVirtual() const
    {
        return true;
    }

    virtual FieldIO * createFieldIO()
    {
        return NULL;
    }

    /*! 
     * 获取当前字段的大小，不包括子字段（例如，绝大部分VirtualField的大小为0，而RealField大小为4）
     * @return     当前字段的大小，单位：字节
     */
    virtual int size() const
    {
        return 0;
    }

    /*! 
     * 获取字段的flags，通常仅被FieldModel的同名函数遍历调用
     * @param[in]  与FieldModel的形参含义相同，详见FieldModel
     *
     * @note       注意IField以及其子类的flags|data|setData三个方法中的row和column参数不是给FieldModel使用的，
     *             而是给其子Field（通常是SubDataField）使用的。当FieldModel调用这三个方法时，需要传入-1。
     */
    virtual Qt::ItemFlags flags(int row, int column);

    /*! 
     * 获取当前数据字段的实际数据，一般用于setupModel迅速获取数据或者用于调试
     * @param[in]  channel 通道号，所有配置数据字段均将数据存储于第0通道
     * @return     当前数据字段的实际数据
     */
    virtual QVariant data(int channel = 0) const
    {
        Q_UNUSED(channel);
        return QVariant();
    }

    /*! 
     * 获取字段的数据，通常仅被FieldModel的同名函数遍历调用
     * @param[in]  与FieldModel的形参含义相同，详见FieldModel
     *
     * @note       注意IField以及其子类的flags|data|setData三个方法中的row和column参数不是给FieldModel使用的，
     *             而是给其子Field（通常是SubDataField）使用的。当FieldModel调用这三个方法时，需要传入-1。
     */
    virtual QVariant data(int row, int column, int role);

    /*! 
     * 设置字段的数据，通常仅被FieldModel的同名函数遍历调用
     * @param[in]  与FieldModel的形参含义相同，详见FieldModel
     *
     * @note       注意IField以及其子类的flags|data|setData三个方法中的row和column参数不是给FieldModel使用的，
     *             而是给其子Field（通常是SubDataField）使用的。当FieldModel调用这三个方法时，需要传入-1。
     */
    virtual bool setData(int row, int column, const QVariant& newData, int role);

    /*! 
     * 返回通知响应槽名称
     */
    QString notifySlot() const
    {
        return m_notifySlot;
    };

    /*! 
     * 设置通知响应槽名称
     * @param[in]  slotName 响应槽名称
     */
    void setNotifySlot(const QString& slotName)
    {
        m_notifySlot = slotName;
    };

    /*! 
     * 可与QObject的ObjectName复用
     */
    QString name() const;

    /*! 
     * 设置当前字段的字段名
     * @param[in]  name 字段名
     */
    void setName(const QString& name);

    /*! 
     * 可与QObject的ObjectName复用
     */
    QString text() const
    {
        if (m_text.isEmpty())
            return this->name();
        return m_text;
    }

    /*! 
     * 设置当前字段的显示字段名
     * @param[in]  name 字段名
     */
    void setText(const QString& name)
    {
        m_text = name;
        emit textChanged();
    };

    /*! 
     * 返回此field的挂载路径
     */
    QString mountPath() const
    {
        return m_mountPath;
    }

    /*! 
     * 设置当前字段的挂载路径
     * @param[in] mountPath 挂载路径
     */
    void setMountPath(const QString& mountPath)
    {
        if (!m_mountPath.isEmpty())
        {
            IField::umount(m_mountPath);
        }
        m_mountPath = mountPath;
        IField::mount(this, m_mountPath);
    };

    /*!  
     * Getter/Setter for enabled
     */
    bool enabled() const 
    { 
        return m_enabled; 
    }
    void setEnabled(bool enabled) 
    { 
        m_enabled = enabled; 
    }

    /*!
    * Getter/Setter for editorType
    */
    QString editorType() const
    {
        return m_editorType;
    }
    void setEditorType(const QString & value)
    {
        m_editorType = value;
    }

    /*!
    * 根据当前字段内部的数据类型及长度，自动提示应该使用的编辑器，主要用于配置视图
    * @return     应该使用的编辑器
    */
    virtual QString editorHints() const
    {
        return QString();
    }

    /*! 
     * 设置relative偏移量
     */
    void setRelativeOffset(const QString& str);

    /*! 
     * 获取relative偏移量
     */
    QString relativeOffset() const
    {
        return QString();
    }

    /*! 
     * 获取字段路径
     * @return 字段路径
     */
    QString& path();

    /*! 
     * 获取父字段
     * @return 父字段
     */
    inline IField* parent() const
    {
        return m_parent;
    }

    /*! 
     * 获取当前节点在父节点的所有子节点中的序号
     * @return     当前节点在父节点的所有子节点中的序号
     */
    virtual int childIndex() const;

    /*! 
     * 获取子字段个数
     * @return 子字段个数
     */
    virtual int childCount() const;

    /*! 
     * 获取使能了的子字段个数
     * @return 使能了的子字段个数
     */
    virtual int enabledChildCount() const;

    /*! 
     * 根据子字段的序号获取子字段对象
     * @param[in]  index 子字段序号
     * @return     子字段对象
     */
    virtual IField* child(int index) const;

    /*! 
     * 根据子字段的名称获取子字段对象
     * @param[in]  name 子字段的名称
     * @return     子字段对象
     */
    virtual IField* child(const QString& name) const;

    /*! 
     * 获取通道掩码
     * @return     通道掩码
     */
    inline QString channelMask() const
    {
        return m_channelMask;
    }

    /*! 
     * 设置通道掩码 ,设置当前字段使能了哪些channel，通常该字段必须为DataField。
     * @param[in]  mask 掩码值
     * @return     操作成功返回true
     */
    bool setChannelMask(const QString& mask);

    /*!
    * 设置通道数，重建数据缓冲区等信息
    * @param[in]  channelCount 通道数，如果与原通道数匹配，则不会重建任何缓冲区
    * @return     重建了缓冲区则返回true，否则即使设置成功也返回false
    */
    virtual bool setChannelCount(int channelCount)
    {
        Q_UNUSED(channelCount);
        return false;
    }

    /*! 
     * 比较运算符，用于在数据模型中对字段进行排序，子类根据需要可以重写
     * @param[in]  right 运算符右侧的字段
     * @return     this < right
     * @note       默认通过字段的xpath进行比较
     */
    virtual bool operator < (const IField &right) const;
    
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

    /*! 
     * 复位当前字段，主要指清除缓存操作，例如globalDataOffset
     */
    virtual void reset();

    /*! 
     * 在此方法中复位趋势、缩略图等大型缓冲区，分配内存，并初始化
     */
    virtual void resetBuffer(void* data = NULL)
    {
        Q_UNUSED(data);
        // NOTHING TO DO IN BASE CLASS
    }

    /*! 
     * 克隆当前对象，返回一个重名但数据独立的新字段对象
     * @return     IField*
     */
    virtual IField* clone() const;

public:
    /*! 
     * 将指定的字段及其子字段挂载至指定路径
     * @param[in]  field 需要挂载的字段
     * @param[in]  path  字段挂载的目标路径（xpath）
     * @return     操作成功返回true
     */
    static bool mount(IField* field, const QString&  path);

    /*! 
     * 将指定路径的字段从该路径移除（解除挂载）
     * @param[in]  path 字段挂载的路径（xpath）
     * @return     操作成功返回true
     */
    static bool umount(const QString&  path);

    /*! 
     * 根据指定字段路径获取节点的数据
     * @param[in] fieldPath 指定的字段路径
     * @param[in] curField 当前字段路径
     * @return 该路径上的字段
     */
    static IField* at(const QString&  fieldPath, IField* curField = 0);

    /*! 
     * 在此静态方法中清除cache，在modelReset信号发出后自动使用
     */
    static void resetAll();

    /*! 
     * 使用指定的访问器来遍历字段树的整个分支
     * @param[in]  root    操作的根节点
     * @param[in]  visitor 指定的访问器（代表某种字段操作）
     * @return     整个分支的字段全部操作成功返回true，否则返回false
     * @note       请注意此函数的实现已高度优化，请勿随意修改或扩展，如有需要，请实现自己的Visitor子类
     */
    static bool visitTree(IField* root, FieldVisitor* visitor);

    /*! 
     * 在此静态方法中复制树的指定分支，其中每一个字段都被创建了一个同名的字段对象，其对应的数据被复制
     * @param[in] root 操作的根节点
     * @return    操作成功则返回复制的新的根节点对象，但需要注意该对象不能被mount到同一个字段树上，否则会产生重名错误
     */
    static IField* cloneTree(IField* root);

    /*! 
     * 在此静态方法中更新树的指定分支的原始数据
     * @param[in]  root      操作的根节点
     * @param[in]  basePtr   原始数据包的基地址（字段对应的原始数据的基地址=数据包基地址+字段全局数据偏移量）
     * @param[in]  channel   原始数据包对应的接线单元的序号，通常为0-7
     * @return    操作全部成功则返回true
     */
    static bool updateRawDataTree(IField* root, const void* basePtr, int channel);

    /*! 
     * 在此静态方法中提交树的指定分支的原始数据
     * @param[in]  root      操作的根节点
     * @param[in]  basePtr   原始数据包的基地址（字段对应的原始数据的基地址=数据包基地址+字段全局数据偏移量）
     * @param[in]  channel   原始数据包对应的接线单元的序号，通常为0-7
     * @return    操作全部成功则返回true
     */
    static bool commitRawDataTree(IField* root, void* basePtr, int channel);

    /*! 
     * 写数据到文件IO
     * @param[in]  file          打开的文件指针
     * @param[in]  dataTypeFlags 内容类型
     * return   成功返回写入数据数目
     */
    quint64 write(QIODevice* file, int dataTypeFlags = -1, void *data = NULL)
    {
        if (m_fieldIO)
        {
            int length = m_fieldIO->write(file, dataTypeFlags, data);
            return length;
        }
        return 0;
    }

    /*! 
     * 从文件IO中读取数据
     * @param[in]  file          打开的文件指针
     * @param[in]  dataTypeFlags 内容类型
     * return   成功返回读出的数据数目
     */
    quint64 read(QIODevice* file, int dataTypeFlags = -1, void *data = NULL)
    {
        if (m_fieldIO)
        {
            int length = m_fieldIO->read(file, dataTypeFlags, data);
            return length;
        }
        return 0;
    }

    /*! 
     * 当前字段的指定类型的内容是否可写
     * @param[in]  dataTypeFlags 内容类型
     * @return     可写
     */
    bool isWritable(int dataTypeFlags = -1, int channel = 0)
    {
        if (m_fieldIO)
        {
            bool ok = m_fieldIO->isWritable(dataTypeFlags, channel);
            return ok;
        }
        return false;
    }

    /*! 
     * 当前字段的指定类型的内容是否可读
     * @param[in]  dataTypeFlags 内容类型
     * @return     可读
     */
    bool isReadable(int dataTypeFlags = -1, int channel = 0)
    {
        if (m_fieldIO)
        {
            bool ok = m_fieldIO->isReadable(dataTypeFlags, channel);
            return ok;
        }
        return true;
    }
    

static  IField* s_root ;                                  //!< 存放根节点

public:
    /*! 
     * 计算间接引用的地址的偏移量
     */
    virtual int childDataOffset() const
    {
        return 0;
    }

    /*! 
     * 计算相对地址的偏移量
     */
    inline int localDataOffset() const
    {
        return m_localDataOffset;
    }

    /*! 
     * 计算绝对地址的偏移量
     */
    int globalDataOffset() const;

signals:
    /*!
    *  NOTIFY SIGNAL
    */
    void textChanged();
    void valueChanged();
     
protected:
    QString m_name;                                                     //!< 此字段仅说明原理，可与QObject的objectName复用
    QString m_text;                                                     //!< 此字段为界面上对应显示的字段名字(区别于name,name为字段树路径对应名字)
    QString m_mountPath;                                                //!< 此字段的挂载路径
    QString m_path;                                                     //!< 缓冲当前字段的路径，字段被重新挂载在其他路径后，该值自动调整
    bool    m_enabled;                                                  //!< 当前字段是否使能，在某些项目中，未使能的字段可能被隐藏
    QString m_editorType;                                               //!< 存放配置视图控件类型

    /**
     * 通道掩码指明了当前IField存在几个SubDataField，例如可以在XML中按照如下格式写：
     * channelMask='A|B|C|N'
     * channelMask='AB|BC|CA'
     * channelMask='Total'
     * 使用|分割各个channel，在mask中包含的通道，会自动创建其SubDataField对象并挂载在Field下，无需手动创建子对象。
     * 默认channelMask为'A|B|C|N'
     */
    QString        m_channelMask;                                       //!< 通道掩码

    QList<IField*> m_children;                                          //!< 此字段仅说明原理，可与QObject的child复用
    IField        *m_parent;                                            //!< 此字段仅说明原理，可与QObject的parent复用
    IField        *m_validField;                                        //!< 指定当前字段是否有效的字段
    QString        m_notifySlot;                                        //!< 当前字段刷新通知响应槽名称

    int            m_localDataOffset;                                   //!< IField相对于根节点数据基地址的偏移量
    mutable int    m_globalDataOffset;                                  //!< IField的绝对偏移量，随时有可能更新，不允许直接访问

protected:
    FieldIO *m_fieldIO;                                                 //!< Field相关IO管理类
    static ChannelFactory *s_channelFactory;                            //!< Field相关通道构建工厂
    static FieldReseter *s_fieldReset;                                  //!< 用于调用子类resetAll操作类
    // 在计算某个IField的绝对偏移量（相对于根节点数据基地址）时，需要逐层向上递归式累加，这是比较耗时的操作，因此建议IField的实现类添加FiledInfoCache属性，用于缓存上次计算得到的绝对偏移量。   
    static QHash<QString, IField*> s_cache;                             //!< path与Field的对应关系，每次查找全局路径以后，会保留此部分内容，下次再查找即可省略递归查找算法，根据hash快速定位
    static QMutex                  s_lock;                              //!< 跨线程操作s_cache时使用的互斥锁
    //friend class FieldProxyModel;
    //friend class FieldModel;
};

Q_DECLARE_METATYPE(IField)
Q_DECLARE_METATYPE(IField::FieldItemDataRole)

/*
 * @macro QUERY_SETUP_FIELD_DATA
 * @brief 获取系统配置的帮助函数，使用示例：
 *        QUERY_SETUP_FIELD_DATA("/sys/setup/Wiring/", int, wiringMode)
 *        if (wiringMode == Pqa::Wiring1PhiNeutral) { ... }
 */
#define QUERY_SETUP_FIELD_DATA(_path, _type, _var)                                                      \
    IField* _var##_field = IField::at(_path);                                                           \
    Q_ASSERT_X(_var##_field!=NULL, "QUERY_SETUP_FIELD_DATA", _path" is an invalid X-Path");             \
    QVariant _var##_variant = _var##_field->data();                                                     \
    Q_ASSERT_X(_var##_variant.isValid(), "QUERY_SETUP_FIELD_DATA", _path" does not contain valid data");\
    _type _var = _var##_variant.value<_type>();                                                           

/*
 * @macro QUERY_SETUP_FIELD_INT, QUERY_SETUP_FIELD_LONGINT, QUERY_SETUP_FIELD_BOOL
 * @brief 获取系统配置的帮助函数，配置信息类型必须为整数/枚举/逻辑值
 */
#define QUERY_SETUP_FIELD_INT(_path, _var)      QUERY_SETUP_FIELD_DATA(_path, int, _var)
#define QUERY_SETUP_FIELD_LONGINT(_path, _var)  QUERY_SETUP_FIELD_DATA(_path, quint64, _var)
#define QUERY_SETUP_FIELD_BOOL(_path, _var)     QUERY_SETUP_FIELD_DATA(_path, bool, _var)

/*
 * @macro QUERY_SETUP_FIELD_REAL
 * @brief 获取系统配置的帮助函数，配置信息类型必须为浮点型
 */
#define QUERY_SETUP_FIELD_REAL(_path, _var)     QUERY_SETUP_FIELD_DATA(_path, float, _var)

 /*
 * @macro QUERY_SETUP_FIELD_STRING
 * @brief 获取系统配置的帮助函数，配置信息类型必须为字符串
 */
#define QUERY_SETUP_FIELD_STRING(_path, _var)   QUERY_SETUP_FIELD_DATA(_path, QString, _var)


/*********************************************************************************************************
** End of file
*********************************************************************************************************/
