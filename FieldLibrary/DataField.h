/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  2015/07/09 
** Latest Version:             V1.0.0
** Latest modified Date:       2017/2/16
** Modified by:                ZhangWenBo
** Descriptions:               提供数据字段基类所需的默认实现，方便子类重写
** Note(s):                    注意DataField中使用的“通道”|“元素”（channel）|“单元”是同一术语，均指接线单元
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 Zhang WenBo
** Created date:               2015/07/09
** Descriptions:               
** 
*********************************************************************************************************/
#pragma once
#include <QVariant>
#include <QFlags>
#include <typeinfo>

#include "IField.h"

template <typename T>
class DataField;
#undef min
#undef max
/**
 *  @class FieldData
 *  @brief 字段数据缓冲区，包括当前值、趋势值、缩略图等，后期根据需求，可以自行扩展
 */
template<typename T>
class FieldData
{
public:
    T                            m_data;                                //!< 字段当前的值
    Qt::ItemFlags                m_flags;                               //!< 字段的flags


public:
    FieldData() 
        : m_data()
        , m_flags(Qt::ItemIsEnabled)
    {
        
    }

    FieldData(const FieldData& other)
        : m_data(other.m_data)
        , m_flags(other.m_flags)
    {

    }

    /*!
    * 获取字段的数据，通常仅被FieldModel的同名函数遍历调用
    * @param[in]  与FieldModel的形参含义相同，详见FieldModel
    *
    * @note       注意IField以及其子类的flags|data|setData三个方法中的row和column参数不是给FieldModel使用的，
    *             而是给其子Field（通常是SubDataField）使用的。当FieldModel调用这三个方法时，需要传入-1。
    */
    virtual QVariant data(int role)
    {
        Q_UNUSED(role);
        return QVariant();
    }

    /*!
    * 设置字段的数据，通常仅被FieldModel的同名函数遍历调用
    * @param[in]  与FieldModel的形参含义相同，详见FieldModel
    *
    * @note       注意IField以及其子类的flags|data|setData三个方法中的row和column参数不是给FieldModel使用的，
    *             而是给其子Field（通常是SubDataField）使用的。当FieldModel调用这三个方法时，需要传入-1。
    */
    virtual bool setData(const QVariant& newData, int role)
    {
        Q_UNUSED(newData);
        Q_UNUSED(role);
        return false;
    }

};

/**
 *  @class FieldSpec
 *  @brief 字段的“规格”，表明字段的选项、范围等
 */
template<typename T>
class FieldSpec
{
public: 
    FieldSpec() :
        m_default(),
        m_step(),
        m_min(),
        m_max(),
        m_dimension(),
        m_unit(0),
        m_digitals(0),
        m_alignment(Qt::AlignCenter),
        m_choices(NULL),
        m_typeName(typeid(T).name())
    {

    }

    FieldSpec(const FieldSpec& other) :
        m_default(other.m_default),
        m_step(other.m_step),
        m_min(other.m_min),
        m_max(other.m_max),
        m_dimension(other.m_dimension),
        m_unit(other.m_unit),
        m_digitals(other.m_digitals),
        m_alignment(other.m_alignment),
        m_choices(other.m_choices),
        m_typeName(other.m_typeName)
    {
        if (other.m_choices)
        {
            m_choices = new QList<T>(*other.m_choices);
        }
    }

    ~FieldSpec()
    {
        delete m_choices;
        m_choices = NULL;
    }

    bool isCustom() const
    {
        return m_choices == NULL ? false : true;
    }

    int addCustomItem(const T& value) 
    {
        m_choices.push_back(value);
    }

    T               m_default;                                          //!< 默认值，通常用于setup时对用户输入的默认操作
    T               m_step;                                             //!< 设置步进，通常用于setup时对用户可选内容的显示
    T               m_min;                                              //!< 最小的有效值，通常用于setup时对用户输入的检查
    T               m_max;                                              //!< 最大的有效值，通常用于setup时对用户输入的检查
    QString         m_dimension;                                        //!< 数值所对应的量纲，例如V、A等，不包含数值部分(m/k/M)
    int             m_unit;                                             //!< 数值所对应的单位，例如m/k/M等，以指数表达，m=-3,k=3,M=6,G=9等
    int             m_digitals;                                         //!< 显示的小数点位数 
    Qt::Alignment   m_alignment;                                        //!< 显示对齐方式 
    QList<T>*       m_choices;                                          //!< 存放自定义设定值的容器
    QString         m_typeName;                                         //!< 模板类型名
};


template<typename T>
class DataFieldIO : public FieldIO
{
public:
    DataFieldIO(IField* field = NULL) : FieldIO(field)
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
        Q_UNUSED(dataTypeFlags);
        Q_UNUSED(data);
        Q_ASSERT(m_field);
        DataField<T> *dataField = static_cast<DataField<T> *>(m_field);
        Q_ASSERT(dataField);
        int channelCount = dataField->channelCount();
        int countLenth = 0;
        for (int i = 0; i < channelCount; ++i)
        {
            QVariant val = dataField->data(i, 0, FieldDataRole);
            T data = val.value<T>();
            countLenth += file->write((char *)(&data), sizeof(T));
        }
        return countLenth;
    }

    /*!
     * 从文件IO中读取数据
     * @param[in]  file          打开的文件指针
     * @param[in]  dataTypeFlags 内容类型
     * return   成功返回读出的数据数目
     */
    quint64 read(QIODevice* file, int dataTypeFlags = -1, void *data = NULL) Q_DECL_OVERRIDE
    {
        Q_UNUSED(dataTypeFlags);
        Q_UNUSED(data);
        Q_ASSERT(m_field);
        DataField<T> *dataField = static_cast<DataField<T> *>(m_field);
        Q_ASSERT(dataField);
        int channelCount = dataField->channelCount();
        int countLenth = 0;
        for (int i = 0; i < channelCount; ++i)
        {
            T data = 0;
            countLenth += file->read((char *)(&data), sizeof(T));
            m_field->setData(i, 0, QVariant::fromValue<T>(data), FieldDataRole);
        }
        return countLenth;
    }

};

/**
 *  @class DataField
 *  @brief 包含实际数据的字段，数据类型为模板参数，各数据字段类应从此类继承以减小工作量
 */
template <typename T>
class DataField : public IField
{
public:
    DataField() : IField(), m_data(NULL), m_spec(NULL), m_channelCount(0)
    {
#if defined(_DEBUG) && 0
        static int count = 0;
        count++;
#endif
        m_spec = new FieldSpec<T>;
        Q_ASSERT(m_spec);

        // 对于非F280项目，默认只创建一个channel，因为只有F280项目才有channel的概念 [5/4/2016 CHENHONGHAO]
        setChannelCount(1);

        // 默认创建一个Default通道，方便外部通过传统的ChannelField接口来访问
        setChannelMask(QString());
    }

    DataField(const DataField& other)
        : IField(other)
        , m_spec(other.m_spec)
        , m_channelCount(other.m_channelCount)
    {
        m_data = new FieldData<T>*[m_channelCount];  // data读写，必须重新创建
        for (int i = 0; i < m_channelCount; ++i)
        {
            m_data[i] = new FieldData<T>();
            m_data[i]->m_data = other.m_data[i]->m_data;
        }

        // spec如果不重新创建，则在字段析构时，spec会被析构两次
        // FIXME: 考虑使用隐式数据共享
        if (other.m_spec)
        {
            m_spec = new FieldSpec<T>(*other.m_spec);
        }
    }

    virtual ~DataField()
    {
        removeAllChildren();

        for (int i = 0; i < m_channelCount; ++i)
        {
            delete m_data[i];
        }
        delete[] m_data;
        delete   m_spec;

        m_data = NULL;
        m_spec = NULL;
    }

    QMetaType::Type type() const Q_DECL_OVERRIDE
    {
        T data;
        QVariant var;

        var.setValue<T>(data);
        return (QMetaType::Type)var.type();
    }

    /*!
     * 获取当前字段的大小，不包括子字段（例如，绝大部分VirtualField的大小为0，而RealField大小为4）
     * @return     当前字段的大小，单位：字节
     */
    virtual int size() const
    {
        return sizeof(T);
    }

    FieldIO* createFieldIO() Q_DECL_OVERRIDE
    {
        FieldIO* theFieldIO = new DataFieldIO<T>(this);
        return theFieldIO;
    }

    /*!
     * 将默认实现从true改为false
     */
    virtual bool isVirtual() const
    {
        return false;
    }

    /*!
     * 申请数据缓存区
     * @param[in]  aFrameSize   数据结构大小
     * @param[in]  aFrameCount  数据总数
     * @return     申请成功返回true，否则返回false
     */
    bool allocBuffer(int frameSize, int frameCount)
    {
        return true;
    }

    /*!
     * 释放数据缓存区
     */
    void freeBuffer()
    {

    }

    /*!
     * 判断存储数据结构是否准备完成
     */
    bool isFrameReady()
    {
        return true;
    }

    /*!
     * 将字段对应的数据类型T的数据转换成字符串，本应为静态函数，考虑到不同子类实现不同，此处设置为虚函数
     * @param[out] stringOut 转换输出的字符串
     * @param[in]  dataIn    转换输入的数据
     * @return     转换成功返回TRUE
     */
    virtual bool toString(const T& dataIn, QString& stringOut, int curChannel = -1) const;

    /*!
     * 将字段对应的数据类型T的数据转换成字符串，本应为静态函数，考虑到不同子类实现不同，此处设置为虚函数
     * @param[in]  stringIn 转换输入的字符串
     * @param[out] dataOut  转换输出的数据
     * @return     转换成功返回TRUE
     */
    virtual bool fromString(T& dataOut, const QString& stringIn) const;

public:
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
    virtual QVariant data(int channel = 0) const;

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
     * 获取子字段的flags，通常仅被FieldModel的同名函数遍历调用
     * @param[in]  child 子字段的childIndex
     * @param[in]  与FieldModel的形参含义相同，详见FieldModel
     *
     * @note       注意IField以及其子类的flags|data|setData三个方法中的row和column参数不是给FieldModel使用的，
     *             而是给其子Field（通常是SubDataField）使用的。当FieldModel调用这三个方法时，需要传入-1。
     */
    virtual Qt::ItemFlags childFlags(int child, int row, int column);

    /*!
     * 获取子字段的数据
     * @param[in]  child      子字段的childIndex
     * @param[in]  row&column 与FieldModel的形参含义相同，详见FieldModel
     * @param[in]  ok         是否处理成功，处理失败必须将ok置为false，否则置为true或者不做处理
     *                        需要注意的时，如果该role能够被识别但数据无法处理，则仍需令ok=true
     */
    virtual QVariant childData(int child, int row, int column, int role, bool& ok);

    /*!
     * 设置子字段的数据
     * @param[in]  child      子字段的childIndex
     * @param[in]  row&column 与FieldModel的形参含义相同，详见FieldModel
     * @param[in]  ok         是否处理成功，处理失败必须将ok置为false，否则置为true或者不做处理
     *                        需要注意的时，如果该role能够被识别但数据无法处理，则仍需令ok=true
     */
    virtual bool setChildData(int child, int row, int column, const QVariant& newData, int role, bool& ok);

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

public:
    /*!
     * Getter/Setter for channelCount
     */
    int channelCount() const
    {
        return m_channelCount;
    }
    /*!
     * 设置通道数，重建数据缓冲区等信息
     * @param[in]  channelCount 通道数，如果与原通道数匹配，则不会重建任何缓冲区
     * @return     重建了缓冲区则返回true，否则即使设置成功也返回false
     */
    bool setChannelCount(int channelCount) Q_DECL_OVERRIDE;

    // 操作属性读写的各种函数
    // read and write property funtion
    T getDefault() const
    {
        return m_spec->m_default;
    }
    void setDefault(const T &value)
    {
        m_spec->m_default = value;
    }
    T step() const
    {
        return m_spec->m_step;
    }
    void setStep(const T &value)
    {
        m_spec->m_step = value;
    }
    T min() const
    {
        return m_spec->m_min;
    }
    void setMin(const T &value)
    {
        m_spec->m_min = value;
    }
    T max() const
    {
        return m_spec->m_max;
    }
    void setMax(const T &value)
    {
        m_spec->m_max = value;
    }
    int digitals() const
    {
        return m_spec->m_digitals;
    }
    void setDigitals(int value)
    {
        m_spec->m_digitals = value;
    }
    Qt::Alignment  alignment() const
    {
        return m_spec->m_alignment;
    }
    void setAlignment(const Qt::Alignment &value)
    {
        m_spec->m_alignment = value;
    }
    QChar unit() const;
    QString dimension() const
    {
        return m_spec->m_dimension;
    }
    void setDimension(const QString& dimension)
    {
        m_spec->m_dimension = dimension;
    }

    QString defaultText() const;

    void setDefaultText(const QString& text);

    /*!
     * 获取（最后一个）选项，该函数仅用于满足定义Q_PROPERTY所需的READ条件，通常无需调用
     * @return     最后一个选项
     */
    T lastCustomItem() const
    {
        if (!m_spec || !m_spec->m_choices || m_spec->m_choices->size() == 0)
            return T();
        return m_spec->m_choices->last();
    }
    /*!
     * 添加一个（自定义）选项
     * @param[in]  v 需要添加的选项
     */
    void addCustomItem(const T& v)
    {
        Q_ASSERT(m_spec);
        if (m_spec->m_choices == NULL)
        {
            m_spec->m_choices = new QList<T>;
        }
        m_spec->m_choices->push_back(v);
    }

    QList<T> choices() const
    {
        if (m_spec->m_choices == NULL)
            return QList<T>();
        return *(m_spec->m_choices);
    }
    void setChoices(const QList<T> &value)
    {
        if (m_spec->m_choices == NULL)
        {
            m_spec->m_choices = new QList<T>;
        }
        (* m_spec->m_choices) = value;
    }

    QStringList choiceTexts() const;
    void setChoiceTexts(const QStringList &texts);

    QString typeName() const
    {
        return m_spec->m_typeName;
    }
protected:

    virtual FieldData<T>* createFieldData()
    {
        FieldData<T>* fieldData = new FieldData<T>();
        return fieldData;
    }

    /**
     * 此处代理存储各个SubDataField的数据，SubDataField需要使用如下伪代码来获取其对应的数据：
     * parent()->channelData(this->column)
     */
    FieldData<T>** m_data;                   //!< 子字段（全部为自动创建的ChannelField）的数据，默认最多8个通道
    FieldSpec<T>* m_spec;                   //!< 字段相关的配置信息，例如极限值、默认值等
    int           m_channelCount;           //!< m_data中对应的接线单元个数，即m_data数组的长度，不代表ChannelField子字段的个数
};

//#include "DataField_p.h"

/**
 *  @macro DECLARE_DATA_FIELD_PROPERTIES
 *  @brief 用于生成DataField子类的各个默认属性的快捷帮助宏
 */
#define DECLARE_DATA_FIELD_PROPERTIES(_type)                                                            \
    Q_PROPERTY(_type default READ getDefault WRITE setDefault)                                          \
    Q_PROPERTY(_type step READ step WRITE setStep)                                                      \
    Q_PROPERTY(_type min READ min WRITE setMin)                                                         \
    Q_PROPERTY(_type max READ max WRITE setMax)                                                         \
    Q_PROPERTY(int digitals READ digitals WRITE setDigitals)                                            \
    Q_PROPERTY(QString defaultText READ defaultText WRITE setDefaultText)                               \
    Q_PROPERTY(QString dimension READ dimension WRITE setDimension)                                     \
    Q_PROPERTY(Qt::Alignment alignment READ alignment WRITE setAlignment)                               \
    Q_PROPERTY(_type choice  READ lastCustomItem WRITE addCustomItem)                                   \
    Q_PROPERTY(QList<_type> choices READ choices WRITE setChoices)                                      \
    Q_PROPERTY(QList<_type> customItems READ choices WRITE setChoices)                                  \
    Q_PROPERTY(QStringList choiceTexts READ choiceTexts WRITE setChoiceTexts)                           \
    Q_PROPERTY(QString typeName READ typeName )                                                         \

/**
 *  @macro DECLARE_DATA_FIELD_CLASS
 *  @brief 用于生成DataField子类的快捷帮助宏
 */
#define DECLARE_DATA_FIELD_CLASS(_type, _name)                                                          \
class _name :  public DataField<_type>                                                                  \
{                                                                                                       \
    Q_OBJECT                                                                                            \
    DECLARE_DATA_FIELD_PROPERTIES(_type)                                                                \
public:                                                                                                 \
    _name() { }                                                                                         \
    _name(const _name& other) : DataField<_type>(other) { }                                             \
};

/*********************************************************************************************************
** End of file
*********************************************************************************************************/
