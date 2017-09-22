#pragma once

#include "FieldLibrary/IFieldWidget.h"
#include "FieldLibrary/IWidgetContainer.h"
#include <QWidget>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QGroupBox>

class MyContainer : public QGroupBox, public IWidgetContainer
{
    Q_OBJECT
    Q_INTERFACES(IWidgetContainer)

public:
    MyContainer()
    {
        setTitle(tr("Untitled"));
        m_layout = new QVBoxLayout(this);
    }

    virtual int addWidget(QWidget* widget) override
    {
        if (widget)
        {
            m_layout->addWidget(widget);
        }
        return m_layout->count();
    }


    virtual bool removeWidget(QWidget* widget) override
    {
        m_layout->removeWidget(widget);
        return true;
    }



    virtual QString title() const override
    {
        return QGroupBox::title();
    }


    virtual void setTitle(const QString& title) override
    {
        return QGroupBox::setTitle(title);
    }

protected:
    QVBoxLayout* m_layout;
};

class MyWidget : public QLineEdit, public IFieldWidget
{
    Q_OBJECT
    Q_INTERFACES(IFieldWidget)

public:
    MyWidget() : QLineEdit(), IFieldWidget(this, "text")
    {

    }
};
