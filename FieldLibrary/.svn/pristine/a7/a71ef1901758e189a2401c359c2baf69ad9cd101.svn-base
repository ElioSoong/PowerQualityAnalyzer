#include "FieldWidgetCreator.h"
#include "FieldView.h"

FieldWidgetCreator::FieldWidgetCreator(QObject* parent)
    : QObject(parent)
    , m_fieldViewFactory(NULL)
    , m_fieldWidgetFactory(NULL)
{

}

QObject* FieldWidgetCreator::createFieldWidget(QObject* rootField, bool createChildren /*= true*/)
{
    if (rootField && m_fieldViewFactory)
    {
        FieldView* rootFieldView = m_fieldViewFactory->createView(rootField, createChildren);
        if (rootFieldView)
        {
#if 1
            // TODO: 这里不应该设置默认的工厂，应该设置专用的工厂
            rootFieldView->setDefaultFieldWidgetFactory(m_fieldWidgetFactory);
#endif
            QObject* rootFieldWidget = rootFieldView->createWidget();
            return rootFieldWidget;
        }
    }

    return NULL;
}
