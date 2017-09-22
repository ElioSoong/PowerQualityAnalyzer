#include "includes.h"
#include "ComContainerWidget.h"
#include "ComWidget.h"
#include <QResizeEvent>
#include <QGridLayout>
#include <QScrollBar>
#include <QSpacerItem>

GroupBox::GroupBox(QWidget *parent /* = NULL */)
    : GroupBox("", parent)
{
}

GroupBox::GroupBox(const QString &title, QWidget *parent /* = NULL */)
    : QGroupBox(title, parent)
    , m_layout(new QVBoxLayout())
    , m_isRelayoutWidget(false)
{
    m_layout->setContentsMargins(0, 0, 0, 0);
    this->setLayout(m_layout);
}

GroupBox::~GroupBox()
{
}

int GroupBox::addWidget(QWidget* widget)
{
    m_layout->addWidget(widget);
    return m_layout->count();
}

bool GroupBox::removeWidget(QWidget* widget)
{
    m_layout->removeWidget(widget);
    widget->hide(); // delete widget;
    return true;
}

void GroupBox::showEvent(QShowEvent *event)
{
    if (!m_isRelayoutWidget)
    {
        for (int index = 0; index < position().size(); ++index)
        {
            QSpacerItem* spacerItem = new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding);
            m_layout->insertSpacerItem(position().at(index), spacerItem);
        }
        m_layout->setSpacing(spacing());
        m_isRelayoutWidget = true;
    }
    
    QWidget::showEvent(event);
}
/****************************BoxLayoutWidget*******************************/
BoxLayoutWidget::BoxLayoutWidget(QWidget* parent)
    : BoxLayoutWidget(Horizontal, parent)
{
    //QPalette pal(this->palette());

    ////设置背景色
    //pal.setColor(QPalette::Background, Qt::darkGray);
    //this->setAutoFillBackground(true);
    //this->setPalette(pal);
}

BoxLayoutWidget::BoxLayoutWidget(LayoutFlag direction, QWidget *parent)
    : QWidget(parent)
    , m_layoutFlag(direction)
    , m_isRelayoutWidget(false)
{
    if (direction == Horizontal)
    {
        m_layout = new QHBoxLayout();
    }
    else if (direction == Vertical)
    {
        m_layout = new QVBoxLayout();
    }
    m_layout->setContentsMargins(0, 0, 0, 0);
    this->setLayout(m_layout);
}

BoxLayoutWidget::~BoxLayoutWidget()
{
    delete m_layout;
    m_layout = NULL;
}

int BoxLayoutWidget::addWidget(QWidget* widget)
{
    m_layout->addWidget(widget);
    return m_layout->count();
}

bool BoxLayoutWidget::removeWidget(QWidget* widget)
{
    m_layout->removeWidget(widget);
    widget->hide(); // delete widget;
    return true;
}

void BoxLayoutWidget::setLayoutFlag(const LayoutFlag flag)
{
    if (m_layoutFlag != flag)
    {
        delete m_layout; m_layout = NULL;
        if (m_layoutFlag == Horizontal)
        {
            m_layout = new QHBoxLayout();
        }
        else if (m_layoutFlag == Vertical)
        {
            m_layout = new QVBoxLayout();
        }
        m_layout->setContentsMargins(0, 0, 0, 0);
        this->setLayout(m_layout);
    }
}

void BoxLayoutWidget::resizeEvent(QResizeEvent* event)
{
    if (m_layoutFlag == Horizontal)
    {
        this->setMaximumHeight(this->childrenRect().height() + m_layout->contentsMargins().top() + \
            m_layout->contentsMargins().bottom());
    }
    else if (m_layoutFlag == Vertical)
    {
        this->setMaximumWidth(this->childrenRect().width() + m_layout->contentsMargins().right() + \
            m_layout->contentsMargins().left());
    }

    QWidget::resizeEvent(event);
}

void BoxLayoutWidget::showEvent(QShowEvent* event)
{
    if (!m_isRelayoutWidget)
    {
        for (int index = 0; index < position().size(); ++index)
        {
            QSpacerItem* spacerItem = NULL;
            if (m_layoutFlag == Vertical)
            {
                spacerItem = new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding);
            }
            else if (m_layoutFlag == Horizontal)
            {
                spacerItem = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum);
            }

            m_layout->insertSpacerItem(position().at(index), spacerItem);
        }

        m_layout->setSpacing(spacing());
        for (int i = 0; i < strenchs().size(); ++i)
        {
            m_layout->setStretch(i, strenchs().at(i));
        }
        m_isRelayoutWidget = true;
    }
    
    QWidget::showEvent(event);
}

/******************************StackedWidget*********************************/
StackedWidget::StackedWidget(QWidget *parent /* = NULL */)
    : QStackedWidget(parent)
{
}

StackedWidget::~StackedWidget()
{
}

int StackedWidget::addWidget(QWidget* widget)
{
    return QStackedWidget::addWidget(widget);
}

bool StackedWidget::removeWidget(QWidget* widget)
{
    QStackedWidget::removeWidget(widget);
    widget->hide();
    return this->count();
}

/*******************************TabWidget************************************/
TabWidget::TabWidget(QWidget *parent /* = NULL */)
    : QTabWidget(parent)
    , m_textList()
    , m_index(0)
{
    m_textList.clear();
    m_childWidget.clear();
}

TabWidget::~TabWidget()
{
    m_textList.clear();
    m_childWidget.clear();
}

int TabWidget::addWidget(QWidget* widget)
{
    QString tabText = "";
    if (m_index < m_textList.size())
    {
        tabText = m_textList.at(m_index++);
    }
    m_childWidget.append(widget);
    return this->addTab(widget, tabText);
}

bool TabWidget::removeWidget(QWidget* widget)
{
    int index = 0;
    for (; index < m_childWidget.size(); ++index)
    {
        if (m_childWidget.at(index) == widget)
        {
            this->removeTab(index);
            return true;
        }
    }
    return false;
}

/*******************************ButtonMenuWidget**********************************/
ButtonMenuWidget::ButtonMenuWidget(QWidget *parent)
    : QWidget(parent)
    , m_buttonGroup(NULL)
    , m_buttonText()
    , m_buttonLayout(NULL)
    , m_rightRespondRect(NULL)
{
    m_buttonText.clear();
    m_mainLayout = new QHBoxLayout(this);
    m_buttonGroup = new QButtonGroup();
    m_rightRespondRect = new QStackedWidget();
    m_buttonLayout = new QVBoxLayout();

    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_buttonLayout->setContentsMargins(0, 0, 0, 0);
    m_buttonLayout->setSpacing(0);

    m_mainLayout->addLayout(m_buttonLayout);
    m_mainLayout->addWidget(m_rightRespondRect);

    connect(m_buttonGroup, SIGNAL(buttonClicked(int)), this, SLOT(changedDisplayWidget(int)));
    connect(m_buttonGroup, SIGNAL(buttonClicked(int)), this, SIGNAL(buttonClicked(int)));
    connect(m_buttonGroup, SIGNAL(buttonClicked(QAbstractButton*)), this, SIGNAL(buttonClicked(QAbstractButton*)));

    createButton(7);
}

ButtonMenuWidget::~ButtonMenuWidget()
{
    m_buttonText.clear();
    delete m_buttonGroup; m_buttonGroup = NULL;
    delete m_buttonLayout; m_buttonLayout = NULL;
    delete m_rightRespondRect; m_rightRespondRect = NULL;
    delete m_mainLayout; m_mainLayout = NULL;
}

int ButtonMenuWidget::addWidget(QWidget* widget)
{
    m_rightRespondRect->addWidget(widget);
    return m_rightRespondRect->count();
}

bool ButtonMenuWidget::removeWidget(QWidget* widget)
{
    m_rightRespondRect->removeWidget(widget);
    widget->hide();
    widget->deleteLater();
    return 1;
}

int ButtonMenuWidget::addButton(QAbstractButton *btn, int id)
{
    Q_ASSERT(btn != NULL);
    m_buttonGroup->addButton(btn, id);
    m_buttonLayout->addWidget(btn);

    return m_buttonLayout->count();
}

void ButtonMenuWidget::setButtonText(const QStringList &buttonText)
{
    m_buttonText = buttonText;
    while (m_buttonText.size() < m_buttonLayout->count())
    {
        m_buttonText.append(QString(""));
    }
    for (int index = 0; index < m_buttonLayout->count(); ++index)
    {
        QWidgetItem* widgetItem = static_cast<QWidgetItem*> (m_buttonLayout->itemAt(index));
        QAbstractButton* btn = qobject_cast<QAbstractButton*>(widgetItem->widget());
        btn->setText(m_buttonText.at(index));
        if (!btn->text().isEmpty())
        {
            btn->setCheckable(true);
            if (m_buttonGroup->id(btn) == 0)
            {
                btn->setChecked(true);
            }
        }
    }
}

void ButtonMenuWidget::createButton(int count)
{
    for (int i = 0; i < count; i++)
    {
        QPushButton *button = new QPushButton();
        button->installEventFilter(this);
        addButton(button, i);
    }
}

void ButtonMenuWidget::changedDisplayWidget(int index)
{
    if (index < m_rightRespondRect->count())
    {
        m_rightRespondRect->setCurrentIndex(index);
    }
}

void ButtonMenuWidget::resizeEvent(QResizeEvent *event)
{

    int buttonHeight = event->size().height() / m_buttonLayout->count();
    for (int i = 0; i < m_buttonLayout->count(); ++i)
    {
        QWidgetItem* item = static_cast<QWidgetItem*>(m_buttonLayout->itemAt(i));
        item->widget()->setMinimumHeight(buttonHeight);
    }

    m_rightRespondRect->setMaximumWidth(event->size().width()*0.8);

    return QWidget::resizeEvent(event);
}

void ButtonMenuWidget::showEvent(QShowEvent *event)
{
    if (!m_isRelayoutWidget)
    {
        m_mainLayout->setSpacing(spacing());
        repaint();
        m_isRelayoutWidget = true;
    }
   
    QWidget::showEvent(event);
}

bool ButtonMenuWidget::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonRelease
        || event->type() == QEvent::MouseButtonPress)
    {
        QPushButton* btn = static_cast<QPushButton*>(obj);
        if (btn && btn->text().isEmpty())
        {
            event->ignore();
            return true;
        }
    }
    return false;
}

/************************************************************************/
scrollAreaWidget::scrollAreaWidget(QWidget *parent)
    : scrollAreaWidget(Vertical, parent)
{

}
scrollAreaWidget::scrollAreaWidget(LayoutFlag flag, QWidget* parent)
    : QScrollArea(parent)
    , m_flag(flag)
    , m_heightResizeable(false)
    , m_widthResizeable(true)
    , m_boxLayout(NULL)
    , m_gridLayout(NULL)
{
    m_areaWidget = new QWidget();
    createLayout();
}

scrollAreaWidget::~scrollAreaWidget()
{
    delete m_boxLayout;
    delete m_gridLayout;
}

void scrollAreaWidget::setLayoutFlag(LayoutFlag flag)
{
    if (m_boxLayout)
    {
        delete m_boxLayout;
    }
    if (m_gridLayout)
    {
        delete m_gridLayout;
    }
    createLayout();
}

int scrollAreaWidget::addWidget(QWidget* widget)
{
    if (m_flag == Grid)
    {
        m_gridLayout->addWidget(widget);
        return m_gridLayout->count();
    }
    else
    {
        m_boxLayout->addWidget(widget);
        return m_boxLayout->count();
    }
}

void scrollAreaWidget::setWidget(QWidget* widget)
{
    QScrollArea::setWidget(widget);
}

void scrollAreaWidget::resizeEvent(QResizeEvent* event)
{
    if (!this->widget())
    {
        setWidget(m_areaWidget);
    }

    if (m_heightResizeable)
    {
        if (this->verticalScrollBar()->maximum())
        {
            this->widget()->setMaximumHeight(this->viewport()->size().height());
        }
        else
        {
            this->widget()->setMinimumSize(this->viewport()->size());
        }
    }
    else if (m_widthResizeable)
    {

        if (this->horizontalScrollBar()->maximum())
        {
            this->widget()->setMaximumWidth(this->viewport()->size().width());
        }
        else
        {
            this->widget()->setMinimumSize(this->viewport()->size());
        }
    }
    QScrollArea::resizeEvent(event);
}

void scrollAreaWidget::createLayout()
{
    if (m_flag == Horizontal)
    {
        m_boxLayout = new QBoxLayout(QBoxLayout::LeftToRight, m_areaWidget);
    }
    else if (m_flag == Vertical)
    {
        m_boxLayout = new QBoxLayout(QBoxLayout::TopToBottom, m_areaWidget);
    }
    else
    {
        m_gridLayout = new QGridLayout(m_areaWidget);
    }
}

/**********************************************WidgetGroup************************************/
#include <QMetaObject>
#include <QMetaMethod>
#include <QQueue>
#include <QWidgetItem>
WidgetGroup::WidgetGroup(QObject* parent)
    : QButtonGroup(parent)
    , m_notifyPropertyName("")
    , m_firstWidgetType(TypeNull)
    , m_index(-1)
    , m_variant(QVariant())
{
    m_childCheckBox.clear();
    m_mapIFieldWidget.clear();
    connect(this, SIGNAL(buttonClicked(QAbstractButton*)), SLOT(valueIndexOfPropery(QAbstractButton*)));
}

WidgetGroup::~WidgetGroup()
{
    clear();
}

int WidgetGroup::addWidget(QWidget* widget)
{
    QList<QWidget*> children = findAllChild(widget);
    if (children.isEmpty()) // widget不是容器
    {
        return addOne(widget);
    }

    foreach(QWidget* child, children)
    {
        addOne(child);
    }

    return this->buttons().size();
}

bool WidgetGroup::removeWidget(QWidget* widget)
{
    if (this->buttons().size())
    {
        QList<QWidget*> children = findAllChild(widget);
        if (children.isEmpty()) // widget不是容器
        {
            return removeOne(widget);
        }

        foreach(QWidget* child, children)
        {
            removeOne(child);
        }
    }
    return true;
}

void WidgetGroup::clear()
{
    m_firstWidgetType = TypeNull;
    m_mapIFieldWidget.clear();
    foreach(QCheckBox* btn, m_childCheckBox)
    {
        removeButton(btn);
        delete btn;
    }
    m_childCheckBox.clear();
}

QList<QWidget*> WidgetGroup::findAllChild(QWidget* widget)
{
    QList<QWidget*> childList = QList<QWidget*>();
    QList<QWidget*> directChildren = findDirectAllChild(widget);

    if (!directChildren.isEmpty())
    {
        QQueue<QList<QWidget*> > childrenQueue;
        childrenQueue.enqueue(directChildren);
        while (!childrenQueue.isEmpty())
        {
            QList<QWidget*> tmpChildList = childrenQueue.dequeue();
            int breakIndex = 0; // 中断索引，当两个具体部件中间的widget表示一个容器时，
            //    首先遍历容器最后添加最后一个子窗口
            foreach(QWidget* childWidget, tmpChildList)
            {
                breakIndex++;
                if (widgetType(childWidget) >= SwitchType)
                {
                    childList.append(childWidget);
                }
                else if (widgetType(childWidget) == TypeNull) // 当前遍历的子窗口为容器类型
                {
                    QList<QWidget*> nextLevelChildren = findDirectAllChild(childWidget);
                    while (breakIndex < tmpChildList.size()) // 将当前层次未遍历的子窗口添加到下一层
                    {
                        nextLevelChildren.append(tmpChildList.value(breakIndex));
                        breakIndex++;
                    }
                    if (!nextLevelChildren.isEmpty())
                    {
                        childrenQueue.enqueue(nextLevelChildren);
                    }
                    break; // 结束foreach循环，遍历下一层
                }

            }

        }
    }
    return childList;
}

QList<QWidget*> WidgetGroup::findDirectAllChild(QWidget* widget)
{
    if (!widget->layout())
    {
        return QList<QWidget*>();
    }

    int childCount = widget->layout()->count();
    int index = 0;
    QList<QWidget*> childList = QList<QWidget*>();

    while (index < childCount && widgetType(widget) != SwitchType) // 不遍历Switch的layout
    {
        QWidgetItem* widgetItem = static_cast<QWidgetItem*>(widget->layout()->itemAt(index));
        childList.append(widgetItem->widget());
        index++;
    }
    return childList;
}

int WidgetGroup::widgetType(QWidget* widget) const
{
    if (qobject_cast<LineEdit*>(widget))
    {
        return LineEditType;
    }
    else if (qobject_cast<ComboBox*>(widget))
    {
        return ComboBoxType;
    }
    else if (qobject_cast<SpinBox*>(widget))
    {
        return SpinBoxType;
    }
    else if (qobject_cast<DoubleSpinBox*>(widget))
    {
        return DoubleSpinBoxType;
    }
    else if (qobject_cast<Switch*>(widget))
    {
        return SwitchType;
    }
    else if (qobject_cast<QAbstractButton*>(widget))
    {
        return ButtonType;
    }

    return TypeNull;
}

void WidgetGroup::valueIndexOfPropery(QAbstractButton* button)
{
    int id = this->id(button);
    IFieldWidget* widgetSender = m_mapIFieldWidget.value(qobject_cast<QCheckBox*>(button));
    if (!widgetSender)
    {
        widgetSender = qobject_cast<IFieldWidget*>(button);
    }
    Q_ASSERT(widgetSender != NULL);
    qDebug() << "WidgetGroup.id:" << id << ",WidgetGroup.value:" << widgetSender->value();

    m_index = id;
    m_variant = widgetSender->value();

    emit propertyValueChanged(id, widgetSender->value());
}

bool WidgetGroup::removeOne(QWidget* widget)
{
    if (m_firstWidgetType == ButtonType && m_firstWidgetType == widgetType(widget))
    {
        removeButton(qobject_cast<QAbstractButton*>(widget));
    }
    else if (m_firstWidgetType != TypeNull && m_firstWidgetType == widgetType(widget))
    {
        IFieldWidget* fieldWidget = qobject_cast<IFieldWidget*>(widget);
        QCheckBox* deleteCheck = m_mapIFieldWidget.key(fieldWidget, NULL);
        if (deleteCheck)
        {
            removeButton(deleteCheck);
            m_childCheckBox.removeOne(deleteCheck);
            m_mapIFieldWidget.remove(deleteCheck);
            delete deleteCheck;
        }
    }
    return true;
}

int WidgetGroup::addOne(QWidget* widget)
{
    if (!m_firstWidgetType)
    {
        m_firstWidgetType = (WIDTYPES)widgetType(widget);
    }
    if (m_firstWidgetType == (WIDTYPES)widgetType(widget))
    {
        if (m_firstWidgetType == ButtonType) // RadioButton、Button、CheckBox
        {
            if (qobject_cast<CheckBox*>(widget))
            {
                this->setExclusive(false);
            }
            addButton(qobject_cast<QAbstractButton*>(widget), this->buttons().size());
            IFieldWidget* fieldWidget = qobject_cast<IFieldWidget*>(widget);
            if (fieldWidget)
            {
                fieldWidget->setGroup(this);
            }
            //disconnect(widget, 0, 0, 0);
            return this->buttons().size();
        }
        else if (m_firstWidgetType != TypeNull) // LineEdit、Switch、ComboBox、SpinBox、DoubleSpinBox
        {
            IFieldWidget *objWidget = qobject_cast<IFieldWidget*>(widget);
            m_notifyPropertyName = objWidget->valueProperty();

            QCheckBox* btnCheck = new QCheckBox();

            const QMetaObject* btnMetaObject = btnCheck->metaObject();
            QMetaMethod reciveSlot = btnMetaObject->method(btnMetaObject->indexOfSlot("click()"));

            const QMetaObject* widgetMetaObject = widget->metaObject();
            int widgetPropertyNotifyIndex = widgetMetaObject->indexOfProperty(m_notifyPropertyName.toStdString().c_str());
            if (widgetPropertyNotifyIndex)
            {
                QMetaProperty widgetProperty = widgetMetaObject->property(widgetPropertyNotifyIndex);
                QMetaMethod widgetNotifySignal = widgetProperty.notifySignal();
                Q_ASSERT(widgetNotifySignal.isValid());
                //disconnect(widget, 0, 0, 0);
                bool connectIsOk = connect(widget, widgetNotifySignal, btnCheck, reciveSlot);
                Q_ASSERT(connectIsOk);
            }
            m_mapIFieldWidget.insert(btnCheck, objWidget);
            m_childCheckBox.append(btnCheck);
            objWidget->setGroup(this);
            addButton(btnCheck, this->buttons().size());
            return this->buttons().size();
        }
    }
    return false;
}