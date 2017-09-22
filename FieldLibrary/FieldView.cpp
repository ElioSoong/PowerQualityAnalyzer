﻿/****************************************Copyright (c)****************************************************
**
**                             INNO Instrument(China) Co.Ltd.
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  FieldView.cpp
** Latest Version:             V1.0.0
** Latest modified Date:       2017/1/18
** Modified by:
** Descriptions:
**
**--------------------------------------------------------------------------------------------------------
** Created by:
** Created date:               2017/1/12
** Descriptions:               FieldView class  [Generated by VP-UML, IGT1.2]
**
*********************************************************************************************************/
#include "includes.h"
#include "FieldView.h"
#include "View.h"
#include "FieldWidgetFactory.h"
#include "IFieldWidget.h"

#include <QMetaObject>
#include <QMetaProperty>

static FieldWidgetFactory s_defaultWidgetFactory;
FieldWidgetFactory* FieldView::s_defaultWidgetFactory = &::s_defaultWidgetFactory;

/**
 * Constructor
 */
FieldView::FieldView(IField* field) : View(), m_widgetType(), m_field(field)
{
    // TODO: Not yet implemented
}

FieldWidgetFactory* FieldView::fieldWidgetFactory() const
{
    return s_defaultWidgetFactory;
}

void FieldView::setDefaultFieldWidgetFactory(FieldWidgetFactory* factory)
{
    s_defaultWidgetFactory = factory;
}

QObject* FieldView::createWidget()
{
    IField* field = this->field();
    // TODO:注意需要在PortView::createWidget中解决内存泄漏问题和widget树形析构导致重复析构问题，
    // 实际应该放到此函数中这是不要的
    FieldWidgetFactory* factory = this->fieldWidgetFactory();
    if (factory && field)
    {
        QObject* widgetObject = factory->createWidget(field, Qt::Orientation(this->property("direction").toInt()), this->property("hasBuddy").toBool());
        QWidget* widget = qobject_cast<QWidget*>(widgetObject);
        if (!widget)
        {
            return widgetObject;
        }

        setWidget(widget);
        setField(m_field);
        commitToWidget();

        const QMetaObject* thisMetaObject = this->metaObject();
        int slotMethodIndex = thisMetaObject->indexOfSlot("onWidgetChanged()");
        QMetaMethod thisSlot = thisMetaObject->method(slotMethodIndex);
        Q_ASSERT(thisSlot.isValid());
        // Disconnect a specific receiver
        this->disconnect(SLOT(onWidgetChanged()));
        // Connect all new receivers
        QList<QObject*> widgets = this->fieldWidgets();
        foreach(QObject* widget, widgets)
        {
            IFieldWidget* fieldWidget = qobject_cast<IFieldWidget*>(widget);
            const QMetaObject* metaObject = widget->metaObject();
            if (metaObject)
            {
                const char* valuePropertyName = fieldWidget ? fieldWidget->valueProperty() : NULL;
                if (valuePropertyName == NULL)
                {
                    valuePropertyName = "value";
                }

                int valuePropertyIndex = metaObject->indexOfProperty(valuePropertyName);
                if (valuePropertyIndex)
                {
                    QMetaProperty valueProperty = metaObject->property(valuePropertyIndex);
                    QMetaMethod valueNotifySignal = valueProperty.notifySignal();
                    // 如果您的代码在这里发生错误，说明您的IFieldWidget子类没有
                    // 正确地定义value属性的NOTIFY signal [3/13/2017 CHENHONGHAO]
                    Q_ASSERT(valueNotifySignal.isValid());
                    bool connectOk = connect(widget, valueNotifySignal, this, thisSlot);
                    Q_ASSERT(connectOk);
                }
            }
        }

        return widget;
    }

    return NULL;
}

QList<QObject*> FieldView::fieldWidgets() const
{
    QList<QObject*> result;

    QWidget* widget = this->widget();
    IFieldWidget* fieldWidget = qobject_cast<IFieldWidget*>(widget);
    if (fieldWidget)
    {
        result << widget;
    }
    else if (widget)
    {
        // 说明根widget只是一个包含了label（必须非IFieldWidget）和editor的wrapper而已
        QList<QWidget*> children = widget->findChildren<QWidget*>(QString(), Qt::FindDirectChildrenOnly);

        //// TODO: 不能做label为第一个child的假设
        //Q_ASSERT(children.size() == 2);
        //QWidget* editorWidget = children.size() == 2 ? qobject_cast<QWidget*>(children[1]) : NULL;
        QWidget* editorWidget = NULL;
        if (this->property("hasBuddy").toBool())
        {
            editorWidget = qobject_cast<QWidget*>(children[1]);
        }
        else
        {
            editorWidget = widget;
        }
        IFieldWidget* editorFieldWidget = qobject_cast<IFieldWidget*>(editorWidget);
        if (editorFieldWidget)
        {
            result << editorWidget;
        }
        else if (editorWidget)
        {
            //说明editorWidget又只是一个包含多个通道的wrapper而已
            //TODO: 将此部分的处理移动至IIFieldWidget内部
            QList<QWidget*> children = editorWidget->findChildren<QWidget*>(QString(), Qt::FindDirectChildrenOnly);
            foreach(QWidget* child, children)
            {
                // TODO: 成功&失败的返回值判断
                IFieldWidget* channelEditorFieldWidget = qobject_cast<IFieldWidget*>(child);
                if (channelEditorFieldWidget)
                {
                    result << child;
                }
            }
        }
    }

    return result;
}

QObject* FieldView::fieldWidget(int channel) const
{
    QList<QObject*> fieldWidgets = this->fieldWidgets();
    if (channel >= 0 && channel < fieldWidgets.count())
        return fieldWidgets[channel];

    return NULL;
}

void FieldView::fieldFromWidget(IField* field, QObject* widget)
{
    IFieldWidget* fieldWidget = qobject_cast<IFieldWidget*>(widget);
    if (field && fieldWidget)
    {
        fieldWidget->getField(field);
    }
}

void FieldView::fieldToWidget(IField* field, QObject* widget)
{
    IFieldWidget* fieldWidget = qobject_cast<IFieldWidget*>(widget);
    if (field && fieldWidget)
    {
        fieldWidget->setField(field);
    }
}

bool FieldView::updateFromWidget(int channel /*= -1*/)
{
    IField* field = this->field();
    QList<QObject*> fieldWidgets = this->fieldWidgets();
    int beginChannelIndex = channel;
    int endChannelIndex = channel;
    if (channel >= fieldWidgets.count())
    {
        return false;
    }
    else if (channel < 0)
    {
        beginChannelIndex = 0;
        endChannelIndex = this->fieldWidgets().count() - 1;
    }

    for (int i = beginChannelIndex; i <= endChannelIndex; ++i)
    {
        IField* childField = field->child(i);
        QObject* widget = fieldWidgets[i];
        fieldFromWidget(childField, widget);
    }

    return true;
}

bool FieldView::commitToWidget(int channel)
{
    IField* field = this->field();
    QList<QObject*> fieldWidgets = this->fieldWidgets();
    int beginChannelIndex = channel;
    int endChannelIndex = channel;
    if (channel >= fieldWidgets.count())
    {
        return false;
    }
    else if (channel < 0)
    {
        beginChannelIndex = 0;
        endChannelIndex = this->fieldWidgets().count() - 1;
    }

    for (int i = beginChannelIndex; i <= endChannelIndex; ++i)
    {
        IField* childField = field->child(i);
        QObject* widget = fieldWidgets[i];
        fieldToWidget(childField, widget);
    }

    return true;
}

void FieldView::onWidgetChanged()
{
    // TODO: 使用QSignalMapper来改善代码
    QObject* senderWidget = QObject::sender();
    QList<QObject*> fieldWidgets = this->fieldWidgets();
    int senderWidgetIndex = fieldWidgets.indexOf((QWidget*)senderWidget);
    updateFromWidget(senderWidgetIndex);
}

/*********************************************************************************************************
** End of file
*********************************************************************************************************/