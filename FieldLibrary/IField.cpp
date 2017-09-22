/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  IField.cpp
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
#include "includes.h"

#include <QDebug>
#include <QHash>
#include <QStack>
#include <QQueue>
#include <QStringList>
#include "IField.h"
#include "VirtualField.h"
#include <QMetaObject>
#include <QMetaProperty>

/*
 * @brief 是否允许通过QObject的相关API来访问字段，例如objectName(), findChildren(), parent()等
 */
#define ENABLE_QOBJECT_API                  0

class RawDataVisitor : public FieldVisitor
{
protected:
    //typedef bool (IField::*RawDataMethod)(void*, int);

    void*         m_basePtr;
    int           m_channel;
    //RawDataMethod m_method;

    bool visit(IField* field) Q_DECL_OVERRIDE
    {
#if 0
        if (!field->(m_method)(m_basePtr, m_channel))
            return false;
#endif
        return field->isVirtual(); // 此处返回false仅为了保证ChannelField子字段不被遍历，从而加速处理
    }

public:
    RawDataVisitor(void* basePtr, int channel) : 
      FieldVisitor(), 
          m_basePtr(basePtr), 
          m_channel(channel)
      {

      }
};

class UpdateRawDataVisitor : public RawDataVisitor
{
    bool visit(IField* field) Q_DECL_OVERRIDE
    {
#if DUMP_FIELD_TREE_ON_UPDATE
        static int index = 0;
        if (++index == 10)
        {
            QString format;
            QString message;
            format.append("%s: size=%d, lo=%d, co=%d, go=%d");
            message.sprintf(
                format.toLocal8Bit().data(), 
                field->name().toLocal8Bit().data(), 
                field->size(), 
                field->localDataOffset(),
                field->childDataOffset(), 
                field->globalDataOffset()
                );
            message.replace("%f", "%%f");
            qDebug(message.toLocal8Bit().data());
            index = 0;
        }
#endif
        if (*((int*)field) == 0x00000000)
        {
            // 在ARM上如果虚函数表为空则会导致段错误 [12/9/2016 CHENHONGHAO]
            qCritical() << "Invalid Field:" << ((void*)field);
            quint32* dumpBase = (quint32*)field;
            qCritical() << "  ADDRESS  |   DATA  ";
            for (int i = 0; i < 32; ++i)
                qCritical("0x%08x | %08x", &dumpBase[i - 16], dumpBase[i - 16]);
#ifndef NDEBUG
            // 调试期间应该让代码崩溃，以协助发现问题
            fflush(stdout);
            fflush(stderr);
            // 当字段对象严重损坏时，获取其成员变量可能导致崩溃，因此在此之前必须将之前的打印信息输出完
            // 但是凭调试经验，m_parent基本没有被刷坏过
            qCritical() << "Field.name=" << (field ? field->name() : QString());
            Q_ASSERT(0);
#else
            return false;
#endif
        }
        if (!field->updateRawData((const void*)m_basePtr, m_channel))
            return false;
        return field->isVirtual(); // 此处返回false仅为了保证ChannelField子字段不被遍历，从而加速处理
    }

public:
    UpdateRawDataVisitor(void* basePtr, int channel) : RawDataVisitor(basePtr, channel)
    {

    }
};

class CommitRawDataVisitor : public RawDataVisitor
{
    bool visit(IField* field) Q_DECL_OVERRIDE
    {
        if (!field->commitRawData(m_basePtr, m_channel))
            return false;
#if 0
        // 目前没发现需要使用此过滤逻辑 [5/5/2016 CHENHONGHAO]
        return field->isVirtual(); // 此处返回false仅为了保证ChannelField子字段不被遍历，从而加速处理
#else
        return true;
#endif
    }

public:
    CommitRawDataVisitor(void* basePtr, int channel) : RawDataVisitor(basePtr, channel)
    {

    }
};

bool ChannelFactory::createChannel(IField* field)
{
    Q_ASSERT(field);
    QString autoMask = field->channelMask();
#if 1
    // TODO: 此处在F280中未启用，最终是否需要此处代码？待张文博确认
    // 张文博已确认要有这句话 [4/20/2017 CHENHONGHAO]
    if (autoMask.isEmpty())
    {
        autoMask = "Default";
    }
#endif
    QList<IField*>::const_iterator childIter = field->children().cbegin();
    int childCount = field->childCount();
    int channelIndex = 0;
    int maskBeginOffset = 0;
    int maskEndOffset = -1;
    static QLatin1Char splitter('|');
    int maskLength = autoMask.length();
    do
    {
        maskEndOffset = autoMask.indexOf(splitter, maskBeginOffset);
        QStringRef part = autoMask.midRef(maskBeginOffset, maskEndOffset < 0 ? -1 : maskEndOffset - maskBeginOffset);
        maskBeginOffset = maskEndOffset + 1;

		int partLength = part.length();
        if (partLength <= 0)
            continue;

        QStringRef channelName = part;
        if (channelIndex++ >= childCount)
        {
            IField* channelField = createChannelField(channelName.toString(), field);
            if (channelField)
            {
                field->addChild(channelField);
            }
        }
        else
        {
            ChannelField* existingChannel = reinterpret_cast<ChannelField*>(*childIter);
            ++childIter;

            bool equal = existingChannel->name() == channelName;
            if (!equal)
            {
                existingChannel->setChannelName(channelName.toString());
            }
        }

    } while (((uint)maskEndOffset) < ((uint)maskLength));


    // 移除变更后无用的旧通道
    while (childCount-- > channelIndex)
    {
        IField* child = field->children().back();
        field->removeChild(child);
    }

    if (!field->children().isEmpty())
    {
        int channelCount = field->childCount();
        field->setChannelCount(channelCount);
    }

    return true;
}

IField* ChannelFactory::createChannelField(const QString& name /*= QString()*/, IField* parent /*= NULL*/, int initialChildIndex /*= -1*/)
{
    IField* field = new ChannelField(name, parent, initialChildIndex);
    return field;
}

//构造函数
IField::IField()
    : m_name()
    , m_text()
    , m_path()
    , m_channelMask()
    , m_enabled(true)
    , m_parent(NULL)
    , m_localDataOffset(0)
    , m_globalDataOffset(-1)
    , m_fieldIO(NULL)
    , m_validField(NULL)
    , m_notifySlot()
{
    m_children.clear();
}

IField::IField(const IField& other)
    : m_parent(NULL)
    , m_localDataOffset(0)
    , m_globalDataOffset(-1)
    , m_fieldIO(NULL)
    , m_validField(NULL)
{
#if ENABLE_QOBJECT_API
    setObjectName(other.objectName());
#endif
    m_enabled          = other.m_enabled;
    m_channelMask      = other.m_channelMask           ;
    m_name             = other.m_name                  ;
    m_text             = other.m_text                  ;
    m_path             = other.m_path                  ;
    m_notifySlot       = other.m_notifySlot            ;
    m_editorType       = other.m_editorType;
    if (other.m_parent)
    {
        m_parent = other.m_parent;
    }
    if (other.m_fieldIO)
    {
        m_fieldIO = other.m_fieldIO;
    }
    if (other.m_validField)
    {
        m_validField = other.m_validField;
    }
    m_globalDataOffset = other.m_globalDataOffset;
    m_localDataOffset = other.m_localDataOffset;
    // 主动创建对应的ChannelField
    setChannelMask(other.m_channelMask);

    // FIXME: Clone ValidField?
}
IField::~IField()
{
    if (m_fieldIO)
    {
        delete m_fieldIO;
        m_fieldIO = NULL;
    }
}
// 定义静态成员变量
QHash<QString, IField*> IField::s_cache;
QMutex  IField::s_lock;
IField* IField::s_root = NULL;
ChannelFactory* IField::s_channelFactory = NULL;
FieldReseter* IField::s_fieldReset = NULL;

/*! 
 * 复位当前字段，主要指清除缓存操作，例如globalDataOffset
 */
void IField::reset()
{
    if (!m_channelMask.isEmpty())
        setChannelMask(m_channelMask);
    
    m_globalDataOffset = -1;
    m_path.clear();
}

/*! 
 * 在此静态方法中清除cache，在modelReset信号发出后自动使用
 */
void IField::resetAll()
{
    s_lock.lock();
    s_cache.clear();
    s_lock.unlock();

    if (s_channelFactory)
    {
        s_channelFactory->resetChannel();
    }

    /*
     * @brief 此处直接嵌套调用各个子类重新实现的resetAll()
     */
    if (s_fieldReset)
    {
        s_fieldReset->resetAll();
    }

    // 考虑到谐波相关字段是在resetAll期间动态构建的，且在这期间调用了IField::at，这里需要再次清除 [1/22/2016 CHENHONGHAO]
    s_lock.lock();
    s_cache.clear();
    s_lock.unlock();
}

/*! 
 * 克隆当前对象，返回一个重名但数据独立的新字段对象
 * @return     IField*
 */
IField* IField::clone() const
{
#if 0
    // FIXME: 此处借助元对象系统来创建新对象，缺点是要求每一个字段类均使用了Q_OBJECT宏，但这个苛刻条件在F280中是可以满足的
    return qobject_cast<IField*>(this->metaObject()->newInstance());
#endif

    const QMetaObject* metaObject = this->metaObject();
    int constructorCount = metaObject->constructorCount();
    const char* metaType = metaObject->className();
    int metaTypeId = QMetaType::type(metaType);
    if (metaTypeId == QMetaType::UnknownType)
    {
        qCritical() << "Failed to clone IField of " << metaType << " type, you may need to register this class to the Meta-System.";
        return NULL;
    }

    return reinterpret_cast<IField*>(QMetaType::create(metaTypeId, this));
}

typedef QPair<IField*, IField*> FieldPair;
/*! 
 * 在此静态方法中复制树的指定分支，其中每一个字段都被创建了一个同名的字段对象，其对应的数据被复制
 * @param[in] root 操作的根节点
 * @return    操作成功则返回复制的新的根节点对象，但需要注意该对象不能被mount到同一个字段树上，否则会产生重名错误
 */
IField* IField::cloneTree(IField* root)
{
    if (root == NULL)
        return NULL;

    IField* clonedRoot = NULL;

    // first=字段，second=克隆后的父字段
    QQueue< FieldPair > queue;
    queue.enqueue(FieldPair(root, NULL));
    while(!queue.empty()) 
    {
        FieldPair pair = queue.dequeue();
        IField* field = pair.first;
        IField* clonedParent = pair.second;

        if (field == NULL)
            continue;
        
        IField* cloned = field->clone();
        if (clonedParent) 
        {
            clonedParent->addChild(cloned);
        }
        else
        {
            clonedRoot = cloned;
        }

        foreach (IField* f, field->m_children) 
        {
            queue.enqueue(FieldPair(f, cloned));
        }
    }

    return clonedRoot;
}

/*! 
 * 将指定的字段及其子字段挂载至指定路径
 * @param[in]  field 需要挂载的字段
 * @param[in]  path  字段挂载的目标路径（xpath）
 * @return     该路径原来挂载的字段，如果不存在则返回NULL
 */
bool IField::mount(IField* field, const QString& path)
{
    if (field == NULL || path.isEmpty())
        return false;

    if (!umount(path))
        return false;
    
    if (!path.startsWith(QChar('/')))
    {
        qDebug() << "Mount point [" << path <<"] does not start with '/'" << endl;
        return false;
    }

    bool isReplaceMode = !path.endsWith(QChar('/'));
    QStringList names = path.split(QChar('/'));
    IField* parent = IField::root();
    if (parent == NULL)
    {
        // 如果根节点尚不存在则创建根节点
        parent = new VirtualField();
        setRoot(parent);
    }

    // 指定的路径为字段，则替换该字段，替换完成后将字段名最后一个排除（因为会在原位置覆盖）
    if (isReplaceMode)
    {
        field->setName(names.back());
    }

    // 排除根节点对应的空白字段名
    names.pop_front();
    names.pop_back();

    for (QStringList::const_iterator it = names.cbegin(); it != names.cend(); ++it)
    {
        IField* child = parent->child(*it);
        if (child == NULL)
        {
            // 如果当前字段不存在子字段，则添加之
            child = new VirtualField(*it);
            parent->addChild(child);
            // 重新挂载时需要清零各个偏移量，假设前提：各个model之间的数据来源是独立的  [4/5/2016 2015113]
            child->m_localDataOffset = 0;
            child->m_globalDataOffset = 0;
        }

        parent = child;
    }
    parent->addChild(field);
    // 重新挂载时需要清零各个偏移量，假设前提：各个model之间的数据来源是独立的  [4/5/2016 2015113]
    field->m_localDataOffset = 0;
    field->m_globalDataOffset = 0;

    return true;
}

/*! 
 * 将指定路径的字段从该路径移除（解除挂载）
 * @param[in]  path 字段挂载的路径（xpath）
 * @return     该路径原来挂载的字段，如果不存在则返回NULL
 */
bool IField::umount(const QString&  path)
{
    IField* field = IField::at(path);
    if (field == NULL)
        return true;

    // 此处会导致EventListField以及WaveField等字段的内部数据被异常复位 [10/16/2015 CHENHONGHAO]
    //IField::resetAll();

    IField* parent = field->parent();
    if (parent == NULL)
    {
        // 正在卸载根节点，只需将记录根节点对象的静态指针成员置空
        setRoot(NULL);
        return true;
    }
    
    return parent->removeChild(field) >= 0 ? true : false;
}

IField* IField::at(const QString&  fieldPath, IField* curField /* = 0 */)
{
    // 当根节点尚不存在时，不可能存在任何其他节点
    if (fieldPath.isNull() || fieldPath.isEmpty() || s_root == NULL || !fieldPath.contains("/"))
        return NULL;

    IField* cachedField = NULL;
    s_lock.lock();
    QHash<QString, IField*>::const_iterator cacheIter = s_cache.constFind(fieldPath);
    if (cacheIter != s_cache.cend())
        cachedField = cacheIter.value();
    s_lock.unlock();
    // 在hash中找到节点，则直接返回
    if (cachedField)
        return cachedField;


    IField *parent = curField ? curField : s_root;
    QStringList names = fieldPath.split("/");
    if (fieldPath.startsWith(QChar('/')))
    {
        parent = s_root;
        names.pop_front();
    }
    if (fieldPath.endsWith(QChar('/')))
    {
        names.pop_back();
    }
    for (QStringList::const_iterator it = names.cbegin(); it != names.cend(); ++it)
    {
        IField* child = parent->child(*it);

        // 如果当前字段不存在子字段，则说明已经出错
        if (child == NULL)
            return NULL;

        parent = child;
    }

    s_lock.lock();
    // 将查找结果添加至缓存
    s_cache.insert(fieldPath, parent);
    s_lock.unlock();

    return parent;
}

/*! 
 * 可与QObject的ObjectName复用
 */
QString IField::name() const
{
#if ENABLE_QOBJECT_API
    if (!m_name.isEmpty())
        return m_name;

    return objectName();
#else
    return m_name;
#endif
}

/*! 
 * 设置当前字段的字段名
 * @param[in]  name 字段名
 */
void IField::setName(const QString& name)
{
    m_name = name;
    m_path.clear();
#if ENABLE_QOBJECT_API || 1
    setObjectName(m_name);
#endif
}

void IField::setRelativeOffset(const QString& str)
{
    if (str.startsWith("+")) 
    {
        int offset = str.mid(1).toInt();
        if (m_parent)
        {
            this->m_localDataOffset = this->m_localDataOffset + offset;
        }
    }
    else if (str.startsWith("-"))
    {
        int offset = str.mid(1).toInt();
        if (m_parent)
        {
            this->m_localDataOffset = this->m_localDataOffset - offset;
        }
    }
    else 
    {
        int offset = str.toInt();
        this->m_localDataOffset = offset;
    }
}

/*! 
 * 获取字段路径
 * @return 字段路径
 */
QString& IField::path()
{
    if (!m_path.isEmpty())
        return m_path;

    m_path = '/' + m_name;
    if (m_parent == NULL)
        return m_path;

    for (IField* parent = m_parent; parent->m_parent; parent = parent->m_parent)
        m_path.prepend('/' + parent->m_name);

    return m_path;
}

/*! 
 * 获取当前节点在父节点的所有子节点中的序号
 * @return     当前节点在父节点的所有子节点中的序号
 */
int IField::childIndex() const
{
    if (m_parent == NULL)
        return 0;

    int siblingCount = m_parent->childCount();
    for (int i = 0; i < siblingCount; i++)
    {
        if (m_parent->child(i) == this)
            return i;
    }

    qDebug() << "Failed to find myself in my parent's child-list." << endl;

    return -1;
}

/*! 
 * 根据子字段的序号获取子字段对象
 * @param[in]  index 子字段序号
 * @return     子字段对象
 */
IField* IField::child(int index) const
{
    if (index < 0 || index >= (int)m_children.size())
        return NULL;

    return m_children.at(index);
}

/*! 
 * 根据子字段的名称获取子字段对象
 * @param[in]  name 子字段的名称
 * @return     子字段对象
 */
IField* IField::child(const QString& name) const
{
    QList<IField*>::const_iterator it = m_children.cbegin();
    int count = m_children.size();
    while (count--)
    {
        IField* field = *it++;
        QString curName = field->name();
        if (curName.startsWith(QChar('@')))
        {
            curName = curName.mid(1, -1);//remove(QChar('@'));
        }
        if (curName == name)
            return field;
    }

    return NULL;
}

/*! 
 * 设置当前字段的父字段
 * @param[in]  parent 父字段
 */
void IField::setParent(IField* parent)
{
    if (parent)
    {
        parent->addChild(this);
    }
    else
    {
        m_parent = parent;
#if ENABLE_QOBJECT_API
        QObject::setParent(parent);
#endif
    }
}

/*! 
 * 在当前字段下挂载一个新的子字段对象
 * @param[in]  child 子字段对象
 * @return     挂载后的当前字段的子字段个数
 */
int IField::addChild(IField* child)
{
    if (child == NULL)
        return -1;

    // 注意递归调用问题（QObject::setParent非虚）
    //child->setParent(this);
    child->m_parent = this;
#if ENABLE_QOBJECT_API
    ((QObject*)child)->setParent(this);
#endif

    // 替换原来的同名但不是同一个对象的节点（如果存在的话）
    QList<IField*>::iterator it = m_children.begin();
    int count = m_children.size();
    while (count--)
    {
        IField* field = (*it);
        // 避免添加m_name为空的子字段被替换,262、260m_name均为QString("")
        if (!field->name().isEmpty() && field->name() == child->name())
        {
            if (field != child)
            {
                field->setParent(NULL);
                m_children.erase(it);
            }
        }

        ++it;
    }

    // 计算字段偏移量
    int localOffset = 0;
    if (m_children.size())
    {
        IField* back = m_children.back();
        localOffset = back->localDataOffset() + back->size();
    }
    // 先提供一个默认局部偏移量，后面可以通过relativeOffset再进行相对调整
    child->m_localDataOffset = localOffset;
    Q_ASSERT(child->m_localDataOffset < 1 * 1024 * 1024);

    m_children.push_back(child);

    return m_children.size();
}

/*! 
 * 在当前字段下解除某个子字段的挂载关系
 * @param[in]  child 子字段对象
 * @return     解除挂载后的当前字段的子字段个数，操作失败则返回-1
 */
int IField::removeChild(IField* child)
{
    if (child == NULL)
        return -1;

    // 此处会导致EventListField以及WaveField等字段的内部数据被异常复位 [10/16/2015 CHENHONGHAO]
    //IField::resetAll();

    child->setParent(NULL);

    int index = m_children.indexOf(child);
    if (index < 0)
        return -1;

    m_children.removeAt(index);
    return m_children.size();
}

void IField::removeAllChildren()
{
    foreach (IField* child, m_children)
    {
        delete child;
    }
    m_children.clear();
}

IField* IField::takeAt(int index)
{
    IField* childField = child(index);
    removeChild(childField);
    return childField;
}

/*! 
 * 获取子字段个数
 * @return 子字段个数
 */
int IField::childCount() const
{
    return m_children.length();
}

/*! 
 * 获取使能了的直系子字段列表
 * @return     使能了的直系子字段列表
 */
QList<IField *> IField::enabledChildren() const
{
    QList<IField *> result;

    foreach (IField* child, m_children)
    {
        if (child && child->enabled()) 
        {
            result.push_back(child);
        }
    }

    return result;
}

/*! 
 * 获取使能了的子字段个数
 * @return 使能了的子字段个数
 */
int IField::enabledChildCount() const
{
    return enabledChildren().length();
}

/*! 
 * 设置通道掩码 ,设置当前字段使能了哪些channel，通常该字段必须为DataField。
 * @param[in]  mask 掩码值
 * @return     操作成功返回true
 */
bool IField::setChannelMask(const QString& mask)
{
    m_channelMask = mask;

    // 没有指定channelFactory时使用默认ChannelFactory
    if (!s_channelFactory)
    {
        s_channelFactory = new ChannelFactory();
    }

    if (s_channelFactory)
    {
        bool ok = s_channelFactory->createChannel(this);
        return ok;
    }

    return false;
}

/*! 
 * 比较运算符，用于在数据模型中对字段进行排序，子类根据需要可以重写
 * @param[in]  right 运算符右侧的字段
 * @return     this < right
 * @note       默认通过字段的xpath进行比较
 */
bool IField::operator < (const IField& right) const
{
    // 实际上path是不会修改成员变量的，此处为了保证效率
    QString& ls = (const_cast<IField*>(this))->path();
    QString& rs = (const_cast<IField*>(&right))->path();

    return ls < rs;
}

int IField::globalDataOffset() const
{
    // 如果已经计算过绝对偏移量，则直接返回
    if (m_globalDataOffset != -1)
        return m_globalDataOffset;

    int offset = m_localDataOffset;
    if (m_parent)
    {
        offset += m_parent->globalDataOffset() + m_parent->childDataOffset();
    }
#if 0
    for (IField* parent = m_parent; parent; parent = parent->m_parent)
        offset += parent->childDataOffset() + parent->localDataOffset();
#endif

    m_globalDataOffset = offset;

    return offset;
}

/*! 
 * 判断字段是否有效，默认有效
 * @param[in] channel 判断的通道
 * @return    有效则返回true
 * @note      若字段无效，则此时界面应该有所变化，例如显示变灰
 */
bool IField::isValid(int channel) const
{
    if (m_validField == NULL)
        return true;

    return m_validField->data(channel).toBool();
}

/*! 
 * 获取字段的flags，通常仅被FieldModel的同名函数遍历调用
 * @param[in]  与FieldModel的形参含义相同，详见FieldModel
 *
 * @note       注意IField以及其子类的flags|data|setData三个方法中的row和column参数不是给FieldModel使用的，
 *             而是给其子Field（通常是SubDataField）使用的。当FieldModel调用这三个方法时，需要传入-1。
 */
Qt::ItemFlags IField::flags(int row, int column)
{
    Q_UNUSED(row);
    Q_UNUSED(column);
    //return Qt::NoItemFlags;
    return Qt::ItemIsEnabled;
}

/*! 
 * 获取字段的数据，通常仅被FieldModel的同名函数遍历调用
 * @param[in]  与FieldModel的形参含义相同，详见FieldModel
 *
 * @note       注意IField以及其子类的flags|data|setData三个方法中的row和column参数不是给FieldModel使用的，
 *             而是给其子Field（通常是SubDataField）使用的。当FieldModel调用这三个方法时，需要传入-1。
 */
QVariant IField::data(int row, int column, int role)
{
    Q_UNUSED(row);
    Q_UNUSED(column);

    switch (role)
    {
    case FieldNameRole:
        return name();

    case FieldTextRole:
        return text();

    case FieldPathRole:
        return path();

    case IsEnabledRole:
        return enabled();

    case EditorTypeRole:
        if (m_editorType.isEmpty())
        {
            return editorHints();
        }
        return m_editorType;

    case FieldMetaTypeRole:
        return QVariant(int(type()));

    default:
        //qDebug("Failed to get field data, role=%d.\n", role);
        return QVariant();
    }
}

/*! 
 * 设置字段的数据，通常仅被FieldModel的同名函数遍历调用
 * @param[in]  与FieldModel的形参含义相同，详见FieldModel
 *
 * @note       注意IField以及其子类的flags|data|setData三个方法中的row和column参数不是给FieldModel使用的，
 *             而是给其子Field（通常是SubDataField）使用的。当FieldModel调用这三个方法时，需要传入-1。
 */
bool IField::setData(int row, int column, const QVariant& newData, int role)
{
    Q_UNUSED(row);
    Q_UNUSED(column);

    switch (role)
    {
    case FieldNameRole:
        setName(newData.toString());
        return true;

    case FieldTextRole:
        setText(newData.toString());
        return true;

    case FieldPathRole:
    {
        QString newName = newData.toString();
        QString oldPath = parent() ? parent()->path() : "";
        if (!oldPath.isEmpty() && newName.contains(oldPath))
        {
            // 不允许修改根字段的名称
            // 仅当新的路径与原路径位于同一父字段下时，允许修改（实际上仅支持修改字段名）
            setName(newName.replace(oldPath, ""));
            return true;
        }

        return false;
    }

    case IsEnabledRole:
        setEnabled(newData.toBool());
        return true;

    case EditorTypeRole:
        setEditorType(newData.toString());
        return true;

    default:
        return false;
    }
}

/*! 
 *  更新当前字段所对应的原始数据（RawData=>FieldTree）
 * @param[in]  basePtr   原始数据包的基地址（字段对应的原始数据的基地址=数据包基地址+字段全局数据偏移量）
 * @param[in]  channel   原始数据包对应的接线单元的序号，通常为0-7
 */
bool IField::updateRawData(const void* basePtr, int channel)
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
bool IField::commitRawData(void* basePtr, int channel) const
{
    Q_UNUSED(basePtr);
    Q_UNUSED(channel);
    return true;
}

/*! 
 * 在此静态方法中更新树的指定分支的原始数据
 * @param[in]  root      操作的根节点
 * @param[in]  basePtr   原始数据包的基地址（字段对应的原始数据的基地址=数据包基地址+字段全局数据偏移量）
 * @param[in]  channel   原始数据包对应的接线单元的序号，通常为0-7
 * @return    操作全部成功则返回true
 */
bool IField::updateRawDataTree(IField* root, const void* basePtr, int channel)
{
    UpdateRawDataVisitor visitor(const_cast<void*>(basePtr), channel);
    return visitTree(root, &visitor);
}

/*! 
 * 在此静态方法中提交树的指定分支的原始数据
 * @param[in]  root      操作的根节点
 * @param[in]  basePtr   原始数据包的基地址（字段对应的原始数据的基地址=数据包基地址+字段全局数据偏移量）
 * @param[in]  channel   原始数据包对应的接线单元的序号，通常为0-7
 * @return    操作全部成功则返回true
 */
bool IField::commitRawDataTree(IField* root, void* basePtr, int channel)
{
    CommitRawDataVisitor visitor(const_cast<void*>(basePtr), channel);
    return visitTree(root, &visitor);
}

#define ENABLE_FAST_QUEUE                   1

template<class T>
class FastQueue
{
public:
    /*! 
     * 构造函数
     * @param[in]  size 必须为2^N的正整数，例如512,1024,2048
     * @note       需要注意本类不做内存自适应，如果超出初始容量将导致数据出错
     */
    FastQueue(int size = 4096, T* memory = NULL) : 
      m_size(size), m_memory(memory), m_writeOffset(0), m_readOffset(0), m_fromHeap(false)
    {
        if (m_memory == NULL && size > 0)
        {
            m_memory = new T[m_size];
            m_fromHeap = true;
        }

        m_size--;
    }

    ~FastQueue()
    {
        if (m_memory && m_fromHeap)
        {
            delete[] m_memory;
            m_memory = NULL;
        }
    }

    bool empty() const
    {
        return m_readOffset == m_writeOffset;
    }

    bool full() const
    {
        if (m_readOffset+m_size <= m_writeOffset)
            return true;

        return false;
    }

    T dequeue()
    {
#if defined(_DEBUG) && defined(_WIN32)
        Q_ASSERT(!empty());
#endif
        return m_memory[m_readOffset++ & m_size];
    }

    void enqueue(const T& field)
    {
#if defined(_DEBUG) && defined(_WIN32)
        // 如果在此处断言，只需增加构造函数的默认参数size值 [5/5/2016 CHENHONGHAO]
        Q_ASSERT(!full());
#endif
        m_memory[m_writeOffset++ & m_size] = field;
    }

    void append(const QList<T>& fieldList)
    {
        typename QList<T>::const_iterator iter = fieldList.cbegin();
        typename QList<T>::const_iterator cend = fieldList.cend();
        for (; iter != cend; ++iter)
            enqueue(*iter);
    }

protected:
    T*  m_memory;
    int m_size;
    int m_writeOffset;
    int m_readOffset;
    bool m_fromHeap;
};

/*! 
 * 使用指定的访问器来遍历字段树的整个分支
 * @param[in]  root    操作的根节点
 * @param[in]  visitor 指定的访问器（代表某种字段操作）
 * @return     整个分支的字段全部操作成功返回true，否则返回false
 * @note       请注意此函数的实现已高度优化，请勿随意修改或扩展，如有需要，请实现自己的Visitor子类
 */
bool IField::visitTree(IField* root, FieldVisitor* visitor)
{
    if (NULL == root || NULL == visitor)
        return false;

    bool ok = true;
#if ENABLE_FAST_QUEUE
    static IField* queueBuffer[4096];
    FastQueue<IField*> queue(sizeof(queueBuffer)/sizeof(queueBuffer[0]), queueBuffer);
#else
    QQueue<IField*> queue;
#endif

    queue.enqueue(root);
    while (!queue.empty()) 
    {
        IField* field = queue.dequeue();
        if (!visitor->visit(field))
        {
            ok = false;
            continue;
        }

        queue.append(field->m_children);
    }

    return ok;
}

bool ResetVisitor::visit(IField* field)
{
    field->reset();
    return field->isVirtual(); // 此处返回false仅为了保证ChannelField子字段不被遍历，从而加速处理
}

bool FieldIOVisitor::visit(IField* field)
{
    if (!field->isVirtual() && !field->fieldIO())
    {
        FieldIO * curFieldIO = field->createFieldIO();
        if (curFieldIO)
        {
            field->setFieldIO(curFieldIO);
        }
    }
    return field->isVirtual(); // 此处返回false仅为了保证ChannelField，子字段不被遍历，从而加速处理
}

/*********************************************************************************************************
** End of file
*********************************************************************************************************/
