﻿/****************************************Copyright (c)****************************************************
**
**                             INNO Instrument(China) Co.Ltd.
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  FieldWidgetFactory.h
** Latest Version:             V1.0.0
** Latest modified Date:       2017/1/18
** Modified by:                
** Descriptions:               
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 
** Created date:               2017/1/12
** Descriptions:               FieldWidgetFactory class  [Generated by VP-UML, IGT1.2]
** 
*********************************************************************************************************/
#ifndef __FIELDWIDGETFACTORY_H__
#define __FIELDWIDGETFACTORY_H__

#include <QObject>
#include <QStringList>

#include "fieldlibrary_global.h"
// #include "QWidget.h"

class QWidget;
class IField;
class QLayout;

class FIELDLIBRARY_EXPORT FieldWidgetFactory : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList containerWidgetTypes MEMBER m_containerWidgetTypes )
    Q_PROPERTY(QString labelWidgetType MEMBER m_labelWidgetType )
    Q_PROPERTY(QString multiChoiceWidgetType MEMBER m_multiChoiceWidgetType )
    Q_PROPERTY(QString rangeWidgetType MEMBER m_rangeWidgetType )
    Q_PROPERTY(QString textWidgetType MEMBER m_textWidgetType )
    Q_PROPERTY(QString readOnlyWidgetType MEMBER m_readOnlyWidgetType )
    Q_PROPERTY(QString actionWidgetType MEMBER m_actionWidgetType )

public:
    /**
     * Constructor
     */
    FieldWidgetFactory();

    void setContainerWidgetTypes(const QStringList& typeList)
    {
        m_containerWidgetTypes = typeList;
    }

public: 
    /**
     * 创建字段对应的数值编辑控件
     * @param[in]    field  字段
     * @return       QWidget
     */
    virtual QWidget* createEditor(IField* field);

    QWidget* createEditorForChannelField(IField* field, const QString& editorType);

    /**
     * 创建字段对应的标题控件
     * @param[in] field  字段
     * @return       QWidget
     */
    virtual QWidget* createLabel(IField* field);

    /**
     * 获取字段的最佳编辑器类型名，例如带有自定义选项（choices）的字段，其最佳编辑器类型为ComboBox
     * @param[in] field  字段
     * @return       QString
     */
    virtual QString editorType(IField* field);

    /**
     * 创建字段对应的完整控件，可以直接创建出来标题和数值编辑控件并加入指定的父布局中
     * @param[in] field  字段
     * @param[in] createLabel  是否创建标题Label（如果不创建则相当于只创建编辑字段）
     * @param[in] parentLayout  父布局，如果指定了父布局则会将当前控件加入父布局
     * @return       QWidget
     */
    virtual QObject* createWidget(IField* field, Qt::Orientation direction = Qt::Horizontal, bool createLabel = true);

    /** 
     * 为容器视图（例如一个PortContainerView）创建控件，此时不需要指定字段，因为容器一般没有字段
     * @param[in]  level        容器等级，默认为根级别，即0级容器
     * @param[in]  parentLayout 父布局，如果指定了父布局则会将当前控件加入父布局
     * @return     QWidget*
     */
    virtual QObject* createContainer(int level = 0, QLayout* parentLayout = NULL);

protected:
    /** 
     * 根据metaType创建对应的widget对象
     * @param[in]  metaTypeName
     * @return     QObject*
     */
    QObject* createWidgetFromMetaType(const QString& metaTypeName);


private: 
    /**
     * 不同层次的IWidgetContainer实现类的元对象名称，第一个为第0层（即根字段），未指定的第n层级则使用默认值WidgetContainern
     */
    QStringList m_containerWidgetTypes; 
    QString     m_labelWidgetType;          //!< 默认标题Label类型名
    QString     m_multiChoiceWidgetType;    //!< 默认ComboBox类型名
    QString     m_rangeWidgetType;          //!< 默认SpinBox类型名
    QString     m_textWidgetType;           //!< 默认LineEdit类型名
    QString     m_readOnlyWidgetType;       //!< 默认只读Label类型名
    QString     m_actionWidgetType;         //!< 默认PushButton类型名
};


#endif
/*********************************************************************************************************
** End of file
*********************************************************************************************************/