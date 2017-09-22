/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  FieldProxyModel.cpp
** Latest Version:             V1.0.0
** Latest modified Date:       2017/2/16
** Modified by:                ZhangWenBo
** Descriptions:               
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 Han ZhenFei
** Created date:               2015/10/15
** Descriptions:               
** 
*********************************************************************************************************/

#include "FieldProxyModel.h"
#include <QDebug>
// 构造函数和析构函数实现

FieldProxyModel::FieldProxyModel(QObject *parent /* = 0 */) 
    : QAbstractProxyModel(parent)
    , m_reg("")
    , m_regFlag(false)
{
  
}

FieldProxyModel::FieldProxyModel(const FieldProxyModel& other)
{
    Q_UNUSED(other);
}

FieldProxyModel::~FieldProxyModel()
{

}

QHash<int, QByteArray> FieldProxyModel::roleNames() const
{
    static QHash<int, QByteArray> hash;
    static bool once = true;
    if (once)
    {
        hash.clear();
        hash[FieldDisplayRole] = QByteArray("display");
        hash[FieldDecorationRole] = QByteArray("decoration");
        hash[FieldEditRole] = QByteArray("edit");
        hash[FieldToolTipRole] = QByteArray("toolTip");
        hash[FieldStatusTipRole] = QByteArray("statusTip");
        hash[FieldWhatsThisRole] = QByteArray("what");
        hash[FieldSizeHintRole] = QByteArray("sizeHint");
        hash[FieldNameRole] = QByteArray("name");
        hash[FieldTextRole] = QByteArray("text");
        hash[FieldPathRole] = QByteArray("path");
        hash[IsEnabledRole] = QByteArray("enable");
        hash[FieldMetaTypeRole] = QByteArray("metaType");
        hash[MaxValueRole] = QByteArray("max");
        hash[MaxValueTextRole] = QByteArray("maxText");
        hash[MinValueRole] = QByteArray("min");
        hash[MinValueTextRole] = QByteArray("minText");
        hash[DefaultValueRole] = QByteArray("default");
        hash[StepValueRole] = QByteArray("step");
        hash[UnitRole] = QByteArray("unit");
        hash[DimensionRole] = QByteArray("dimension");
        hash[DigitalsRole] = QByteArray("digital");
        hash[IsCustomChoicesRole] = QByteArray("isChoices");
        hash[CustomChoicesRole] = QByteArray("choices");
        hash[CustomChoiceTextsRole] = QByteArray("choiceTexts");
        hash[CustomChoiceSelectionsRole] = QByteArray("choiceSelections");
        hash[EditorTypeRole] = QByteArray("editorType");
        hash[FieldDataRole] = QByteArray("value");
        hash[FieldDataTextRole] = QByteArray("dataText");
        hash[TrendDataRole] = QByteArray("trend");
        hash[ThumbnailDataRole] = QByteArray("thumbnail");
        hash[IsOverloadedRole] = QByteArray("overLoad");
        hash[IsInvalidRole] = QByteArray("invalid");
        hash[PhaseColorRole] = QByteArray("phaseColor");
        hash[ViewColumnHintRole] = QByteArray("columnHint");
        hash[CursorPositionRole] = QByteArray("cursorPosition");
        hash[MarkerDataRole] = QByteArray("markerData");
        hash[CursorVisibleRole] = QByteArray("cursorVisible");
        hash[FrameSizeRole] = QByteArray("frameSize");
        hash[PointSizeRole] = QByteArray("pointSize");
        hash[ChannelCountRole] = QByteArray("channelCount");
        hash[FlushNumberRole] = QByteArray("flushNumber");
        once = false;
    }

    return hash;
}

// 各种方法实现
QRegExp FieldProxyModel::filterRegExp() const
{
    return m_reg;
}

QString FieldProxyModel::filterRegValue() const
{
    return m_reg.pattern();
}

void FieldProxyModel::setFilterRegExp(const QRegExp &regExp)
{
    m_reg = regExp;   
    m_reg.setPatternSyntax(QRegExp::W3CXmlSchema11);    // 设置接受的正则表达式模型
    m_regFlag = regExp.pattern().isEmpty() ? false : true;
}

void FieldProxyModel::setRootName(const QString & name)
{    
    m_rootName = name;
    IField* f = IField::at(m_rootName);
    if (f)
    {
        m_Xpath = m_rootName;   
    }
    else
    {
        qWarning() << "Find path error, Path:" << m_rootName;
    }
}

void FieldProxyModel::resetProxyModel()
{
    source_index_mapping.clear();
    m_rootList.clear();
    FieldModel *model = (FieldModel *)(sourceModel());
    if (!model)
        return;
    foreach(QString str, m_whiteList)
    {  
        m_rootList << model->index(str);
    }

    beginResetModel();
    endResetModel();
}

const QStringList& FieldProxyModel::whiteList() const
{
    return m_whiteList;
}

void FieldProxyModel::setWhiteList(const QStringList& list)
{
    // 一旦设置了白名单，重新开始映射
    m_whiteList.clear();
    
    if (list.isEmpty())
    {
        return;
    }
    m_whiteList <<list;
                
    resetProxyModel();
    // 复位当前model [12/11/2015 CHENHONGHAO]

    emit whiteListChanged();
#if 0
    if (objectName() == "recordModel")
    {
        int a = 0;
    }
#endif

}

void FieldProxyModel::setReg(const QString& str)
{
    if (str.isEmpty())
    {
        return;
    }  
    setFilterRegExp(QRegExp(str));     
}


QModelIndex FieldProxyModel::index(int row, int column, const QModelIndex & parent /* = QModelIndex */) const
{
#if _DEBUG1
    Dbg << QString ("%1").arg(source_index_mapping.count());
#endif
    if (row < 0 || column < 0)
    {
        return QModelIndex();
    }

    if (!parent.isValid())
    {
        return getRootIndex(row, column);
    }
    else
    {
        QString p = parent.data(FieldPathRole).toString();
        Mapping* m = (Mapping*)parent.internalPointer();
        QModelIndex srcParent = source_index_mapping.key(m);

        if (m_regFlag && isLastParent(srcParent))
        {
            QVector<int> res = filterSrcRow(srcParent);
            if (row >= res.count())
            {
                qWarning()<<"please invoke rowCount before invoking index to avoid index out of range";
                return QModelIndex();
            }
            return createIndexMapping(res[row], column, srcParent);
        }
        return createIndexMapping(row, column, srcParent);
    }   
}

QModelIndex FieldProxyModel::parent(const QModelIndex& proxIndex) const
{
    FieldModel *model = qobject_cast<FieldModel*>(sourceModel());
    if (!proxIndex.isValid())
    {
        return QModelIndex();
    }
    else
    {
        Mapping *m = (Mapping *)proxIndex.internalPointer();
        QModelIndex d = m->map_iter.key();//source_index_mapping.key(m);
        QModelIndex p = model->parent(d);
        //  如果p 不在hash表中，说明是根节点
        if (!source_index_mapping.contains(p))
        {
            return QModelIndex();
        }
        m = source_index_mapping.value(p);
        return srcToProxy(m->proxyRow, m->proxyColum, p);
    }
}

int FieldProxyModel::columnCount(const QModelIndex &parent) const
{
    FieldModel *model = qobject_cast<FieldModel*>(sourceModel());
    if (!parent.isValid())
    {
        return model->columnCount(m_rootIndex);
    }
    else 
    {
        Mapping *m = (Mapping *)parent.internalPointer();
        QModelIndex d = m->map_iter.key();
        return model->columnCount(d);
    }
}

int FieldProxyModel::rowCount(const QModelIndex &parent ) const
{
    FieldModel *model = qobject_cast<FieldModel*>(sourceModel());
    if (!parent.isValid()) 
    {
        if (m_whiteList.empty())
        {
            if (m_regFlag)
            {
                if (m_Xpath.isEmpty())
                {   
                    return 0;
                }
                QVector<int> res = filterSrcRow(m_rootIndex);
                return res.count();
            }
            else 
            {
                if (m_Xpath.isEmpty())
                {   
                    return 0;
                }
                return model->rowCount(m_rootIndex);
            }
        } else 
        {
            return m_whiteList.count();
        }
    }
    else
    {
        Mapping *m = (Mapping*)parent.internalPointer();
        QModelIndex d = m->map_iter.key();
#if 0
        // 此处占用CPU较多，但本次调试未见调用，若后期无法正常工作，请将此处还原尝试 [11/6/2015 lenovo]
        QString rootPath =d.data(FieldPathRole).toString();
        if (m_whiteList.indexOf(rootPath) == -1) {
            if (m_regFlag) 
                return filterSrcRow(d).count();
        }
#endif

        return model->rowCount(d);
    }
}

QModelIndex FieldProxyModel::createIndexMapping(int row, int column, QModelIndex srcParent) const
{    
    FieldModel *model = qobject_cast<FieldModel*>(sourceModel());
    QModelIndex srcIndex = model->index(row, column, srcParent);
    return srcToProxy(row,column, srcIndex);
}

QModelIndex FieldProxyModel::mapFromSource(const QModelIndex & sourceIndex) const
{
    if (!sourceIndex.isValid())
        return QModelIndex(); // for now; we may want to be able to set a root index later
    if (sourceIndex.model() != this->sourceModel()) 
    {
        qWarning() << "FieldProxyModel: index from wrong model passed to mapFromSource";
        Q_ASSERT(!"FieldProxyModel: index from wrong model passed to mapFromSource");
        return QModelIndex();
    }

    // 指向了不在prxoyModel 范围的内容，直接给root节点
    IndexMap::const_iterator it = source_index_mapping.constFind(sourceIndex);
    Mapping* m = (*it);
    if (it != source_index_mapping.constEnd())
    {
        return createIndex(m->proxyRow, m->proxyColum, m);
    }

    return QModelIndex();
}

/*
*  @brief 注意，这里判断proxyIndex 是否是非法的原因： 是因为如果是非法proxyIndex ，则无法取得Mapping的句柄，此proxyModle 与一般代理不同，因为重新指定了根节点，所以重写index是必然的
*/
QModelIndex FieldProxyModel::mapToSource(const QModelIndex & proxyIndex) const
{
    if (!proxyIndex.isValid())
    {
        return QModelIndex();  
    }

    // 避免映射错误，再次检查
    if (proxyIndex.model() != this) 
    {
        qWarning() << "FieldProxyModel: index from wrong model passed to mapToSource";
        //Q_ASSERT(!"FieldProxyModel: index from wrong model passed to mapToSource");
        return QModelIndex();
    }
    Mapping* m = reinterpret_cast<Mapping*>(proxyIndex.internalPointer());
    return m->map_iter.key();
}

QModelIndex FieldProxyModel::getRootIndex(int row, int column) const
{
    FieldModel *model = reinterpret_cast<FieldModel*>((QAbstractItemModel*)(sourceModel()));
    if (!m_whiteList.empty())
    {
        if (row >=  m_whiteList.count())
        {
            Q_ASSERT(0);
        }
        QModelIndex srcIndex = model->index(m_whiteList[row]);               // 白名单就是List， 不考虑列了
        if (!srcIndex.isValid())
        {
            qWarning() << "white List error:" << m_whiteList[row];       
            return QModelIndex();
        }
        return this->srcToProxy(row, column, srcIndex);
    } 
    else 
    {
        if (m_Xpath.isEmpty())
        {
            qWarning() << "FieldProxyModel::getRootIndex root xPath is empty!" ;
            return QModelIndex();
        }      
        QModelIndex srcIndex = model->index(row, column, m_rootIndex);        // 仅仅指定XPath的情况 
        if (m_regFlag)
        {
           QVector<int> res = filterSrcRow(srcIndex);
           if (!(res.count() > row))
           {
               qWarning()<< "index out of range!";
               return QModelIndex();
           }
           return srcToProxy(res[row], column, srcIndex);          
        }
//         int num = source_index_mapping.count();
//         qDebug() << "QHash Num:" << num;
        return this->srcToProxy(row, column, srcIndex);
    }  
}


bool FieldProxyModel::filterRegExpRow(const QRegExp& regExp, const QModelIndex& srcIndex) const
{  
    return regExp.exactMatch(srcIndex.data(FieldPathRole).toString());
}

//  这里的row ， column是proxy 的row 和column, 本函数仅仅是创建一个属于proxy的index
QModelIndex FieldProxyModel::srcToProxy(int row, int column, const QModelIndex &srcIndex) const
{
    // PLEASE check it! [10/23/2015 lenovo]
    if (!srcIndex.isValid())
        return QModelIndex();

    IndexMap::const_iterator it = source_index_mapping.constFind(srcIndex);
    Mapping *m = NULL;
    if (it != source_index_mapping.constEnd())
    {
        m = *it;
        m->proxyRow = row;
        m->proxyColum = column;
//         QModelIndex newIndex = createIndex(m->proxyRow, m->proxyColum, m);
//         return newIndex;
    }
    else 
    {
        m = new Mapping;
        m->srcRow = srcIndex.row();
        m->srcColum = srcIndex.column();
        m->proxyRow = row;
        m->proxyColum = column;
        IndexMap::const_iterator itItem = source_index_mapping.insert(srcIndex, m);
        m->map_iter = itItem;
    }
    return createIndex(row, column, m);
}

// 目前只有列需要过滤，所以只过滤了列数,注意不允许父索引不在hash中
QVector<int> FieldProxyModel::filterSrcRow(const QModelIndex &srcParent) const
{
    FieldModel *model = const_cast<FieldModel*>(qobject_cast<const FieldModel*>(srcParent.model()));
    if (!model)
    {
        Q_ASSERT(!"FieldProxyModel::filterSrcRow  的srcParent 不是srcModel 的索引！");
    }
    if (!srcParent.isValid())
    {
        qWarning() << "invalid index from source model,pleace to check!";
    }
    QVector<int> res;
    int c = model->rowCount(srcParent);
    for (int i = 0; i < c; ++i)
    {
        QModelIndex srcItem = model->index(i,0, srcParent);
        if (filterRegExpRow(filterRegExp(), srcItem))
        {
            res.push_back(i);
        }
    }
    return res;
}

bool FieldProxyModel::isLastParent(const QModelIndex &srcParent) const
{
    static int mark = 0;
    static bool res = false;
    int count = srcParent.model()->rowCount(srcParent);
    if (count > 0)
    {
        ++mark;
        isLastParent(srcParent.child(0, 0));
    }
    else
    {
        if (mark == 1)
            res = true;
        else 
            res = false;
        mark = 0;
    }
    return res;
}
/*********************************************************************************************************
** End of file
*********************************************************************************************************/
