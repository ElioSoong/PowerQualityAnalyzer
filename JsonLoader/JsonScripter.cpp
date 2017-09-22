/****************************************Copyright (c)****************************************************
**
**                             INNO Instrument(China) Co.Ltd.
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  JsonScripter.cpp
** Latest Version:             V1.0.0
** Latest modified Date:       2016/10/19
** Modified by:
** Descriptions:
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 Chen Honghao
** Created date:               2016/10/19
** Descriptions:               生成JSON脚本的工具类，可以将指定类型的数据转换为JSON脚本保存
**
*********************************************************************************************************/
#include "includes.h"
#include "JsonScripter.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QMetaProperty>
#include <QDir>

JsonScripter::JsonScripter()
    : QObject()
    , m_options(Default)
    , m_maxTreeDepth(-1)
    , m_currentTreeDepth(0)
{
}


JsonScripter::~JsonScripter()
{
}

void JsonScripter::reset()
{
    Options m_options = Default;
    int     m_maxTreeDepth = -1;
    int     m_currentTreeDepth = 0;

    m_diabledProperties.clear();
    m_standaloneProperties.clear();
    m_externalObjects.clear();
    m_objectRefs.clear();
    m_propertyBindings.clear();
    m_methodConnections.clear();
}

QByteArray JsonScripter::scriptToBinaryData(
    const QVariant& variant, 
    const QObjectList& externalObjects, 
    QObjectList& scriptedObjects
    )
{
    QJsonDocument document;
    QJsonValue value = script(variant, externalObjects, scriptedObjects);
    if (value.isObject())
        document.setObject(value.toObject());
    else if (value.isArray())
        document.setArray(value.toArray());
    else
    {
        QJsonArray array;
        array.append(value);
        document.setArray(array);
    }

    QJsonDocument::JsonFormat format = (m_options & DisableIndentedFormat) ? 
        QJsonDocument::Compact : QJsonDocument::Indented;
    QByteArray data = document.toJson(format);
    return data;
}

QByteArray JsonScripter::scriptToBinaryData(const QVariant& variant)
{
    QObjectList scriptedObjects;
    m_currentTreeDepth = 0;
    return scriptToBinaryData(variant, m_externalObjects, scriptedObjects);
}

bool JsonScripter::scriptToFile(
    const QVariant& variant, 
    const QString& filePath, 
    const QObjectList& externalObjects,
    QObjectList& scriptedObjects
    )
{
    QByteArray data = scriptToBinaryData(variant, externalObjects, scriptedObjects);
    if (!data.isEmpty())
    {
        QFile file(filePath);
        if (file.open(QFile::WriteOnly))
        {
            file.write(data);
            file.close();
            return true;
        }
    }

    return false;
}


bool JsonScripter::scriptToFile(const QVariant& variant, const QString& filePath)
{
    QObjectList scriptedObjects;
    scriptedObjects = m_externalObjects;
    foreach (const ObjectRef& ref, m_objectRefs)
    {
        scriptedObjects.removeAll(ref.object);
    }
    m_currentTreeDepth = 0;
    return scriptToFile(variant, filePath, m_externalObjects, scriptedObjects);
}

void JsonScripter::addDisabledProperty(const QRegExp& namePattern)
{
    if (!m_diabledProperties.contains(namePattern))
        m_diabledProperties.push_back(namePattern);
}


bool JsonScripter::removeDisabledProperty(const QRegExp& namePattern)
{
    return m_diabledProperties.removeAll(namePattern) > 0;
}


void JsonScripter::addStandardaloneProperty(const QRegExp& namePattern)
{
    if (!m_standaloneProperties.contains(namePattern))
        m_standaloneProperties.push_back(namePattern);
}


bool JsonScripter::removeStandardaloneProperty(const QRegExp& namePattern)
{
    return m_standaloneProperties.removeAll(namePattern) > 0;
}

void JsonScripter::addExternalObject(QObject* object, bool addChildren /*= true*/)
{
    if (!object)
        return;

    QObjectList allObjects;
    //allObjects.prepend(object);
    if (addChildren)
    {
        QObjectList sharedObjects;
        findAllObjects(QVariant::fromValue(object), allObjects, sharedObjects);
        Q_UNUSED(sharedObjects);
    }

    foreach (QObject* iter, allObjects)
    {
        if (iter && !m_externalObjects.contains(iter))
        {
            m_externalObjects.push_back(iter);
        }
    }
}


bool JsonScripter::removeExternalObject(QObject* object)
{
    return m_externalObjects.removeAll(object) > 0;
}


int JsonScripter::addObjectRefsFromFile(const QString& filePath)
{
    QFile file(filePath);
    if (file.open(QFile::ReadOnly))
    {
        QByteArray data = file.readAll();
        int count = addObjectRefsFromBinaryData(data);
        file.close();
        return count;
    }

    return 0;
}

int JsonScripter::addObjectRefsFromBinaryData(const QByteArray& data)
{
    return 0;
}

void JsonScripter::addObjectRef(QObject* object, QObject* sourceObject /*= QString()*/, const QString& name /*= QString()*/, const QString& sourceName /*= QString() */)
{
    ObjectRef item(object, sourceObject, name, sourceName);
    addObjectRef(item);
}

void JsonScripter::addObjectRef(const ObjectRef& objectRef)
{
    int index = m_objectRefs.indexOf(objectRef);
    if (index < 0)
        m_objectRefs.push_back(objectRef);
    else
        m_objectRefs[index] = objectRef;
}

void JsonScripter::addObjectRefs(const QList<ObjectRef>& objectRefs)
{
    m_objectRefs.append(objectRefs);
}

bool JsonScripter::removeObjectRef(QObject* object)
{
    ObjectRef item(object);
    return m_objectRefs.removeAll(item) > 0;
}


void JsonScripter::addMethodConnection(QObject* receiverObject, QObject* senderObject, const QString& receiverMethod, const QString& senderMethod)
{

}


bool JsonScripter::removeMethodConnection(QObject* receiverObject, QObject* senderObject, const QString& receiverMethod, const QString& senderMethod)
{
    return false;
}


void JsonScripter::addPropertyBinding(QObject* object, QObject* sourceObject, const QString& property, const QString& sourceProperty)
{
}


bool JsonScripter::removePropertyBinding(QObject* object, QObject* sourceObject, const QString& property, const QString& sourceProperty)
{
    return false;
}


int JsonScripter::findAllObjects(const QVariant& variant, QObjectList& allObjects, QObjectList& sharedObjects)
{
    int count = 0;

    if (variant.type() == QMetaType::QVariantList)
    {
        QVariantList list = variant.toList();
        foreach(const QVariant& element, list)
            count += findAllObjects(element, allObjects, sharedObjects);
        return count;
    }
    else if (!variant.canConvert(QMetaType::QObjectStar))
        return 0;


    QObject* object = variant.value<QObject*>();
    if (object == NULL)
        return 0;

    if (allObjects.contains(object))
    {
        if (!sharedObjects.contains(object))
            sharedObjects.push_back(object);
        return count;
    }
    else
    {
        allObjects.push_back(object);
        count++;
    }
    
#if defined(_DEBUG) && 1
    //qDebug() << "FindAllObjects += " << object;
#endif
    QList<QByteArray> dynamicProperties = object->dynamicPropertyNames();
    const QMetaObject* metaObject = object->metaObject();
    if (metaObject)
    {
        QString className = metaObject->className();
        int propertyCount = metaObject->propertyCount();
        for (int i = 1; i < propertyCount; ++i)
        {
            QMetaProperty prop = metaObject->property(i);
            // 排除被人为设置为STORED false的属性，这些属性不需要被脚本化地存储 [10/21/2016 CHENHONGHAO]
            if (!prop.isStored(object))
                continue;

            QByteArray propNameBuffer = prop.name();
            QString propName = QString::fromLatin1(propNameBuffer);
            // 如果显示地指定了禁用此属性，则不保存
            if (isDisabledProperty(className, propName))
                continue;
            // 如果禁用了动态属性，则不保存动态属性
            if ((m_options & DisableDynamicProperties) && !dynamicProperties.isEmpty())
            {
                if (dynamicProperties.contains(propNameBuffer))
                    continue;
            }
//             // 如果源脚本中未显式重载，则不保存此属性，以避免重复
//             if (objectRef.sourceObject && !objectRef.overridedProperties.contains(propName))
//                 continue;

            QVariant propValue = prop.read(object);
            if (propValue.canConvert(QMetaType::QVariantList))
                propValue = variantToList(propValue);
#if defined(_DEBUG) && 1
            //qDebug() << "FindAllObjects ? " << object << "." << propName;
#endif
            count += findAllObjects(propValue, allObjects, sharedObjects);
        }
    }

    return count;
}


int JsonScripter::findSharedObjects(const QVariant& variant, QObjectList& sharedObjects)
{
    QObjectList allObjects = sharedObjects;
    // 注意避免递归依赖造成的stack overflow [10/20/2016 CHENHONGHAO]
    // 注意移除树形共享 [10/20/2016 CHENHONGHAO]
    findAllObjects(variant, allObjects, sharedObjects);
    return sharedObjects.count();
}

class AutoInt
{
public:
    explicit AutoInt(int& i) : m_int(i)
    {
        ++m_int;
    }

    ~AutoInt()
    {
        --m_int;
    }

    operator int&() const
    {
        return m_int;
    }

protected:
    int& m_int;
};

#define BEGIN_RECURSE() int __scriptedObjectsCount = scriptedObjects.count();
#define END_RECURSE()   scriptedObjects = scriptedObjects.mid(0, __scriptedObjectsCount);

QJsonValue JsonScripter::script(
    const QVariant& variant, 
    const QObjectList& externalObjects, 
    QObjectList& scriptedObjects
    )
{
    AutoInt treeDepth(m_currentTreeDepth);
    int maxTreeDepth = (m_options & DisableRecursion) ? 1 : m_maxTreeDepth;
    if (maxTreeDepth > 0 && treeDepth > maxTreeDepth)
        return QJsonValue();

    if (variant.canConvert(QMetaType::QJsonValue))
        return variant.toJsonValue();
    else if (variant.type() == QMetaType::QVariantList)
    {
        QJsonArray array;
        QVariantList list = variant.toList();
        foreach (const QVariant& var, list)
        {
#if defined(_DEBUG) && 1
            //qDebug() << "Script " << var << " at level " << treeDepth;
#endif
            QJsonValue element = script(var, externalObjects, scriptedObjects);
            array.append(element);
        }
        return array;
    }
    else if (!variant.canConvert(QMetaType::QObjectStar))
        return QJsonValue::fromVariant(variant);


    QObject* object = variant.value<QObject*>();
    const QMetaObject* metaObject = object ? object->metaObject() : NULL;
    if (!object || !metaObject)
        return QJsonValue();

    QJsonObject jsonObject;
    QString objectName = object->objectName();
    QString className = metaObject->className();
    QList<QByteArray> dynamicProperties = object->dynamicPropertyNames();
#if defined(_DEBUG) && 1
    if (objectName == "externalField" || objectName == "externalLinkField")
    {
        int breakpoint = 0;
    }
#endif
    if (scriptedObjects.contains(object))
    {
        if (objectName.isEmpty())
        {
            qCritical() << "Cannot ref/copy from unnamed object:" << object;
            objectName = QString("<0x%1>").arg((quint32)object, 8, 16, QLatin1Char('0'));
            object->setObjectName(objectName);
        }
        jsonObject.insert(".ref", objectName);
        return jsonObject;
    }


    QObjectList sharedInternalObjects;
    if (!(m_options & DisableSharedObjects) && 
        treeDepth <= 1 && 
        findSharedObjects(variant, sharedInternalObjects) > 0)
    {
        // .objects [10/20/2016 CHENHONGHAO]
        QJsonArray array;
        // 顶层对象是会被展开的，但不想在处理.objects时就展开 [10/26/2016 CHENHONGHAO]
        scriptedObjects.push_back(object);
        foreach (QObject* sharedObject, sharedInternalObjects)
        {
            // 外部引用的对象不写入.objects【已禁用，改为外部对象不展开，仍然有可能被写入.objects】
            //if (!externalObjects.contains(sharedObject)) 
            // 直接排除root自己共享自己的特殊场景
            if (sharedObject != object) 
            {
                BEGIN_RECURSE();
                QObjectList sharedObjectExcludingThisOne = sharedInternalObjects;
                sharedObjectExcludingThisOne.removeAll(sharedObject);
                scriptedObjects.append(sharedObjectExcludingThisOne);
#if defined(_DEBUG) && 1
                //qDebug() << "Script " << sharedObject << " at level " << treeDepth;
#endif
                QJsonValue value = script(QVariant::fromValue(sharedObject), externalObjects, scriptedObjects);
                END_RECURSE();
                array.append(value);
            }
        }
        scriptedObjects.removeAll(object);
        if (!array.isEmpty())
            jsonObject.insert(".objects", array);

        scriptedObjects.append(sharedInternalObjects);
    }
    scriptedObjects.push_back(object);


    IJsonScriptable* scriptable = qobject_cast<IJsonScriptable*>(object);
    if (scriptable)
        return scriptable->toJsonValue();

    QObject* sourceObject = NULL;
    QString sourceName;
    ObjectRef objectRef(object, NULL, objectName, QString());
    int objectRefIndex = m_objectRefs.indexOf(objectRef);
    if (m_options & RefAllObjects)
    { 
        jsonObject.insert(".ref", objectName);
    }
    else if (objectRefIndex >= 0)
    {
        objectRef = m_objectRefs[objectRefIndex];
        sourceObject = objectRef.sourceObject;
        sourceName = objectRef.sourceName;

        if (sourceName.isEmpty())
            sourceName = sourceObject->objectName();
        if (sourceName.isEmpty())
        {
            qCritical() << "Cannot ref/copy from unnamed object:" << sourceObject;
            sourceName = QString("<0x%1>").arg((quint32)sourceObject, 8, 16, QLatin1Char('0'));
        }

        QString refString = QString("%1").arg(sourceName);
        jsonObject.insert(".ref", refString);

        if (objectName != sourceName && !objectName.isEmpty())
            jsonObject.insert(".id", objectName);

        if (object != sourceObject)
            jsonObject.insert(".copy", true);
    }
    else if (!externalObjects.contains(object))
    {
        if (!objectName.isEmpty())
            jsonObject.insert(".id", objectName);

        // TODO: 这里不能直接使用className，
        // 因为有可能在meta-system中被注册的名字并不是类名 [10/21/2016 CHENHONGHAO]
        QString metaTypeName = className;
        jsonObject.insert(".type", metaTypeName);
    }
    else
    {
        // 如果是外部对象，并且没有重载属性值（如果有重名或重载属性则不会进此分支），则不需要展开属性列表，
        // 但需要引用则引用(.ref)，需要重命名则重命名(.id) [10/26/2016 CHENHONGHAO]
        jsonObject.insert(".ref", objectName);
        return jsonObject;
    }

    // 导出children的属性
    if ((m_options & ScriptChildrenObjects) && (object->children().size() > 0))
    {
        QVariantList list;
        for (int i = 0; i < object->children().length(); ++i)
        {
            list << qVariantFromValue<QObject*>(object->children().at(i));
        }
        QJsonValue jsonValue1 = script(list, externalObjects, scriptedObjects);
        jsonObject.insert(".qchildren", jsonValue1);
    }

    int propertyOffset = propertyStartOffset(metaObject);
    int propertyCount = metaObject->propertyCount();
    for (int i = propertyOffset; i < propertyCount; ++i)
    {
        QMetaProperty prop = metaObject->property(i);
        // 排除被人为设置为STORED false的属性，这些属性不需要被脚本化地存储 [10/21/2016 CHENHONGHAO]
        if (!prop.isStored(object))
            continue;

        QByteArray propNameBuffer = prop.name();
        QString propName = QString::fromLatin1(propNameBuffer);
        // 如果显示地指定了禁用此属性，则不保存
        if (isDisabledProperty(className, propName))
            continue;
        // 如果禁用了动态属性，则不保存动态属性
        if ((m_options & DisableDynamicProperties) && dynamicProperties.contains(propNameBuffer))
            continue;
        // 如果源脚本中未显式重载，则不保存此属性，以避免重复
        if (objectRef.sourceObject && !objectRef.overridedProperties.contains(propName))
            continue;

        QVariant propValue = prop.read(object);
        if (propValue.canConvert(QMetaType::QVariantList))
            propValue = variantToList(propValue);
        else if (prop.isEnumType() || prop.isFlagType())
            propValue = enumToString(propValue, prop);
        else if (propValue.type() == QMetaType::QRegExp)
        {
            // 正则表达式目前只保存pattern，因此暂不支持固定字符串、通配符等格式，
            // 若要支持需要与JsonLoader配套
            propValue = propValue.toRegExp().pattern();
        }
        // Add your specific handlers here ...

        BEGIN_RECURSE();
#if defined(_DEBUG) && 1
        //qDebug() << "Script " << propValue << " at level " << treeDepth;
#endif
        QJsonValue jsonValue = script(propValue, externalObjects, scriptedObjects);
        END_RECURSE();
        if (isStandaloneProperty(objectName, propName))
        {
            QString childJsonFileName = objectName + '.' + propName + ".json";
            if (scriptToFile(QVariant(jsonValue), childJsonFileName, externalObjects, scriptedObjects))
                jsonValue = QJsonValue(childJsonFileName);
            else
                qCritical() << "Failed to script property-JSON file to " << childJsonFileName;
        }

        jsonObject.insert(propName, jsonValue);
    }

    return jsonObject;
}


QVariant JsonScripter::variantToList(const QVariant &value)
{
    QSequentialIterable iterable = value.value<QSequentialIterable>();
    QVariantList list;
    foreach(const QVariant &v, iterable)
        list.push_back(v);
    return list;
}

QVariant JsonScripter::enumToString(const QVariant &propValue, const QMetaProperty &prop)
{
    int intValue = propValue.toInt();
    QMetaEnum enumerator = prop.enumerator();
    QByteArray keysValue;
    if (prop.isFlagType())
        keysValue = enumerator.valueToKeys(intValue);
    else
        keysValue = enumerator.valueToKey(intValue);
    QString keysString = QString::fromLatin1(keysValue);
#if 1
    QStringList keys = keysString.split('|');
    const QMetaObject* enumMetaObject = enumerator.enclosingMetaObject();
    Q_ASSERT(enumMetaObject);
    QString enumClassName = enumMetaObject->className();
    QString enumName = enumClassName + '.';
    for (QStringList::iterator iter = keys.begin(); iter != keys.end(); ++iter)
        iter->prepend(enumName);
    keysString = keys.join('|');
#endif
    return keysString;
}

bool JsonScripter::isDisabledProperty(const QString& className, const QString &propName)
{
    if (!m_diabledProperties.isEmpty())
    {
        QString propFullName = className + '.' + propName;
        foreach (const QRegExp& regExp, m_diabledProperties)
        {
            if (regExp.exactMatch(propFullName))
            {
                return true;
            }
        }
    }

    return false;
}

bool JsonScripter::isStandaloneProperty(const QString& objectName, const QString& propName)
{
    if (!m_standaloneProperties.isEmpty())
    {
        QString propFullName = objectName + '.' + propName;
        foreach (const QRegExp& regExp, m_standaloneProperties)
        {
            if (regExp.exactMatch(propFullName))
            {
                return true;
            }
        }
    }

    return false;
}

int JsonScripter::propertyStartOffset(const QMetaObject* metaObject)
{
    int propertyOffset = 1;
#if 1
    if (m_options & DisableQtSystemObjects)
    {
        const QMetaObject* currentMetaObject = metaObject;
        do
        {
            QString className = currentMetaObject->className();
            if (className.startsWith('Q'))
            {
                propertyOffset = currentMetaObject->propertyCount();
                break;
            }

        } while ((currentMetaObject = currentMetaObject->superClass()) != NULL);
    }
#endif	
    return propertyOffset;
}

/*********************************************************************************************************
** End of file
*********************************************************************************************************/
