#include "includes.h"
#include "ObjectListModel.h"

#include <QMetaObject>
#include <QMetaProperty>

int ObjectListModel::updateRolesFromObject(const QObject* object)
{
    const QMetaObject* metaObject = object->metaObject();
    int count = metaObject->propertyCount();
    m_roleNameList.clear();
    for (int i = 0; i < count; i++)
    {
        QMetaProperty property = metaObject->property(i);
        QString name = QString::fromLocal8Bit(property.name());
        m_roleNameList.push_back(name);
    }

    return m_roleNameList.count();
}

void ObjectListModel::setObjectList(const QObjectList& objectList) 
{
    this->m_objectList = objectList;

    // 根据首个QObject的属性列表自动生成角色列表 [12/10/2015 CHENHONGHAO]
    if (objectList.size() > 0)
    {
        QObject* firstObject = objectList.at(0);
        updateRolesFromObject(firstObject);
    }

    //reset();
}

QObjectList ObjectListModel::getObjectList() const {
    return this->m_objectList;
}

void ObjectListModel::setRoleNameList(const QStringList& roleNameList) {
    this->m_roleNameList = roleNameList;
}

QStringList ObjectListModel::getRoleNameList() const {
    return this->m_roleNameList;
}

int ObjectListModel::rowCount(const QModelIndex & parent) const
{
    Q_UNUSED(parent);
    return count();
}

int ObjectListModel::count() const
{
    return m_objectList.size();
}

QVariant ObjectListModel::data(const QModelIndex & index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role <= Qt::UserRole)
        return QVariant();
        //return QAbstractListModel::data(index, role);

    
    int row = index.row();
    Q_ASSERT(row >= 0 && row < m_objectList.size());
    QObject* object = m_objectList.at(row);
    const QMetaObject* metaObject = object->metaObject();
    Q_ASSERT(role < Qt::UserRole + m_roleNameList.size());
    QString roleName = m_roleNameList.at(role - Qt::UserRole);
    int propertyIndex = metaObject->indexOfProperty(roleName.toLocal8Bit().data());
    Q_ASSERT(propertyIndex > 0);
    return metaObject->property(propertyIndex).read(object);
}

bool ObjectListModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
    if (!index.isValid())
        return false;

    if (role <= Qt::UserRole)
        return QAbstractListModel::setData(index, value, role);


    int row = index.row();
    Q_ASSERT(row >= 0 && row < m_objectList.size());
    QObject* object = m_objectList.at(row);
    const QMetaObject* metaObject = object->metaObject();
    Q_ASSERT(role <= Qt::UserRole + m_roleNameList.size());
    QString roleName = m_roleNameList.at(role - Qt::UserRole);
    int propertyIndex = metaObject->indexOfProperty(roleName.toLocal8Bit().data());
    Q_ASSERT(propertyIndex > 0);
    bool ok = metaObject->property(propertyIndex).write(object, value);

    // 此处千万不能调用notify，否则将导致DeviceSettings.qml中出现大量selectByMouse属性的binding loop [8/5/2016 CHENHONGHAO]
    //notify(index.row());

    return ok;
}

QHash<int, QByteArray> ObjectListModel::roleNames() const
{
    QHash<int, QByteArray> hash;
    for (int i = 0; i < m_roleNameList.size(); i++)
    {
        hash.insert(Qt::UserRole + i, m_roleNameList.at(i).toLocal8Bit());
    }
    
    return hash;
}

QObject* ObjectListModel::get(const QModelIndex& index) const
{
    if (!index.isValid() || index.row() >= m_objectList.size())
        return NULL;

    return m_objectList.at(index.row());
}

QObject* ObjectListModel::get(int index) const
{
    if (index < 0 || index >= m_objectList.size())
        return NULL;

    return m_objectList.at(index);
}

bool ObjectListModel::notify(int index)
{
    emit changed();

    if (index < 0 || index >= m_objectList.size())
    {
        emit dataChanged(createIndex(0, 0), createIndex(m_objectList.size() - 1, 0));
        return true;
    }

    emit dataChanged(createIndex(index, 0), createIndex(index, 0));
    return true;
}

void ObjectListModel::reset()
{
    beginResetModel();
    endResetModel();
}

QObject* ObjectListModel::find(const QString& text, const QString& role) const
{
    for (QObjectList::const_iterator iter = m_objectList.cbegin(); iter != m_objectList.cend(); ++iter)
    {
        QObject* object = *iter;
        if (object == NULL)
            continue;

        QVariant var = object->property(role.toLatin1().constData());
        if (!var.isValid())
            continue;

        QString objectText = var.toString();
        if (text == objectText)
            return object;
    }

    return NULL;
}
