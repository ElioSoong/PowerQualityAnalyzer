/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  MsgBoxFrame.cpp
** Latest Version:             
** Latest modified Date:       
** Modified by:                
** Descriptions:               
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 Song Yunhua
** Created date:               2016/03/24 
** Descriptions:               
** 
*********************************************************************************************************/
#include <QPainter>
#include <QLayout>
#include <QAbstractButton>
#include <QPushButton>
#include <QTime>

#include "MsgBoxFrame.h"
#include "common/FocusManager.h"
#include "MainWindow.h"

QStack<QDialog *> MsgBoxFrame::s_widgetStack;

MsgBoxFrame::MsgBoxFrame(QWidget *parent, VisibleAreaPos visiblePos)
    : QDialog(parent), m_visiblePos(visiblePos)
{
#ifdef WIN32
    setParent(NULL);        //模态去边框窗口不能设置父窗口，否则无效
#else
    setParent(parent);      //
#endif // WIN32

    initMsgBoxFrame();
}

MsgBoxFrame::MsgBoxFrame(VisibleAreaPos visiblePos, QWidget *widget,  MsgBoxFrame::StdButtons buttons, QWidget *parent)
    : m_visiblePos(visiblePos)
{
#ifdef WIN32
    setParent(NULL);        //模态去边框窗口不能设置父窗口，否则无效
#else
    setParent(parent);      //
#endif // WIN32

    initMsgBoxFrame();
    addButtons(buttons);
    setCentralWidget(widget);
}

MsgBoxFrame::~MsgBoxFrame()
{

}

void MsgBoxFrame::paintEvent(QPaintEvent *e)
{
    //设置背景颜色以及透明度
    QPainter painter(this);
    painter.fillRect(this->rect(), QColor(0, 0, 0, 155));
}

void MsgBoxFrame::showEvent(QShowEvent *event)
{
    m_clickedBtn = NULL;        //清空已点击按钮的记录值

#if 0
    QWidget *widget = F280Application::focusWidget();
    if (widget)
    {
        widget->clearFocus();
    }
#endif
    MainWindow::instance()->releaseKeyboard();
    grabKeyboard();
    //重新调整button的大小
    QBoxLayout *layout = static_cast<QBoxLayout *>( m_buttonBox->layout());

    //设置默认按钮
    int itemCount = layout->count();
    while(itemCount)
    {
        QAbstractButton *btn = (QAbstractButton *)(layout->itemAt(--itemCount)->widget());
        if (btn)
        {
            btn->activateWindow();
            btn->setFocus();
        }
    }

    layout->setContentsMargins(0, 5, 0, 10);
    QList<QAbstractButton *> buttons = m_buttonBox->buttons();
    foreach(QAbstractButton *btn, buttons)
    {
        btn->setFixedSize(152, 40);
        btn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    }

    //防止弹出窗是静态对象时，隐藏后，再次显示的时候无显示内容
    if (m_undeletableWidget && !m_undeletableWidget->parent())
    {
        setCentralWidget(m_undeletableWidget);
    }

    pushWidget(this);
    
    QDialog::showEvent(event);

    if (m_undeletableWidget)
    {
        m_undeletableWidget->show();
    }
}

void MsgBoxFrame::hideEvent(QHideEvent *event)
{
    this->releaseKeyboard();
    MainWindow::instance()->grabKeyboard();
    if (m_undeletableWidget)
    {
        QObject *obj = m_undeletableWidget->parent();
        m_undeletableWidget->setParent(NULL);
        m_undeletableWidget->hide();
    }

    popWidget();
    if (!s_widgetStack.isEmpty())
    {
        QDialog *widget = s_widgetStack.top();
        if (widget)
        {
            widget->activateWindow();
            widget->setFocus();
            widget->grabKeyboard();
        }
    }

    QDialog::hideEvent(event);
}

void MsgBoxFrame::keyPressEvent(QKeyEvent *event)
{
    int key = event->key();
    Qt::KeyboardModifiers modifier = event->modifiers();

    if (key == Qt::Key_P && modifier == Qt::ControlModifier)
    {
        MainWindow::instance()->snapshot();
        return;
    }

    if (key < Qt::Key_A || key > Qt::Key_Z)
    {
        modifier = modifier &~ Qt::ControlModifier;
        event->setModifiers(modifier);
    }

    if (event->modifiers() & (Qt::ShiftModifier))
    {
        if (m_eventTarget)
        {
            ((QObject *)(m_eventTarget))->event(event);
            return;
        }
    }

    if(key == Qt::Key_Escape)
    {
        static QTime lockTime;
        if (!event->isAutoRepeat())
            lockTime = QTime::currentTime();
        if (event->isAutoRepeat() && lockTime.isValid() && lockTime.msecsTo(QTime::currentTime()) >= 3000)
        {
            lockTime = QTime(100, 100, 100);
            MainWindow::instance()->showScreenLock(!MainWindow::instance()->isScreenLocked(), this);
        }

        //Esc按键默认取消任何操作
        m_clickedBtn = button(MsgBoxFrame::Cancel);
    }

    FocusManager::instance()->setCurObj(this);
    static_cast<QObject *>(FocusManager::instance())->event(event);
}

void MsgBoxFrame::keyReleaseEvent(QKeyEvent *event)
{
    int key = event->key();
    if(key == Qt::Key_Escape && !event->isAutoRepeat() && !MainWindow::instance()->isScreenLocked() && !MainWindow::instance()->isScreenLockVisible())
    {
        close();
        return;
    }
}

void MsgBoxFrame::focusInEvent(QFocusEvent *event)
{
    QDialog::focusInEvent(event);
    m_buttonBox->setFocus();
}

bool MsgBoxFrame::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == m_buttonBox && event->type() == QEvent::LanguageChange)
    {
        return true;
    }
    return QDialog::eventFilter(obj, event);
}

bool MsgBoxFrame::event(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange)
    {
        retranslateStrings();
    }
    return QDialog::event(event);
}

/*! 
 * 初始化界面属性
 */
void MsgBoxFrame::initMsgBoxFrame()
{
    m_eventTarget = NULL;
    m_buttonBox   = new QDialogButtonBox(this);
    m_buttonBox->setCenterButtons(true);
    m_centralWidget = new QWidget(this);
    initCentralWidget();

    //窗体属性：模态，隐藏，大小
    setWindowModality(Qt::ApplicationModal);
    resize(WIDGET_WIDTH, WIDGET_HEIGHT);
    if (parent())
    {
        move(0, 0);
    }

    //去边框，背景透明
    setWindowFlags(Qt::FramelessWindowHint); 
    setAttribute(Qt::WA_TranslucentBackground);

    //重建布局
    initLayout();

    /*! 
     * 过滤buttonBox的changeEvent事件
     */
    m_buttonBox->installEventFilter(this);

    connect(m_buttonBox, SIGNAL(clicked(QAbstractButton *)), this, SIGNAL(buttonClicked(QAbstractButton *)));
    connect(m_buttonBox, SIGNAL(clicked(QAbstractButton *)), this, SLOT(onBtnClicked(QAbstractButton *)));
}

void MsgBoxFrame::initCentralWidget()
{
    //可视区域背景
    m_centralWidget->setAutoFillBackground(true);
    QPalette palette;
    palette.setBrush(m_centralWidget->backgroundRole(),Qt::white);
    m_centralWidget->setPalette(palette);

    //在布局中的大小适应属性
    m_centralWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    //在可视区域添加布局，方便添加窗口后，自动布局
    m_centralLayout = new QVBoxLayout(m_centralWidget);
    m_centralLayout->setMargin(0);

    m_centralWidget->setLayout(m_centralLayout);

    if (!m_buttonBox->buttons().empty())
    {
        m_centralLayout->addWidget(m_buttonBox, 0, Qt::AlignCenter);
    }
}

/*! 
 * 初始化布局
 */
void MsgBoxFrame::initLayout()
{
    QLayout *curLayout = layout();
    int itemCount = curLayout ? curLayout->count() : 0;
    while (itemCount)
    {
        QLayoutItem *item = curLayout->takeAt(--itemCount);
    }

    if(m_visiblePos == Center)
    {
        centralLayout();
    }
    else if(m_visiblePos == Bottom)
    {
        bottomLayout();
    }
    else
    {
        fullWidgetLayout();
    }
}

/*! 
 * 可视区域显示在中心区域
 */
void MsgBoxFrame::centralLayout()
{
    QGridLayout *gridLayout = new QGridLayout(this);

    QSpacerItem *topSpacer = new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Expanding);
    QSpacerItem *rightSpacer = new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Minimum);
    QSpacerItem *bottomSpacer = new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Expanding);
    QSpacerItem *leftSpacer = new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Minimum);

    gridLayout->addItem(topSpacer, 1, 2);
    gridLayout->addItem(rightSpacer, 2, 3);
    gridLayout->addItem(bottomSpacer, 3, 2);
    gridLayout->addItem(leftSpacer, 2, 1);

    gridLayout->addWidget(m_centralWidget, 2, 2);
    setLayout(gridLayout);
}

/*! 
 * 可视区域显示在底部
 */
void MsgBoxFrame::bottomLayout()
{
    QVBoxLayout *vLayout = new QVBoxLayout(this);
    vLayout->setMargin(0);

    QSpacerItem *vSpacer = new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Expanding);
    vLayout->addSpacerItem(vSpacer);
    vLayout->addWidget(m_centralWidget);
}

/*! 
 * 可视区域在覆盖整个窗口
 */
void MsgBoxFrame::fullWidgetLayout()
{
    QVBoxLayout *vLayout = new QVBoxLayout(this);
    vLayout->setMargin(0);
    vLayout->addWidget(m_centralWidget);
}

/*! 
 * 设置中心显示窗体
 */
void MsgBoxFrame::setCentralWidget(QWidget *widget)
{
    //打破布局,重新布局
    int itemCount = m_centralLayout->count();
    while(itemCount)
    {
        m_centralLayout->takeAt(--itemCount);
    }

    if (!widget)
    {
        widget = new QWidget();         
    }
    else
    {
        setundeletableWidget(widget);       //若widget是有效的，则不能随父组件删除而删除
    }
    widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_centralLayout->addWidget(widget);

    m_centralLayout->addWidget(m_buttonBox);
    if (m_buttonBox->buttons().size() == 0)
    {
        m_buttonBox->hide();
    }
}

QWidget* MsgBoxFrame::centralWidget()
{
    return m_centralWidget;
}

/*! 
 * 添加按钮
 */
QPushButton* MsgBoxFrame::addButton(MsgBoxFrame::StdButton button)
{
    m_buttonBox->show();
    QPushButton *btn = m_buttonBox->addButton(QDialogButtonBox::StandardButton(button));
    setButtonAttr(QDialogButtonBox::StandardButton(button));
    return btn;
}

QPushButton* MsgBoxFrame::addButton(const QString &text, const QIcon icon)
{
    QPushButton *btn = m_buttonBox->addButton(text, QDialogButtonBox::NoRole);
    btn->setIcon(icon);;
    m_buttonBox->show();
    return btn;
}

void MsgBoxFrame::addButtons(MsgBoxFrame::StdButtons buttons)
{
    uint i = MsgBoxFrame::FirstButton;
    while (i <= MsgBoxFrame::LastButton) 
    {
        if (i & buttons) 
        {
            QDialogButtonBox::StandardButton stdBtn = QDialogButtonBox::StandardButton(i);
            m_buttonBox->addButton(stdBtn);
            setButtonAttr(QDialogButtonBox::StandardButton(i));
        }
        i = i << 1;
    }
    m_buttonBox->show();
}

void MsgBoxFrame::addButton(QAbstractButton *button)
{
    m_buttonBox->addButton(button, QDialogButtonBox::NoRole);
    m_buttonBox->show();
}

void MsgBoxFrame::setButtonAttr(QDialogButtonBox::StandardButton button)
{
    QPushButton *btn = m_buttonBox->button(button);
    if (btn)
    {
        switch (button)
        {
        case MsgBoxFrame::Ok:
            btn->setIcon(OK_ICON);
            btn->setText(tr("确认"));
            m_textToBtn.insert("确认", btn);
            break;

        case MsgBoxFrame::Cancel:
            btn->setIcon(CANCEL_ICON);
            btn->setText(tr("取消"));
            m_textToBtn.insert("取消", btn);
            break;

        case MsgBoxFrame::Save:
            btn->setIcon(SAVE_ICON);
            btn->setText(tr("保存"));
            m_textToBtn.insert("保存", btn);
            break;

        case MsgBoxFrame::Reset:
            btn->setText(tr("重置"));
            m_textToBtn.insert("重置", btn);
            break;

        case MsgBoxFrame::Yes:
            btn->setText(tr("是"));
            m_textToBtn.insert("是", btn);
            break;

        case MsgBoxFrame::No:
            btn->setText(tr("否"));
            m_textToBtn.insert("否", btn);
            break;

        case MsgBoxFrame::Retry:
            btn->setText(tr("重试"));
            m_textToBtn.insert("重试", btn);
            break;

        case MsgBoxFrame::Close:
            btn->setText(tr("关闭"));
            m_textToBtn.insert("关闭", btn);
            break;

        default:
            break;
        }
    }
}

/*! 
 * 获取按钮
 */
QPushButton* MsgBoxFrame::button(MsgBoxFrame::StdButton btn)
{
    return m_buttonBox->button(QDialogButtonBox::StandardButton(btn));
}

QPushButton* MsgBoxFrame::button(const QString &name)
{
    QList<QAbstractButton *> btns = m_buttonBox->buttons();
    foreach(QAbstractButton *btn, btns)
    {
        if (btn->text() == name)
        {
            return static_cast<QPushButton *>(btn);
        }
    }
    return NULL;
}

void MsgBoxFrame::setTargetEvent(QWidget *widget)
{
    m_eventTarget = widget;
}

/*! 
 * 通用的弹出窗口，自定义窗口以弹出窗的形式显示
 * @param[in]  widget: 显示的自定义窗口， buttons: 默认的按钮
 * @return     返回点击的按钮
 */
MsgBoxFrame::StdButton MsgBoxFrame::commonMsgBox(VisibleAreaPos pos, QWidget *widget, MsgBoxFrame::StdButtons buttons)
{
    MsgBoxFrame msgBox(pos, widget, buttons);
    msgBox.exec();
    return MsgBoxFrame::StdButton(msgBox.buttonBox()->standardButton(msgBox.clickedButton()));
}

/*! 
 * 设置可视区域的位置
 */
void MsgBoxFrame::setVisiblePos(VisibleAreaPos pos)
{
    m_visiblePos = pos;
    initLayout();
}

/*! 
 * 返回当前可视区域的位置
 */
MsgBoxFrame::VisibleAreaPos MsgBoxFrame::visiblePos()
{
    return m_visiblePos;
}

/*! 
 * 获取当前被点击按钮
 */
void MsgBoxFrame::onBtnClicked(QAbstractButton *button)
{
    m_clickedBtn = button;
}

QAbstractButton* MsgBoxFrame::clickedButton()
{
    return m_clickedBtn;
}

/*! 
 * 获取按钮容器
 */
QDialogButtonBox* MsgBoxFrame::buttonBox()
{
    return m_buttonBox;
}

void MsgBoxFrame::clear()
{
    if (m_buttonBox)
    {
        m_buttonBox->clear();
    }
}

void MsgBoxFrame::setundeletableWidget(QWidget *widget)
{
    m_undeletableWidget = widget;
}

void MsgBoxFrame::retranslateStrings()
{
    QMap<QByteArray, QPushButton*>::iterator it = m_textToBtn.begin();
    while (it != m_textToBtn.end())
    {
        QAbstractButton *btn = it.value();
        btn->setText(tr(it.key().constData()));
        QString str = btn->text();
        ++it;
    }
}

/*! 
 * 保存上一层界面
 */
void MsgBoxFrame::pushWidget(QDialog *widget)
{
    s_widgetStack.push(widget);
}

QDialog* MsgBoxFrame::popWidget()
{
    return s_widgetStack.pop();
}

QDialog* MsgBoxFrame::topWidget()
{
    return s_widgetStack.top();
}

QStack<QDialog *>& MsgBoxFrame::widgetStack()
{
    return s_widgetStack;
}

void MsgBoxFrame::closeByOk()
{
    m_isDone = true;
    close();
}

void MsgBoxFrame::closeByCancel()
{
    m_isDone = false;
    close();
}

bool MsgBoxFrame::thingsToBeDone()
{
    return m_isDone;
}

void MsgBoxFrame::closeAllMsgBox()
{
    while (!s_widgetStack.isEmpty())
    {
        s_widgetStack.top()->close();
    }
}

/*********************************************************************************************************
** End of file
*********************************************************************************************************/
