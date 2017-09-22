/****************************************Copyright (c)****************************************************
**
**                             INNO Instrument(China) Co.Ltd.
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  JsonScripter.h
** Latest Version:             V1.0.0
** Latest modified Date:       2016/12/30
** Modified by:                Chen Honghao
** Descriptions:               完善递归逻辑，通过单元测试
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 Chen Honghao
** Created date:               2016/10/19
** Descriptions:               生成JSON脚本的工具类，可以将指定类型的数据转换为JSON脚本保存
** 
*********************************************************************************************************/
#pragma once

#include "Object.h"

#include <QObject>
#include <QVariant>
#include <QJsonValue>
#include <QRegExp>

/**
 *  @class JsonScripter
 *  @brief 生成JSON脚本(脚本化)的工具类
 */
class JSON_LOADER_EXPORT JsonScripter : public QObject
{
    Q_OBJECT
    Q_ENUMS(Option)
    Q_FLAGS(Options)
    Q_PROPERTY(JsonScripter::Options options READ options WRITE setOptions)
    Q_PROPERTY(int maxTreeDepth READ maxTreeDepth WRITE setMaxTreeDepth)

public:
    enum Option
    {
        DisableQtSystemObjects = 0x00000001,    //!< 不生成Qt系统对象的属性，例如QPushButton.text
        DisableRecursion = 0x00000002,          //!< 不递归生成对象树，只为当前指定对象生成脚本
        DisableDynamicProperties = 0x00000004,  //!< 不为运行时动态添加的动态属性生成脚本
        DisableSharedObjects  = 0x00000008,     //!< 不合并共享对象，共享对象的不同引用位置独立生成脚本
        DisableIndentedFormat = 0x00000010,     //!< 禁用自动缩进的JSON格式，减少空格，可明显缩小文件体积
        RefAllObjects = 0x00000020,             //!< 强制将所有遍历到的QObject子类对象视作外部对象，使用.ref存储
        ScriptChildrenObjects = 0x00000040,     //!< 是否导出children的属性

        Default = 0
    };
    Q_DECLARE_FLAGS(Options, Option)

    struct PropertyBinding 
    {
        QObject* object;
        QObject* sourceObject;
        QString  property;
        QString  sourceProperty;
    };

    struct MethodConnection
    {
        QObject* receiverObject;
        QObject* senderObject;
        QString  receiverMethod;
        QString  senderMethod;
    };

    JsonScripter();
    virtual ~JsonScripter();

    /** 
     * 清空全部内部状态和设置，通常在载入并回写新文件前使用，以避免历史数据的影响
     */
    void reset();

    QByteArray scriptToBinaryData(const QVariant& variant);
    QByteArray scriptToBinaryData(QObject* object)
    {
        return scriptToBinaryData(qVariantFromValue<QObject*>(object));
    }

    bool scriptToFile(const QVariant& variant, const QString& filePath);
    bool scriptToFile(QObject* object, const QString& filePath)
    {
        return scriptToFile(qVariantFromValue<QObject*>(object), filePath);
    }

    void addDisabledProperty(const QRegExp& namePattern);
    bool removeDisabledProperty(const QRegExp& namePattern);
    void addDisabledProperty(const QString& name)
    {
        addDisabledProperty(QRegExp(name, Qt::CaseSensitive, QRegExp::Wildcard));
    }

    void addStandardaloneProperty(const QRegExp& namePattern);
    bool removeStandardaloneProperty(const QRegExp& namePattern);
    void addStandardaloneProperty(const QString& name)
    {
        addStandardaloneProperty(QRegExp(name, Qt::CaseSensitive, QRegExp::Wildcard));
    }

    void addExternalObject(QObject* object, bool addChildren = true);
    bool removeExternalObject(QObject* object);

    /** 
     * 在指定的JSON文件中查找尚未添加的对象引用
     * @param[in]  filePath JSON文件路径
     * @return     int      新增的对象引用个数
     */
    int  addObjectRefsFromFile(const QString& filePath);
    int  addObjectRefsFromBinaryData(const QByteArray& data);
    void addObjectRef(
        QObject* object, 
        QObject* sourceObject = NULL,
        const QString& name = QString(),
        const QString& sourceName = QString()
        );
    void addObjectRef(const ObjectRef& objectRef);
    void addObjectRefs(const QList<ObjectRef>& objectRefs);
    bool removeObjectRef(QObject* object);

    void addMethodConnection(
        QObject* receiverObject, 
        QObject* senderObject,
        const QString& receiverMethod,
        const QString& senderMethod
        );
    bool removeMethodConnection(
        QObject* receiverObject,
        QObject* senderObject,
        const QString& receiverMethod,
        const QString& senderMethod
        );

    void addPropertyBinding(
        QObject* object,
        QObject* sourceObject,
        const QString& property,
        const QString& sourceProperty
        );
    bool removePropertyBinding(
        QObject* object,
        QObject* sourceObject,
        const QString& property,
        const QString& sourceProperty
        );


    /*!
     * Getter/Setter for options
     */
    Options options() const
    {
        return m_options;
    }
    void setOptions(Options options)
    {
        m_options = options;
    }

    /*!
     * Getter/Setter for maxTreeDepth
     */
    int maxTreeDepth() const
    {
        return m_maxTreeDepth;
    }
    void setMaxTreeDepth(int maxTreeDepth)
    {
        m_maxTreeDepth = maxTreeDepth;
    }

protected:
    bool scriptToFile(
        const QVariant& variant, 
        const QString& filePath, 
        const QObjectList& externalObjects,
        QObjectList& scriptedObjects
        );

    QByteArray scriptToBinaryData(
        const QVariant& variant, 
        const QObjectList& externalObjects, 
        QObjectList& scriptedObjects
        );

    virtual QJsonValue script( 
        const QVariant& variant, 
        const QObjectList& externalObjects, 
        QObjectList& scriptedObjects 
        );

    int findAllObjects(
        const QVariant& variant,
        QObjectList& allObjects,
        QObjectList& sharedObjects
        );

    int findSharedObjects(
        const QVariant& variant,
        QObjectList& sharedObjects
        );

    QVariant variantToList(const QVariant &propValue);
    QVariant enumToString(const QVariant &propValue, const QMetaProperty &prop);

    int propertyStartOffset(const QMetaObject* metaObject);

    bool isDisabledProperty(const QString& className, const QString &propName);
    bool isStandaloneProperty(const QString& objectName, const QString& propName);

protected:
    Options m_options;
    int     m_maxTreeDepth;
    int     m_currentTreeDepth;

    QList<QRegExp> m_diabledProperties;
    QList<QRegExp> m_standaloneProperties;
    QList<ObjectRef> m_objectRefs;
    QObjectList m_externalObjects;
    QList<PropertyBinding> m_propertyBindings;    // TODO
    QList<MethodConnection> m_methodConnections;        // TODO
};

Q_DECLARE_OPERATORS_FOR_FLAGS(JsonScripter::Options)

/**
 *  @class IJsonScriptable
 *  @brief 可以被自定义地脚本化的类
 */
class IJsonScriptable
{
public:
    IJsonScriptable()
    {

    }

    virtual QJsonValue toJsonValue() const = 0;
    virtual bool fromJsonValue(const QJsonValue& value) = 0;
};

Q_DECLARE_INTERFACE(IJsonScriptable, "com.inno.IJsonScriptable")

/*********************************************************************************************************
** End of file
*********************************************************************************************************/
