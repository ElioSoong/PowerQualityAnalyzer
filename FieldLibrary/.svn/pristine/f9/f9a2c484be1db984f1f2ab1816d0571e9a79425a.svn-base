/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  DataField.cpp
** Latest Version:             V1.0.0
** Latest modified Date:       2017/2/16
** Modified by:                ZhangWenBo
** Descriptions:               
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 Chen Honghao
** Created date:               2016/06/16
** Descriptions:               数据字段的实现，由于是模板类，需要引用实现细节的类，必须包含此文件
** 
*********************************************************************************************************/
#pragma once

#include <QDebug>

template <typename T>
void DataField<T>::setChoiceTexts(const QStringList &texts)
{
    T item;
    if (m_spec->m_choices == NULL)
    {
        m_spec->m_choices = new QList<T>;
    }

    m_spec->m_choices->clear();
    for (QStringList::const_iterator it = texts.cbegin(); it != texts.cend(); it++)
    {
        QString str = *it;

        QString d = this->dimension();
        QChar   u = this->unit();
        if (!d.isEmpty())
            str.remove(d);

        if (!fromString(item, str))
        {
            qWarning() << "Failed to convert customItemText[" << *it << "] to data." << endl;
            item = T();
        }

        m_spec->m_choices->push_back(item);
    }
}

template <typename T>
QStringList DataField<T>::choiceTexts() const
{
    QStringList texts;
    if (!m_spec->m_choices)
        return texts;
    for (typename QList< T >::const_iterator it=m_spec->m_choices->cbegin();
            it != m_spec->m_choices->cend(); it++)
    {
        QString str;
        if (!toString(*it, str))
        {
            qWarning() << "Failed to convert data from customItemText." << endl;
            str = "<UnknownField>";
        }

        QChar   u = this->unit();
        QString d = this->dimension();
        if (u != ' ' || !d.isEmpty())
        {
            str.append(u);
        }
        if (!d.isEmpty())
        {
            str.append(d);
        }

        texts.push_back(str);
    }
    return texts;
}

template <typename T>
void DataField<T>::setDefaultText(const QString& text)
{
    QString str = text;

    QString d = this->dimension();
    QChar   u = this->unit();
    if (!d.isEmpty())
        str.remove(d);

    Q_ASSERT(m_spec);
    fromString(m_spec->m_default, str);
}

template <typename T>
QString DataField<T>::defaultText() const
{
    QString text;
    Q_ASSERT(m_spec);
    toString(m_spec->m_default, text);

    QChar   u = this->unit();
    QString d = this->dimension();
    if (u != ' ' || !d.isEmpty())
    {
        text.append(u);
    }
    if (!d.isEmpty())
    {
        text.append(d);
    }

    return text;
}

template <typename T>
QChar DataField<T>::unit() const
{
    static const QChar UnitCharTab[] = 
    {
        'm', ' ', 'K', 'M', 'G',
    };

    if (m_spec->m_unit >= -3 && m_spec->m_unit <= 9)
    {
        return UnitCharTab[m_spec->m_unit/3 + 1];
    }

    return QChar('?');
}

template <typename T>
bool DataField<T>::setChannelCount(int channelCount)
{
    if (m_channelCount == channelCount)
        return false;

    for (int i = 0; i < m_channelCount; ++i)
    {
        delete m_data[i];
    }
    delete[] m_data;

    // 保证缓冲区顺序如下：
    // A=0,B=1,C=2,N=3,Total=4
    m_channelCount = channelCount;

    m_data = new FieldData<T>*[m_channelCount];
    for (int i = 0; i < m_channelCount; ++i)
    {
        m_data[i] = createFieldData();
    }

    if (m_data == NULL)
    {
        Q_ASSERT_X(m_data, "DataField::setChannelCount()", "OUT OF MEMORY");
        return false;
    }

    return true;
}

template <typename T>
void DataField<T>::reset()
{
    for (int i = 0; i < m_channelCount; i++)
    {
        m_data[i]->m_data = m_spec->m_default;
    }

    IField::reset();
}

template <typename T>
bool DataField<T>::commitRawData(void* basePtr, int channel) const
{
    int offset = globalDataOffset();
    T* d = reinterpret_cast<T*>(reinterpret_cast<char*>(basePtr) + offset);

    if (m_data == NULL || channel < 0 || channel >= m_channelCount)
        return false;

    *d = m_data[channel]->m_data;
    return true;
}

template <typename T>
bool DataField<T>::updateRawData(const void* basePtr, int channel)
{
    int offset = globalDataOffset();
    const T* d = reinterpret_cast<const T*>(reinterpret_cast<const char*>(basePtr) + offset);

    if (m_data == NULL || channel < 0 || channel >= m_channelCount)
        return false;

    m_data[channel]->m_data = *d;

    return true;
}

template <typename T>
QVariant DataField<T>::childData(int child, int row, int column, int role, bool& ok)
{
    Q_UNUSED(row);
    Q_UNUSED(column);
    // 检查FieldData，这些请求只能是子字段(ChannelField，)
    Q_ASSERT(m_data);
    QVariant dataVar = m_data[child]->data(role);
    if (dataVar != QVariant())
    {
        return dataVar;
    }
    switch (role)
    {
    case Qt::DisplayRole:
    case FieldDataTextRole:
    case Qt::EditRole:
        {
            QString str;
            if (!toString(m_data[child]->m_data, str, child))
            {
                qWarning() << "Failed to convert data from customItemText." << endl;
                str = "<UnknownField>";
            }

            if (role == Qt::DisplayRole)
            {
                QChar   u = this->unit();
                QString d = this->dimension();

#if 1
                if (u != ' ' || !d.isEmpty())
#else
                // 排除无效的空格
                if (u != ' ' && !d.isEmpty())
#endif
                {
                    str.append(u);
                }
                str.append(d);
            }

            return str;
        }
        break;

    case FieldDataRole:
        return QVariant::fromValue<T>(m_data[child]->m_data);

    case IsOverloadedRole:
        Q_ASSERT(m_spec);
        return m_spec->m_max < m_data[child]->m_data || m_data[child]->m_data < m_spec->m_min ? true : false;

    case IsInvalidRole:
        // 执行合法性检测
        return !isValid(child);

    case PhaseColorRole:
    case ViewColumnHintRole:
        return QVariant();
        //Q_ASSERT_X(0, "DataField::data()", "PhaseColorRole|ViewColumnHintRole MUST be retrieved from ChannelField");
        //break;

    case CursorPositionRole:
    case MarkerDataRole:
    default:
        // 留给基类处理，通常基类只能处理FieldNameRole和FieldPathRole两个角色
        //qDebug("Unsupported role id(%d) @DataField::data().\n", role);
        break;
    }

    ok = false;
    return QVariant();
}

template <typename T>
Qt::ItemFlags DataField<T>::childFlags(int child, int row, int column)
{
    Q_UNUSED(row);
    Q_UNUSED(column);
    if (child < 0)
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    else if (child >= m_channelCount)
    {
        qDebug("Requesting channel-field flags with invalid child index (%d).\n", child);
        return Qt::NoItemFlags;
    }

    return m_data[child]->m_flags;
}

template <typename T>
bool DataField<T>::setData(int row, int column, const QVariant& newData, int role)
{
    // 首先尝试由IField处理公共角色，例如FieldPathRole
    bool b = IField::setData(row, column, newData, role);
    if (b) 
        return b;

    int child = row;
    row = -1;
    column = -1;

    // FieldSpec不支持运行时改变，只支持通过setProperty接口在xml中静态调整

    // 如果不是DataField的相关Role，且在向本字段请求数据，则执行基类默认行为
    if (child < 0)
    {
        // TODO: 1009 暂时禁用通道检测，如果通道非法，默认获取首个通道的数据
#if 0
        // 仅当DataField的子Field个数超过1时，尝试获取FieldDataRole等角色时才使用IField的默认实现（通常是失败的）
        // 否则，认为该DataField的唯一子字段为该DataField的默认通道，此时可以不指定子字段的名称（例如A|B|C|Total）
        if (childCount() > 1)
            return IField::setData(row, column, newData, role);
        else
#endif
            child = 0;
    }
    else if (child >= m_channelCount)
    {
        qDebug() 
            << "ChannelField::setData(): index out-of-bound: parent=" << name() 
            << ", channelCount=" << m_channelCount
            << ", index=" << child;
        return false;
    }

    bool ok = true;
    bool childResult = setChildData(child, row, column, newData, role, ok);
    if (ok)
        return childResult;

    return IField::setData(row, column, newData, role);
}

template <typename T>
QVariant DataField<T>::data(int channel /*= 0*/) const
{
    Q_ASSERT_X(
        m_data != NULL && channel < m_channelCount, 
        "DataField::data(int)", "Failed to get setup-data in DataField."
        );
    return QVariant::fromValue<T>(m_data[channel]->m_data);
}

template <typename T>
QVariant DataField<T>::data(int row, int column, int role)
{
    // 首先尝试由IField处理公共角色，例如FieldPathRole
    QVariant result = IField::data(row, column, role);
    if (result.isValid())
        return result;

    // DataField认为每一个字段均是单行单列的，包括各个子ChannelField
    int child = row;
    row = -1;
    column = -1;

    // 检查FieldSpec，无论是在访问本字段还是子字段
    switch (role)
    {
    case MaxValueRole:
        Q_ASSERT(m_spec);
        return QVariant::fromValue<T>(m_spec->m_max);

    case MinValueRole:
        Q_ASSERT(m_spec);
        return QVariant::fromValue<T>(m_spec->m_min);

    case MaxValueTextRole:
    case MinValueTextRole:
        {
            QString str;
            if (!toString((role == MaxValueTextRole ? m_spec->m_max : m_spec->m_min), str))
            {
                qWarning() << "Failed to convert data from customItemText." << endl;
                str = "<UnknownField>";
            }

            QChar   u = this->unit();
            if (u != ' ')
                str.append(u);
            //str.append(this->dimension());

            return str;
        }

    case DefaultValueRole:
        Q_ASSERT(m_spec);
        return QVariant::fromValue<T>(m_spec->m_default);

    case StepValueRole:
        Q_ASSERT(m_spec);
        return QVariant::fromValue<T>(m_spec->m_step);

    case UnitRole:
        {
            static const QChar UnitCharTab[] = 
            {
                'm', ' ', 'K', 'M', 'G',
            };

            Q_ASSERT(m_spec);
            if (m_spec->m_unit >= -3 && m_spec->m_unit <= 9)
            {
                return QVariant(UnitCharTab[m_spec->m_unit/3 + 1]);
            }

            return QVariant('?');
        }

    case DimensionDataRole:
        Q_ASSERT(m_spec);
        return QVariant(m_spec->m_dimension);

    case DigitalsRole:
        Q_ASSERT(m_spec);
        return QVariant(m_spec->m_digitals);

    case Qt::TextAlignmentRole:
        if (m_spec)
            return QVariant(m_spec->m_alignment);
        else
        {
            // 对齐信息不可用时，应尝试使用默认对齐信息，而不是报错
            break;
        }

    case IsCustomChoicesRole:
        Q_ASSERT(m_spec);
        return (m_spec->m_choices == NULL || m_spec->m_choices->size() == 0) ? false : true;

    case CustomChoicesRole:
        Q_ASSERT(m_spec);
        if (m_spec->m_choices == NULL)
            return QVariant();
        return QVariant::fromValue< QList<T> >(*m_spec->m_choices);

    case CustomChoiceTextsRole:
        return QVariant::fromValue<QStringList>(choiceTexts());

    case ChannelCountRole:
        return m_channelCount;

    default:
        break;
    }

    // 如果不是DataField的相关Role，且在向本字段请求数据，则执行基类默认行为
    if (child < 0)
    {
        // TODO: 1009 暂时禁用通道检测，如果通道非法，默认获取首个通道的数据
#if 0
        // 仅当DataField的子Field个数超过1时，尝试获取FieldDataRole等角色时才使用IField的默认实现（通常是失败的）
        // 否则，认为该DataField的唯一子字段为该DataField的默认通道，此时可以不指定子字段的名称（例如A|B|C|Total）
        if (childCount() > 1)
            return IField::data(row, column, role);
        else
#endif
            child = 0;
    }
    else if (child >= m_channelCount)
    {
        if (child != 0x7FFFFFFF) 
        {
            qDebug() 
                << "ChannelField::data(): index out-of-bound: parent=" << name() 
                << ", channelCount=" << m_channelCount
                << ", index=" << child;
            return QVariant();
        }
        else
        {
            //qDebug()
            //    << "Default as r=0,c=0 for IFields at SetupView, "
            //    << "you MUST NOT use this feature in other views.";
            child = 0;
            row = 0;
            column = 0;
        }
    }

    bool ok = true;
    QVariant childResult = childData(child, row, column, role, ok);
    if (ok)
        return childResult;

    return IField::data(row, column, role);
}

template <typename T>
Qt::ItemFlags DataField<T>::flags(int row, int column)
{
    Q_UNUSED(column);
    // 子字段复用row来传递自身的childIndex，实际上真正的row和column机制在字段树中从未被使用
    return childFlags(row, -1, -1);
    //return flags(-1, row, column);
}

template <typename T>
bool DataField<T>::fromString(T& dataOut, const QString& stringIn) const
{
    QVariant variant(stringIn);
    if (!variant.canConvert<T>())
        return false;

    dataOut = QVariant(stringIn).value<T>();
    return true;
}

template <typename T>
bool DataField<T>::toString(const T& dataIn, QString& stringOut, int curChannel) const
{
    Q_UNUSED(curChannel);
    stringOut = QVariant::fromValue<T>(dataIn).toString();
    return true;
}

template <typename T>
bool DataField<T>::setChildData(int child, int row, int column, const QVariant& newData, int role, bool& ok)
{
    emit valueChanged();
    Q_UNUSED(row);
    Q_UNUSED(column);
    // 检查FieldData，这些请求只能是子字段(ChannelField)
    Q_ASSERT(m_data);
    bool dataResult = m_data[child]->setData(newData, role);
    if (dataResult)
    {
        return dataResult;
    }
    switch (role)
    {
    case Qt::DisplayRole:
    case Qt::EditRole:
    case FieldDataTextRole:
        {
            QString str = newData.toString();

            QString d = this->dimension();
            if (!d.isEmpty())
            {
                str.remove(d);
            }

            if (!fromString(m_data[child]->m_data, str))
            {
                qWarning() << "Failed to convert string[" << str << "] to data.";
            }

            return true;
        }

    case FieldDataRole:
        m_data[child]->m_data = newData.value<T>();
        return true;

    case IsOverloadedRole:
    case IsInvalidRole:
        Q_ASSERT(0);
        return false;

    case PhaseColorRole:
    case ViewColumnHintRole:
        Q_ASSERT_X(0, "DataField::data()", "PhaseColorRole|ViewColumnHintRole MUST be retrieved from ChannelField");
        return false;

    case CursorPositionRole:
    case MarkerDataRole:
    default:
        qDebug("Unsupported role id(%d) @DataField::data().\n", role);
        break;
    }

    ok = false;
    return false;
}
/*********************************************************************************************************
** End of file
*********************************************************************************************************/

