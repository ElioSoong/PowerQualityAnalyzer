﻿/****************************************Copyright (c)****************************************************
**
**                             INNO Instrument(China) Co.Ltd.
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  FieldView.h
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
#ifndef __FIELDVIEW_H__
#define __FIELDVIEW_H__


// #include "QWidget.h"
#include "IField.h"
#include "View.h"

class QWidget;
class FieldWidgetFactory;
// class View;

/**
 * @class FieldView
 * @brief 字段视图
 */
class FIELDLIBRARY_EXPORT FieldView : public View
{
    Q_OBJECT
    Q_PROPERTY(QString widgetType READ widgetType WRITE setWidgetType)
    Q_PROPERTY(IField* field READ field WRITE setField)

public:
    /**
     * Constructor
     */
    FieldView(IField* field = NULL);

    /*!
     * Getter/Setter for widgetType
     */
    QString widgetType() const
    {
        return m_widgetType;
    }
    void setWidgetType(const QString& widgetType)
    {
        m_widgetType = widgetType;
    }
    /*!
     * Getter/Setter for field
     */
    IField* field() const
    {
        return m_field;
    }
    void setField(IField* field)
    {
        m_field = field;
    }

public:
    /**
     * 当前字段视图使用的字段Widget工厂，通常某类字段视图使用统一的字段Widget工厂
     * @return       FieldWidgetFactory
     */
    virtual FieldWidgetFactory* fieldWidgetFactory() const;

    /** 
     * 设置所有FieldView默认的字段Widget工厂，未指定工厂的FieldView子类默认使用这个工厂
     * @param[in]  factory 默认的字段Widget工厂
     * @return     void
     */
    static void setDefaultFieldWidgetFactory(FieldWidgetFactory* factory);

    /**
     * 使用当前字段视图对应的工厂来创建一个字段Widget（前提是字段已经存在）
     * @return       QWidget
     */
    virtual QObject* createWidget();

    /** 
     * 获取指定通道对应的fieldWidgets
     * @param[in]  channel  指定通道
     * @return     QWidget*
     */
    QObject* fieldWidget(int channel) const;

    /** 
     * 获取全部通道对应的fieldWidgets
     * @return     QList<QWidget*> 全部通道对应的fieldWidgets
     */
    virtual QList<QObject*> fieldWidgets() const;

    /** 
     * 获取指定通道对应的FieldWidget
     * @param[in]  channel  指定通道
     * @return     QWidget* 指定通道对应的FieldWidget
     */

    /**
     * 从Widget中获取最新数据并保存到Field
     * @param[in]  channel 需要更新的通道，默认为-1，代表更新所有通道
     * @return       Return true on succeed, otherwise return false.
     */
    virtual bool updateFromWidget(int channel = -1);

    /**
     * 将Field中的数据提交到Widget
     * @param[in]  channel 需要提交的通道，默认为-1，代表提交所有通道
     * @return       bool
     */
    virtual bool commitToWidget(int channel = -1);

    /**
     * 当widget的数据发生变化时，需要更新对应的字段，注意避免递归调用
     */
    Q_SLOT virtual void onWidgetChanged();


protected:
    void fieldFromWidget(IField* field, QObject* widget);

    void fieldToWidget(IField* field, QObject* widget);

private: 
    /**
     * 子视图的容器Widget类型名称，例如：
     * QTabCtrl的子类
     * QButtonGroup的子类
     * QMdiArea的子类
     * 需要继承自公共的第二接口IWidgetContainer才行
     */
    QString m_widgetType;
    IField* m_field;                                                    //!< 当前字段视图对应的字段


    static FieldWidgetFactory* s_defaultWidgetFactory;                  //!< 默认的字段Widget工厂
};


#endif
/*********************************************************************************************************
** End of file
*********************************************************************************************************/