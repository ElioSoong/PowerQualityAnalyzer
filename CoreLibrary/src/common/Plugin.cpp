/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  Plugin.cpp
** Latest Version:             V1.0.0
** Latest modified Date:       2016/05/26
** Modified by:                
** Descriptions:               
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 Chen Honghao
** Created date:               2016/05/26
** Descriptions:               插件支持类
** 
*********************************************************************************************************/
#include "includes.h"
#include "Plugin.h"

#include <QCoreApplication>
#include <QDir>
#include <QJsonObject>
#include <QtDebug>

Plugin::Plugin(QObject* instance, const QString& source, Type type)
    : QObject()
    , m_instance(instance)
    , m_source(source)
    , m_type(type)
{

}

QList<Plugin*> Plugin::load(JsonLoader& jsonLoader, const QString& iid, const QString& suffix /*= QString()*/)
{
    QList<Plugin*> plugins;

    QStringList paths = QCoreApplication::libraryPaths();
#if 1
    // 程序启动的当前路径，通常意味着解决方案的根目录或者发行包的根目录，这里极有可能有插件 [5/3/2017 CHENHONGHAO]
    QString currentPath = QDir::currentPath();
    if (!paths.contains(currentPath))
        paths.push_front(currentPath);
#endif
    for (int i = 0; i < paths.count(); ++i) {
        QString pluginDir = paths.at(i);
        if (!pluginDir.endsWith('/'))
            pluginDir.push_back('/');

        QString dirPath = pluginDir + suffix;
        QFileInfoList dirEntries = QDir(dirPath).entryInfoList(QDir::Files);

        foreach (QFileInfo fileInfo, dirEntries)
        {
            QString filePath = fileInfo.absoluteFilePath();

            Plugin* plugin = NULL;
            IScriptable* scriptable = NULL;

            if (QLibrary::isLibrary(filePath))
            {
                QPluginLoader loader(filePath);

                QObject* object = loader.instance();
                QString loadedIID = loader.metaData().value(QLatin1String("IID")).toString();
                if (object && (iid.isEmpty() || iid == loadedIID))
                {
                    qDebug() << "Qt Plugin loaded: " << qUtf8Printable(filePath);
                    plugin = new Plugin(object, filePath, Plugin::QtPlugin);
                    // 将Qt plugin创建的对象加入JsonLoader中供引用
                    if (!object->objectName().isEmpty())
                    {
                        jsonLoader.addGlobalObject(object);
                        // addGlobalObject会隐式地设置parent为JsonLoader，这不是预期功能
                        object->setParent(NULL);
                    }
                }

                scriptable = qobject_cast<IScriptable*>(object);
                if (scriptable)
                    scriptable->registerAllTypes(&jsonLoader);
            }
            else if (filePath.endsWith(".json", Qt::CaseInsensitive))
            {
                //JsonLoader jsonLoader;  // 目前每一个json插件有一个独立的JsonLoader的上下文

                QVariant variant = jsonLoader.load(filePath);
                QObject* object = variant.value<QObject*>();
                // TODO: 此处目前无法判断iid
                if (object)
                {
                    qDebug() << "JsonLoader Plugin loaded: " << qUtf8Printable(filePath);
                    plugin = new Plugin(object, filePath, Plugin::QtPlugin);

#if 0
                    // 通常从JSON文件载入的对象类型，是来自于DLL的，而DLL已经被注册了一次
                    scriptable = qobject_cast<IScriptable*>(object);
                    if (scriptable)
                        scriptable->registerAllTypes(&jsonLoader);
#endif
                }
            }

            if (plugin)
                plugins.push_back(plugin);
            if (scriptable)
            {
                QStringList typeNames = scriptable->typeNames();
                foreach (QString typeName, typeNames)
                {
                    QObject* scriptableObject = IScriptable::newInstance(typeName);
                    if (scriptableObject)
                    {
                        qDebug() << "Scriptable Plugin loaded: " << qUtf8Printable(filePath);
                        Plugin* plugin = new Plugin(scriptableObject, typeName, Plugin::ScriptablePlugin);
                        plugins.push_back(plugin);

                        // 将Qt plugin创建的对象加入JsonLoader中供引用
                        if (!scriptableObject->objectName().isEmpty())
                        {
                            jsonLoader.addGlobalObject(scriptableObject);

                            // addGlobalObject会隐式地设置parent为JsonLoader，这不是预期功能
                            scriptableObject->setParent(NULL);
                        }
                    }
                }
            }
        }
    }

    return plugins;
}


/*********************************************************************************************************
** class PluginList
*********************************************************************************************************/
int PluginList::load(JsonLoader& loader)
{
    *static_cast<QList<Plugin*>*>(this) = Plugin::load(loader, m_iid, m_name);
    return this->length();
}

QObjectList PluginList::instances() const
{
    QObjectList result;

    foreach(Plugin* plugin, *static_cast<const QList<Plugin*>*>(this))
    {
        QObject* instance = NULL;
        if (plugin && (instance = plugin->instance()))
        {
            result << instance;
        }
    }

    return result;
}

/*********************************************************************************************************
** End of file
*********************************************************************************************************/
