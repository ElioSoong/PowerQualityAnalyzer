
#ifndef __ObjectListModel_h__
#define __ObjectListModel_h__

#include "corelibrary_global.h"
#include <QAbstractListModel>
#include <QModelIndex>
#include <QHash>
#include <QByteArray>

// class QAbstractListModel;
class ObjectListModel;
class QModelIndex;

class CORELIBRARY_EXPORT ObjectListModel: public QAbstractListModel
{
    Q_OBJECT

public:
    ObjectListModel()
    {

    }

    ObjectListModel(const ObjectListModel& other)
    {
        *this = other;
    }

    ObjectListModel& operator=(const ObjectListModel& other)
    {
        this->m_objectList = other.m_objectList;
        this->m_roleNameList = other.m_roleNameList;

        return *this;
    }

    int updateRolesFromObject(const QObject* object);

    void setObjectList(const QObjectList& objectList);
    QObjectList getObjectList() const;

    void setRoleNameList(const QStringList& roleNameList);
    QStringList getRoleNameList() const;

    virtual void reset();

signals:
    void changed();

public slots:
    QObject* get(const QModelIndex& index) const;
    QObject* get(int index) const;
    QObject* find(const QString& text, const QString& role) const;
    bool notify(int index = -1);
    int count() const;

protected:
    /*
     * @brief Reimplemented virtual methods
     */
    QHash<int, QByteArray> roleNames() const Q_DECL_OVERRIDE;
    int rowCount(const QModelIndex & parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole) Q_DECL_OVERRIDE;


protected:
    /**
     * 对象列表
     */
    QObjectList m_objectList;
    /**
     * 角色列表
     */
    QStringList m_roleNameList;
};

Q_DECLARE_METATYPE(ObjectListModel);

#endif
