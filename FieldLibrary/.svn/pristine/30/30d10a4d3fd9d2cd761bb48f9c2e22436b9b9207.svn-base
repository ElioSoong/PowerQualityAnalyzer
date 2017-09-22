#pragma once
#include "IWidgetContainer.h"
#include <QGroupBox>
#include <QTabWidget>
#include <QList>
#include <QButtonGroup>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QStringList>
#include <QScrollArea>
class QAbstractButton;
class QGridLayout;

Q_PLUGIN_METADATA(IID IWidgetContainer_IID FILE "loadIFieldWidget.json")
class FIELDLIBRARY_EXPORT GroupBox : public QGroupBox, public IWidgetContainer
{
    Q_OBJECT
        Q_INTERFACES(IWidgetContainer)
public:
    explicit GroupBox(QWidget *parent = NULL);
    explicit GroupBox(const QString &title, QWidget *parent = NULL);
    ~GroupBox();
    void setTitle(const QString& title) Q_DECL_OVERRIDE
    {
        QGroupBox::setTitle(title);
    }

    QString title() const Q_DECL_OVERRIDE
    {
        return QGroupBox::title();
    }

    int addWidget(QWidget* widget) Q_DECL_OVERRIDE;

    bool removeWidget(QWidget* widget) Q_DECL_OVERRIDE;

protected:
    void showEvent(QShowEvent *event);

private:
    QVBoxLayout*        m_layout;
    bool                m_isRelayoutWidget;//!< 窗体第一次显示标记，防止多次调用showEvent中的操作
};

/*********************************************************************************************************************/
class FIELDLIBRARY_EXPORT BoxLayoutWidget : public QWidget, public IWidgetContainer
{
    Q_OBJECT
        Q_INTERFACES(IWidgetContainer)
public:
    explicit BoxLayoutWidget(QWidget* parent = NULL);
    explicit BoxLayoutWidget(LayoutFlag direction, QWidget *parent = NULL);
    ~BoxLayoutWidget();
    void setTitle(const QString& title) Q_DECL_OVERRIDE
    {
        QWidget::setWindowTitle(title);
    }

    QString title() const Q_DECL_OVERRIDE
    {
        return QWidget::windowTitle();
    }

    /*!
    * 布局里面添加/移除部件
    */
    int addWidget(QWidget* widget) Q_DECL_OVERRIDE;
    bool removeWidget(QWidget* widget) Q_DECL_OVERRIDE;
    /*!
     * 选择布局方式，垂直布局或者水平布局。默认水平布局
    */
    void setLayoutFlag(const LayoutFlag flag);

protected:
    void resizeEvent(QResizeEvent* event);
    void showEvent(QShowEvent* event);

private:
    LayoutFlag                 m_layoutFlag;
    QBoxLayout*                m_layout;
    bool                       m_isRelayoutWidget;//!< 窗体第一次显示标记，防止多次调用showEvent中的操作
};
/*********************************************************************************************************************/
#include <QStackedWidget>
class FIELDLIBRARY_EXPORT StackedWidget : public QStackedWidget, public IWidgetContainer
{
    Q_OBJECT
        Q_INTERFACES(IWidgetContainer)
public:
    explicit StackedWidget(QWidget *parent = NULL);
    ~StackedWidget();

    int addWidget(QWidget* widget) Q_DECL_OVERRIDE;
    bool removeWidget(QWidget* widget) Q_DECL_OVERRIDE;
};

/*********************************************************************************************************************/
class FIELDLIBRARY_EXPORT TabWidget : public QTabWidget, public IWidgetContainer
{
    Q_OBJECT
        Q_INTERFACES(IWidgetContainer)
public:
    explicit TabWidget(QWidget *parent = NULL);
    ~TabWidget();
    void setTitle(const QString& title) Q_DECL_OVERRIDE
    {
        QTabWidget::setWindowTitle(title);
    }

    QString title() const Q_DECL_OVERRIDE
    {
        return QTabWidget::windowTitle();
    }
    /*!
    * 添加/移除tab页
    */
    int addWidget(QWidget* widget) Q_DECL_OVERRIDE;
    bool removeWidget(QWidget* widget) Q_DECL_OVERRIDE;
    /*!
     * 设置每个Tab也的显示文本
     */
    void setTextGroup(const QStringList& textList) Q_DECL_OVERRIDE
    {
        m_textList = textList;
    }
    

private:
    QList<QWidget*>         m_childWidget;
    QStringList             m_textList;
    int                     m_index;
};

/*********************************************************************************************************************/

class FIELDLIBRARY_EXPORT ButtonMenuWidget : public QWidget, public IWidgetContainer
{
    Q_OBJECT
        Q_INTERFACES(IWidgetContainer)
        Q_PROPERTY(QStringList buttonText READ buttonText WRITE setButtonText)

public:

    explicit ButtonMenuWidget(QWidget *parent = 0);
    virtual ~ButtonMenuWidget();

    /*!
     * 向切换区域添加响应按钮切换的窗口
     */
    int addWidget(QWidget* widget) Q_DECL_OVERRIDE;
    /*!
     * 移除切换区域的窗口
     */
    bool removeWidget(QWidget* widget) Q_DECL_OVERRIDE;

    /*!
     * 按钮间的间距
     */
    void setButtonSpacing(int spacing)
    {
        m_buttonLayout->setSpacing(spacing);
    }

    /*!
     * \brief setButtonText 设置ButtonMenuWidget里面每个Button的text
     * \param buttonText
     */
    void setButtonText(const QStringList& buttonText);

    QStringList buttonText() const
    {
        return m_buttonText;
    }
    void setTextGroup(const QStringList& textList) Q_DECL_OVERRIDE
    {
        setButtonText(textList);
    }
    /*!
     * 创建一组按钮
     */
    void createButton(int count);

signals:
    void buttonClicked(int id);
    void buttonClicked(QAbstractButton* buttn);

protected:
    void resizeEvent(QResizeEvent *event);
    void showEvent(QShowEvent *event);
    bool eventFilter(QObject *obj, QEvent *event);

private:
    int addButton(QAbstractButton *btn, int id);

private slots:
    void changedDisplayWidget(int index);

private:
    QButtonGroup*    m_buttonGroup;
    QStringList      m_buttonText;
    QHBoxLayout*     m_mainLayout;
    QVBoxLayout*     m_buttonLayout;
    QStackedWidget*  m_rightRespondRect;
    bool             m_isRelayoutWidget;//!< 窗体第一次显示标记，防止多次调用showEvent中的操作
};

/**************************************scrollAreaWidget**********************************/

class FIELDLIBRARY_EXPORT scrollAreaWidget : public QScrollArea, public IWidgetContainer
{
    Q_OBJECT
        Q_INTERFACES(IWidgetContainer)
public:
    explicit scrollAreaWidget(QWidget* parent = NULL);
    explicit scrollAreaWidget(LayoutFlag flag, QWidget* parent = NULL);
    virtual ~scrollAreaWidget();

    void setTitle(const QString& title) Q_DECL_OVERRIDE
    {
        QWidget::setWindowTitle(title);
    }
    /*!
     * 设置布局方式，垂直布局、水平布局、栅格布局
     */
    void setLayoutFlag(LayoutFlag flag);

    /*!
     * 布局管理器添加子窗口，显示到滚动窗口
     */
    int addWidget(QWidget* widget) Q_DECL_OVERRIDE;
    bool removeWidget(QWidget* widget) Q_DECL_OVERRIDE
    {
        Q_UNUSED(widget);
        delete takeWidget();
        return true;
    }
    /*!
     *  设置滚动窗口显示的窗体
     */
    void setWidget(QWidget* widget);

    // 水平方向、垂直方向自适应大小
    void setWidgetResizable(bool resizeable)
    {
        m_heightResizeable = m_widthResizeable = resizeable;
    }
    // 水平方向自适应大小
    void setWidgetHeightResizeable(bool resizeable)
    {
        m_heightResizeable = resizeable;
    }
    // 垂直方向自适应大小
    void setWidgetWidthResizeable(bool resizeable)
    {
        m_widthResizeable = resizeable;
    }

protected:
    void resizeEvent(QResizeEvent *event);

private:
    void createLayout();

private:
    QWidget*        m_areaWidget;
    LayoutFlag      m_flag;     //!< 子窗口的布局方式
    QBoxLayout*     m_boxLayout;
    QGridLayout*    m_gridLayout;
    bool            m_heightResizeable;
    bool            m_widthResizeable;
};

/**********************************************WidgetGroup************************************/

class QCheckBox;
class IFieldWidget;
#include <QVariant>
#include <QMap>
/*!
 * 实现类似于QButtonGroup的功能，支持QLineEdit、QComboBox、QSpinbox、QDoubleSpinbox、QLabel和QAbstructButton的子类
 * 进行分组。QCheckBox的setExclusive设置为false，QCheckBox不支持唯一触发。
 */
class FIELDLIBRARY_EXPORT WidgetGroup : public QButtonGroup, public IWidgetContainer
{
    Q_OBJECT
        Q_INTERFACES(IWidgetContainer)

public:
    enum WIDTYPE
    {
        TypeNull = 0,
        SwitchType,
        LineEditType,
        ComboBoxType,
        SpinBoxType,
        DoubleSpinBoxType,
        ButtonType,
    };
    typedef WIDTYPE WIDTYPES;
    Q_ENUMS(WIDTYPES)
    explicit WidgetGroup(QObject* parent = NULL);
    ~WidgetGroup();
    int addWidget(QWidget* widget) Q_DECL_OVERRIDE;
    bool removeWidget(QWidget* widget) Q_DECL_OVERRIDE;
    void clear();

    Q_SIGNAL void propertyValueChanged(int, QVariant);
    /*!
     * 查找widget的所有子窗口，包括父窗口能够追踪到widget的子窗口,
     * 适用于查找以布局管理器添加子窗口的窗口
     */
    QList<QWidget*> findAllChild(QWidget* widget);
    /*!
     * 查找widget的第一级所有子窗口，* 适用于查找以布局管理器添加子窗口的窗口
     */
    QList<QWidget*> findDirectAllChild(QWidget* widget);

    int childIndex() const
    {
        return m_index;
    }

    QVariant childValue() const
    {
        return m_variant;
    }

private:
    int widgetType(QWidget* widget) const;
    /*!
      得到Button组中IFieldWidget的valueProperty变化值
     */
    Q_SLOT void valueIndexOfPropery(QAbstractButton* button);

    bool removeOne(QWidget* widget);
    int addOne(QWidget* widget);
private:
    QString                                  m_notifyPropertyName;
    WIDTYPES                                 m_firstWidgetType;
    QMap<QCheckBox*, IFieldWidget*>          m_mapIFieldWidget;
    QList<QCheckBox*>                        m_childCheckBox;
    int                                      m_index;
    QVariant                                 m_variant;
};