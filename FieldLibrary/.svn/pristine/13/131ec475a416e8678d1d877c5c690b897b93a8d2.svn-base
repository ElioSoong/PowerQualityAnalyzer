/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  FieldModel.cpp
** Latest Version:             V1.0.0
** Latest modified Date:       2017/2/16
** Modified by:                ZhangWenBo
** Descriptions:               字段模型
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 Zhang WenBo
** Created date:               2015/07/09
** Descriptions:               Field模型
** 
*********************************************************************************************************/
#include <QVariant>
#include <QStack>
#include <QQueue>
#include <QStringList>
#include <QDateTime>
#include <QDebug>
#include <QCoreApplication>
#include "FieldModel.h"
#include "VirtualField.h"

QVector<FieldModel*> FieldModel::s_models;
QStringList FieldModel::s_roleLists;
// ____________________________________ public: 成员函数定义___________________________________________________________

FieldModel::FieldModel(IField * rootField /* = NULL */, const QString& rootPath /* = "" */)
    : m_rootField(rootField)
    , m_rootPath(rootPath)
    , m_dirty(false)
    , m_limitDirty(false)
    , m_lock()
    , m_updateCount(0)
    , m_lastUiNotifyTime(0)
{
    s_models.push_back(this);
    s_roleLists << "FieldDisplayRole"
                << "FieldDecorationRole"
                << "FieldEditRole"
                << "FieldToolTipRole"
                << "FieldStatusTipRole"
                << "FieldWhatsThisRole"
                << "FieldSizeHintRole"
                << "FieldNameRole"
                << "FieldTextRole"
                << "FieldPathRole"           
                << "IsEnabledRole"
                << "FieldMetaTypeRole"              
                << "MaxValueRole"              
                << "MaxValueTextRole"              
                << "MinValueRole"              
                << "MinValueTextRole"              
                << "DefaultValueRole"              
                << "StepValueRole"              
                << "UnitRole"              
                << "DimensionRole"              
                << "DigitalsRole"              
                << "IsCustomChoicesRole"              
                << "CustomChoicesRole"              
                << "CustomChoiceTextsRole"           
                << "CustomChoiceSelectionsRole"
                << "EditorTypeRole" 
                << "FieldDataRole"
                << "FieldDataTextRole"                           
                << "TrendDataRole"              
                << "ThumbnailDataRole"              
                << "IsOverloadedRole"              
                << "IsInvalidRole" 
                << "PhaseColorRole"              
                << "ViewColumnHintRole"              
                << "CursorPositionRole"
                << "MarkerDataRole" ;       
    connect(this, SIGNAL(rawDataUpdated()), this, SLOT(notifyRawDataUpdated()), Qt::QueuedConnection);
}

FieldModel::~FieldModel()
{
    int index = s_models.indexOf(this);
    if (index < 0)
        return;

    s_models.remove(index);
}

/*! 
 * 设置根字段路径
 * @param[in] path  根字段路径
 */
void FieldModel::setRootPath(const QString& path)
{
    m_rootPath = path;
    mountRootField();
}

/*! 
 * 设置根字段
 * @param[in] root  根字段对象
 */
void FieldModel::setRootField(IField* root)
{
    m_rootField = root;
    mountRootField();
}

/*! 
 * 在主线程中使用此内部槽函数响应rawDataUpdated信号，并发出dataChanged信号以刷新各个视图
 */
void FieldModel::notifyRawDataUpdated()
{
    static int updateCount = 0;

    // 正常原始数据包每隔约200ms接收一次，这里设置各个视图约每隔600ms刷新一次 [4/21/2016 CHENHONGHAO]
    // 取消上一行注释对应的600ms刷新机制，直接使用定时器，如果超出500ms，则刷新界面显示（注意不能用于界面的性能分析） [5/25/2016 CHENHONGHAO]
    if (updateCount++ % 3 == 0
            //currentTime >= m_lastUiNotifyTime + 500
     || m_dirty) // 配置视图变更需要立即刷新
    {
        // m_lastUiNotifyTime = currentTime;
        // 不再做无用功：手动设置更新范围为左上角到右下角。直接使用空参数，预期可以完成同样的效果 [6/28/2016 CHENHONGHAO]
        emit dataChanged(QModelIndex(), QModelIndex());
        // 需要注意调用rowCount和columnCount可能会导致statusModel打印警告信息（FieldModel xxx is empty）
        //emit dataChanged(index(0, 0), index(rowCount(QModelIndex()), columnCount(QModelIndex())));
    }
}

/*! 
 *  更新模型各个字段所对应的原始数据（RawData=>FieldTree）
 * @param[in]  basePtr   原始数据包的基地址（字段对应的原始数据的基地址=数据包基地址+字段全局数据偏移量）
 * @param[in]  element   原始数据包对应的接线单元的序号，通常为0-7
 */
void FieldModel::updateRawData(const void* basePtr, int element)
{
#if defined(_DEBUG) && 1
    static int updateCount = 0;
    static qint64 prevUpdateTime = 0;
    static double history = 0;
    qint64 curUpdateTime = QDateTime::currentMSecsSinceEpoch();
    if (prevUpdateTime == 0) 
    {
        prevUpdateTime = curUpdateTime;
    }
    if (updateCount++ % 5 == 0 && updateCount > 0)
    {
        qint64 delta = curUpdateTime - prevUpdateTime;
        prevUpdateTime = curUpdateTime;
        if (history == 0) 
        {
            history = delta;
        } else 
        {
            history = history * 0.9 + delta * 0.1;
        }
    }
#if 0
    if (updateCount % 1000 == 0)
    {
        qDebug() << "FieldModel.updateRawData/second: " << (5000 / (history + 1E-6));
    }
#endif
    //if (updateCount > 10000) QCoreApplication::exit(0);
#endif

    //qDebug("UpdateRawData[E%d]:", element);

    m_lock.lock();
    IField::updateRawDataTree(m_rootField, basePtr, element);
    m_lock.unlock();

#if 1
    if (element == 4)
    {
        // 此处应根据packetTag来判定是否需要刷新视图
        emit rawDataUpdated();
    }
#endif
}

/*! 
 *  提交模型各个字段所对应的原始数据（FieldTree=>RawData）
 * @param[in]  basePtr   原始数据包的基地址（字段对应的原始数据的基地址=数据包基地址+字段全局数据偏移量）
 * @param[in]  element   原始数据包对应的接线单元的序号，通常为0-7
 */
void FieldModel::commitRawData(void* basePtr, int element) const
{
    m_lock.lock();
    IField::commitRawDataTree(m_rootField, basePtr, element);
    m_lock.unlock();
}

void FieldModel::reset()
{
    m_lock.lock();
    IField::resetAll();
    m_lock.unlock();
}

/*! 
 * 根据路径名获取相应Model
 * @param[in]  wildPath 路径名
 * @return  成功则返回数据域类型model
 */
FieldModel *FieldModel::getModel(QString wildPath)
{
    foreach(FieldModel* model, FieldModel::s_models) 
    {
        //  只要wild的路径在 rootPath上，即包含了rootPath，那么则认为此model是需要的model
        if (wildPath.contains(model->m_rootPath)) 
            return model;
    }

    return NULL;
}

/*! 
 * 尝试将当前模型对应的根节点挂载至当前模型对应的xpath处
 */
void FieldModel::mountRootField()
{
    if (!m_rootPath.isEmpty() && m_rootField != NULL)
    {
        IField::mount(m_rootField, m_rootPath);
    }
}

/*! 
 * 通过QModelIndex获取其对应的IField指针，如果IField不存在则返回model对应的rootField
 * @param[in]  index 输入的QModelIndex
 * @return     IField指针
 */
IField* FieldModel::fieldFromIndex(const QModelIndex& index) const
{
    const FieldModel* model = this;//qobject_cast<FieldModel*>(index.model());
    if (model == NULL)
    {
        // The index is not created for a FieldModel object
        return NULL;
    }
    IField *field = index.isValid() ? reinterpret_cast<IField*>(index.internalPointer()) : model->m_rootField;
    return field;
}

/*! 
 * 重写Qt类库的部分虚函数（详见Qt帮助文档-QAbstractItemModel）：
 * [1] 获取数据flags
 * [2] 获取数据
 * [3] 设置数据
 * 
 * @note 这些函数的仅在内部调用对应的IField对象的同名函数，详见IField
 */
QVariant FieldModel::data(const QModelIndex & index, int role /* = Qt::DisplayRole */) const
{
    // 根节点尚未准备好时，无法进行数据操作
    // 另外，当QModelIndex无效时，说明在尝试获取根节点的信息
    if (m_rootField == NULL)
        return QVariant();

    IField *field = FieldModel::fieldFromIndex(index);
    Q_ASSERT(field);
    // 注意每一个QModelIndex只对应一个IField，
    // 调用该字段的接口时，不应使用-1以外的行列号，因为字段的行列号仅供内部使用
    return field->data(index.row(), index.column(), role);
}

/*! 
 * 重写Qt类库的部分虚函数（详见Qt帮助文档-QAbstractItemModel）：
 * [1] 获取数据flags
 * [2] 获取数据
 * [3] 设置数据
 * 
 * @note 这些函数的仅在内部调用对应的IField对象的同名函数，详见IField
 */
Qt::ItemFlags FieldModel::flags(const QModelIndex & index) const
{
    // 根节点尚未准备好时，无法进行数据操作
    if (m_rootField == NULL)
        return  Qt::NoItemFlags;

    IField *field = FieldModel::fieldFromIndex(index);
    Q_ASSERT(field);
    // 注意每一个QModelIndex只对应一个IField，
    // 调用该字段的接口时，不应使用-1以外的行列号，因为字段的行列号仅供内部使用
    return field->flags(-1, -1);
}

/*! 
 * 重写Qt类库的部分虚函数（详见Qt帮助文档-QAbstractItemModel）：
 * [1] 获取数据flags
 * [2] 获取数据
 * [3] 设置数据
 * 
 * @note 这些函数的仅在内部调用对应的IField对象的同名函数，详见IField
 */
bool FieldModel::setData(const QModelIndex & index, const QVariant & value, int role /* = Qt::EditRole */)
{
    // 根节点尚未准备好时，无法进行数据操作
    if (m_rootField == NULL)
        return false;

    IField *field = FieldModel::fieldFromIndex(index);
    Q_ASSERT(field);
    // 注意每一个QModelIndex只对应一个IField，
    // 调用该字段的接口时，不应使用-1以外的行列号，因为字段的行列号仅供内部使用
    bool ok = field->setData(index.row(), index.column(), value, role);
    if (ok)
    {
        emit rawDataUpdated();
    }

    return ok;
}

/*! 
 * 根据指定字段的XPath来生成QModelIndex，仅用于获取单个字段的值，无法用来跟View和Delegate交互
 * @param[in]  fieldPath 指定字段的XPath
 * @return     生成的QModelIndex
 */
QModelIndex FieldModel::index(const QString& fieldPath) const
{
    IField* field = IField::at(fieldPath, m_rootField);
    if (field == NULL)
        return QModelIndex();

    // TODO: 1010 此处如果createIndex(-1, -1, field)，则将导致创建的QModelIndex无效
    // 默认为第0行，第0列，这对配置字段是很好的，但EventListField等可能与之不兼容 [5/5/2016 CHENHONGHAO]
    return createIndex(0, 0, field);
    //return createIndex(0x7FFFFFFF, 0x7FFFFFFF, field);
}

/************************************************************************/
/* Derived virtual function (See QT Manual)                             */
/************************************************************************/
QModelIndex FieldModel::index(int row, int column, const QModelIndex & parent /* = QModelIndex */) const
{
#if 0
    // 对不合法数据做处理
    if (!m_rootField || row < 0 || column < 0) 
        return QModelIndex(); 
#else
    if (!hasIndex(row, column, parent))
        return QModelIndex();
#endif

    IField *p = parent.isValid() ? reinterpret_cast<IField*>(parent.internalPointer()) : m_rootField;
    if (p)
    {
        // 仅针对EventListField等多行多列的字段
        if (row < p->rowCount() && column < p->columnCount())
        {
            return createIndex(row, column, p);
        }

        IField *child = p->child(row);
        if (child)
        {
            // TODO: 1010 此处如果createIndex(-1, -1, field)，则将导致创建的QModelIndex无效
            return createIndex(0x7FFFFFFF, 0x7FFFFFFF, child);
        }
    }
    
    return QModelIndex();
}

/************************************************************************/
/* Derived virtual function (See QT Manual)                             */
/************************************************************************/
int FieldModel::rowCount(const QModelIndex & parent /* = QModelIndex */) const
{
    IField *field = parent.isValid() ? reinterpret_cast<IField*>(parent.internalPointer()) : m_rootField;
    //Q_ASSERT(field);
    if (field == NULL)
    {
        qDebug(
            "FieldModel 0x%08x is empty, this may happen when"
            "a view object is requesting to set its model when initializing.",
            this
            );
        return 0;
    }

    int thisRowCount = field->rowCount();
    if (thisRowCount > 0)
    {
        // 说明当前字段为多行多列的扩展字段，例如EventListField
        return thisRowCount;
    }

    return field->childCount();
}

/************************************************************************/
/* Derived virtual function (See QT Manual)                             */
/************************************************************************/
int FieldModel::columnCount(const QModelIndex &parent /* = QModelIndex */) const
{
    IField *field = parent.isValid() ? reinterpret_cast<IField*>(parent.internalPointer()) : m_rootField;
    //Q_ASSERT(field);
    if (field == NULL)
    {
        qDebug(
            "FieldModel 0x%08x is empty, this may happen when"
            "a view object is requesting to set its model when initializing.",
            this
            );
        return 0;
    }

    int thisColumnCount = field->columnCount();
    if (thisColumnCount > 0)
    {
        // 说明当前字段为多行多列的扩展字段，例如EventListField
        return thisColumnCount;
    }

    return 1;
}

/************************************************************************/
/* Derived virtual function (See QT Manual)                             */
/************************************************************************/
QModelIndex FieldModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    IField *child = reinterpret_cast<IField*>(index.internalPointer());
    Q_ASSERT(child);
    IField* parent = child->parent();
    if (!parent)
        return QModelIndex();

    if (parent == m_rootField)
        return QModelIndex();

    return createIndex(parent->childIndex(), 0, parent); 
}

/************************************************************************/
/* Derived virtual function (See QT Manual)                             */
/************************************************************************/
bool FieldModel::hasChildren(const QModelIndex & parent) const
{
   IField *field  = (IField*)parent.internalPointer();
   if ( !field) {
       return false;
   }
   return field->childCount();
}

/*! 
* 将QString类型的role字段解析成对应的FieldRole
* @param[in]  role 远程通信输入的string类型的role
* @return     IField指针
 */
int FieldModel::parseTheRole(const QString & role)
{
    int theRole = FieldNameRole;

    if (!s_roleLists.contains(role) && role != "Qt::DisplayRole") 
    {
        qDebug() << "Can't Find The Role!!";
        return -1;
    }
    if (role == "Qt::DisplayRole" || role == "FieldDisplayRole")
    {
        theRole = 0;
    }
    else 
    {
        int offset = s_roleLists.indexOf(role);
        qDebug() << "oooFieldModel::offset :" << offset << " namerole:" << FieldNameRole << " datarole:" << FieldDataRole;
        if (offset >= 7)
        {
            theRole = FieldNameRole + offset - 7;
        }
    }

    return theRole;
}

/*! 
 * 字段树的写操作响应槽
 */
bool FieldModel::writeFieldData(const QString &path, const QString& role, QVariant data)
{
    qDebug() << "FieldModel::writeFieldData path = " << path << "role = " << role << "data = " << data;
    int theRole = parseTheRole(role); 
    if (theRole < 0)
        return false;

#if 0
    IField * field = IField::at(path);
    
    if (!field)
        return false;

    bool ok = field->setData(-1, -1, data, theRole);
#else

    bool ok = setData(index(path), data, theRole);
#endif

    if (ok)
    {
        emit rawDataUpdated();
    }
    return ok;
}

/*! 
 * 字段树的读操作响应槽
 */
QVariant FieldModel::readFieldData(const QString &path, const QString& role)
{
    int theRole = parseTheRole(role); 
    qDebug() << "FieldModel::readFieldData path = " << path << "role = " << role << theRole;
    if (theRole < 0)
        return false;

#if 0
    IField * field = IField::at(path);
    //qDebug() << "FieldModel::readFieldData:field " << field;
    if (!field)
        return false;
    QVariant outData = field->data(-1, -1, theRole);
    //qDebug() << "FieldModel::readFieldData:outData " << outData;

#else
    QVariant outData = data(index(path), theRole);
    qDebug() << "FieldModel::readFieldData:outdata" << outData;
#endif
    return outData;
}
/*********************************************************************************************************
** End of file
*********************************************************************************************************/
