#ifndef __FIELDWIDGETCREATOR_H__
#define __FIELDWIDGETCREATOR_H__
#include "includes.h"
#include "fieldlibrary_global.h"
#include "FieldViewFactory.h"
#include "FieldWidgetFactory.h"
#include "IField.h"

class FIELDLIBRARY_EXPORT FieldWidgetCreator : public QObject
{
    Q_OBJECT
public:
    explicit FieldWidgetCreator(QObject* parent = NULL);
    ~FieldWidgetCreator() {}
    /** 
     * 为指定的字段树枝创建动态界面
     * @param[in]  rootFieldObject  字段树枝根节点，可以是与字段一一对应的任何对象（通过FieldProxy访问）
     * @param[in]  createChildren   是否为字段根节点的孩子创建界面
     * @return     QObject*         创建的Widget/QML界面
     */
    virtual QObject* createFieldWidget(QObject* rootFieldObject, bool createChildren = true);
    QObject* createFieldWidget(const QString& rootFieldPath, bool createChildren = true)
    {
        IField* rootField = IField::at(rootFieldPath);
        return createFieldWidget(rootField, createChildren);
    }

    /*!
     * Getter/Setter for fieldViewFactory
     */
    FieldViewFactory* fieldViewFactory() const
    {
        return m_fieldViewFactory;
    }
    void setFieldViewFactory(FieldViewFactory* fieldViewFactory)
    {
        m_fieldViewFactory = fieldViewFactory;
    }

    /*!
     * Getter/Setter for fieldWidgetFactory
     */
    FieldWidgetFactory* fieldWidgetFactory() const
    {
        return m_fieldWidgetFactory;
    }
    void setFieldWidgetFactory(FieldWidgetFactory* fieldWidgetFactory)
    {
        m_fieldWidgetFactory = fieldWidgetFactory;
    }

private:
    FieldViewFactory*   m_fieldViewFactory; //!< 用于创建FieldView的factory，
    FieldWidgetFactory* m_fieldWidgetFactory; //!< 而FieldView则使用FieldWidgetFactory创建FieldWidget
};
#endif // __FIELDWIDGETCREATOR_H__