/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  Plugin.h
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
#ifndef PLUGIN_H
#define PLUGIN_H

#include "corelibrary_global.h"
#include <QObject>
#include <QLibrary>
#include <QList>
#include <QVector>
#include <QPluginLoader>
#include "JsonLoader/JsonLoader.h"
#include "IScriptable.h"


/**
 *  @class Plugin
 *  @brief 插件支持类
 */
class CORELIBRARY_EXPORT Plugin : public QObject
{
public:
    enum Type
    {
        Unknown = 0,
        QtPlugin,
        JsonLoaderPlugin,
        ScriptablePlugin
    };

    /*! 
     * 获取当前插件加载得到的QObject对象
     * @return     QObject*
     */
    inline QObject* instance() const
    {
        return m_instance;
    }

    /*! 
     * 在系统库路径（包括应用程序所在路径）中扫描指定的子文件夹，加载指定类型的接口实现插件
     * @param[in]  iid              接口名
     * @param[in]  suffix           子文件夹名
     * @return     QList<Plugin*>   加载的插件列表
     */
    static QList<Plugin*> load(JsonLoader& jsonLoader, const QString& iid, const QString& suffix = QString());

protected:
    Plugin(QObject* instance, const QString& source, Type type);

private:
    QObject* m_instance;
    QString  m_source;
    Type     m_type;
};

/**
 *  @class PluginList
 *  @brief 插件列表
 */
class CORELIBRARY_EXPORT PluginList : public QObject, public QList<Plugin*>
{
    Q_OBJECT
    Q_PROPERTY(QString iid READ iid WRITE setIid)
    Q_PROPERTY(QString name READ name WRITE setName)

public:
    PluginList()
    {

    }

    PluginList(const PluginList& other)
        : QObject()
        , m_iid(other.m_iid)
        , m_name(other.m_name)
    {

    }

    PluginList& operator = (const PluginList& other)
    {
        m_iid = other.m_iid;
        m_name = other.m_name;
        return *this;
    }

    /**
     * 执行插件的载入操作
     * @param[in]
     * @return     int
     */
    int load(JsonLoader& loader);

    /** 
     * 获取当前类型插件列表成功加载的所有实例
     * @return     QObjectList 成功加载的所有实例
     */
    QObjectList instances() const;

public:
    /*!
     * Getter/Setter for iid
     */
    QString iid() const
    {
        return m_iid;
    }
    void setIid(const QString& iid)
    {
        m_iid = iid;
    }

    /*!
     * Getter/Setter for name
     */
    QString name() const
    {
        return m_name;
    }
    void setName(const QString& name)
    {
        m_name = name;
    }
private:
    QString m_iid;                          //!< 插件列表对应的iid
    QString m_name;                         //!< 插件列表的名称，与插件所在的文件夹重名
                                            //   （通常为某个名词的复数，例如devices）
};

template <class T>
inline T plugin_cast(Plugin* plugin)
{
    if (plugin == NULL) return NULL;

    return qobject_cast<T>(plugin->instance());
}

/*
 * @macro DECLARE_PLUGINS
 * @brief 在某个类（通常是Application类）的定义中定义一组插件，例如DECLARE_PLUGINS(views)
 */
#define DECLARE_PLUGINS(_type)                                                                          \
protected: PluginList m_##_type;                                                                        \
signals:                                                                                                \
           void _type##Loaded();                                                                        \
public:    PluginList _type() const { return m_##_type; }               

/*
 * @macro LOAD_PLUGINS
 * @brief 在某个类（目前仅支持Application类）的初始化函数中载入一组插件，例如LOAD_PLUGINS(views)
 */
#define LOAD_PLUGINS(_type, _iid)                                                                       \
    showSplashMessage("Loading " #_type " ...");                                                        \
    m_##_type = Plugin::load(jsonLoader(), _iid, "/" #_type);                                           \
// emit _type##Loaded();                                                                              \
// TODO: 将各个插件对象作为global objects加入当前JsonLoader的上下文                                   \
    

/*
 * @macro LOAD_PLUGINS
 * @brief 在某个类（通常是Application类）的销毁函数中卸载一组插件，例如UNLOAD_PLUGINS(views)
 */
#define UNLOAD_PLUGINS(_type)                                                                           \
    foreach(Plugin* plugin, m_##_type)                                                                  \
    {   \
        delete plugin->instance();                                                                      \
    }

#endif // PLUGIN_H
/*********************************************************************************************************
** End of file
*********************************************************************************************************/

