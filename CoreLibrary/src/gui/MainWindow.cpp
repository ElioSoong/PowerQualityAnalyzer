﻿/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  MainWindow.cpp
** Latest Version:             V1.0.0
** Latest modified Date:       2016/3/2
** Modified by:                
** Descriptions:               
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 
** Created date:               2016/2/29
** Descriptions:               MainWindow class  [Generated by VP-UML, IGT1.0]
** 
*********************************************************************************************************/
#include "includes.h"
#include "common/Application.h"
#include "common/Activity.h"
#include "common/ActivityModel.h"
#include "gui/View.h"
#include "gui/HomepageWidget.h"
#include "gui/MainWindow.h"

#include <QKeyEvent>
#include <QTime>
#include <QMessageBox>

/*!
 * Constructor
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    m_currentActivity(NULL),
    m_currentView(NULL),
    m_homepage(NULL),
    m_setupView(NULL),
    m_aboutView(NULL),
    m_centralWidget(NULL),
    m_centralLayout(NULL),
    m_locked(false),
    m_calibrationScreenProcess(new QProcess)
{
    Q_ASSERT_X(s_instance == NULL, "MainWindow::MainWindow()", "Mainwindow class is in singleton mode.");
    s_instance = this;

    // 主窗口抓取程序的全部按键事件，并进行自定义拦截与分发
    grabKeyboard();

    m_centralWidget = new QWidget(this);
    m_centralWidget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    setCentralWidget(m_centralWidget);

    this->setObjectName("mainwindow");
    ((Application*)(qApp))->addJsonObject(this);

    //设置屏幕锁控件的属性
    m_screenLock.setParent(this);
    m_screenLock.setWindowFlags(Qt::FramelessWindowHint);
    m_screenLock.setAttribute(Qt::WA_TranslucentBackground);
    m_screenLock.hide();
    //设置隐藏屏幕锁使用的timer
    m_lockTimer.setInterval(2000);
    m_lockTimer.setSingleShot(true);
    connect(&m_lockTimer, SIGNAL(timeout()), &m_screenLock, SLOT(hide()));

    m_captureScreenIcon.setParent(this);
    m_captureScreenIcon.setWindowFlags(Qt::FramelessWindowHint);
    m_captureScreenIcon.setAttribute(Qt::WA_TranslucentBackground);
    m_captureScreenIcon.hide();
    //设置隐藏截屏图标使用的timer
    m_captureScreenTimer.setInterval(500);
    m_captureScreenTimer.setSingleShot(true);
    connect(&m_captureScreenTimer, SIGNAL(timeout()), &m_captureScreenIcon, SLOT(hide()));
}

MainWindow* MainWindow::s_instance = NULL;

/*! 
 * 获取主窗口的全局唯一实例
 * @return     主窗口的全局唯一实例，若不存在则创建之
 */
MainWindow* MainWindow::instance()
{
    Q_ASSERT(s_instance != NULL);
    return s_instance;
}

/*!
 * 截图并保存至指定的图片文件
 * @param[in]    savePath	 图片保存路径
 * @return       操作成功返回true
 */
bool MainWindow::snapshot(QWidget* widget, const QString& savePath)
{
    if (NULL == widget)
    {
        widget = this;
    }

    QPixmap pixmap = QPixmap::grabWidget(widget, widget->geometry());
    if (!savePath.isEmpty())
        return pixmap.save(savePath);

    return true;
}

/*!
 * 获得当前的测量活动
 * @return       测量活动，指任何从主界面选择进入的工作模式、测量功能之类的概念，例如故障检查、谐波测量、系统设置等
 */
Activity* MainWindow::currentActivity() const
{
    return this->m_currentActivity;
}

/*!
 * 重新启动测量活动
 * @return       操作成功返回true
 */
bool MainWindow::restartActivity()
{
    Activity *activity = currentActivity();
    stopActivity();
    return startActivity(activity);
}

/*!
 * 停止当前的测量活动
 * @return       操作成功返回true
 */
bool MainWindow::stopActivity()
{
    m_currentActivity = 0;
    return true;
}

void MainWindow::showWindow(bool show /*= true*/)
{
    if (show)
        this->show();
    else
        this->hide();
}

/*!
 * 启动一个测量活动
 * 一般来讲每个测量活动对应主页的一个图标，点击该图标则启动对应的测量活动
 * @param[in]    activity	 需要启动的测量活动对象
 * @return       操作成功返回true
 */
bool MainWindow::startActivity( Activity* activity )
{
    m_currentActivity = activity;

    if (!m_currentActivity->start())
    {
        qCritical() << "Failed to start activity " << activity->name();
        return false;
    }

    if (!navigateTo(m_currentActivity->defaultView(), true))
    {
        qCritical() << "Failed to navigate to the default view of " << m_currentActivity->name();
        return false;
    }

    return true;
}

/*!
 * 实现视图导航，即切换至指定页面，但由于基类不知道布局等具体信息，因此只能在每个项目中独立实现
 * @param[in]    view	 导航目标视图
 * @return       操作成功返回true
 */
bool MainWindow::navigateTo( View* view, bool canNavigateBack)
{
    // 确认确实为View或者View的子类
    view = qobject_cast<View*>(view);
    if (view == NULL)
        return false;

    // TODO: 针对构造函数可能会耗时很久的View.widget，启用延迟加载

    //将view压入堆栈
    if (m_viewStack.size() <= 1)
    {
        if (view == m_homepage)
        {
            m_viewStack.removeAll(view);
        }
        m_viewStack.push(view);
    }
    else
    {
        View* topWidget = m_viewStack.top();
        if (!canNavigateBack)
        {
            m_viewStack.removeAll(topWidget);
            m_viewStack.removeAll(view);
            m_viewStack.push(view);
        }
        else if (topWidget != view)
        {
            m_viewStack.removeAll(view);
            m_viewStack.push(view);
        }
    }

    bool ok = true;
    //设置主副标题
    if (!setTitle(view->title()) || !setSubTitle(view->subTitle()))
    {
        ok = false;
    }

    //设置要显示的主widget
    if (!setViewWidget(view))
    {
        ok = false;
    }

    //更新当前当前视图变量
    m_currentView = view;

    // 刷新界面显示
    //QMetaObject::invokeMethod(this, "show");
    //show();

    return ok;
}

/*!
 * 按键事件处理函数，请注意：
 * 1.需要在初始化时调用grabKeyboard才能拦截其他对象的按键事件
 * 2.如果不能处理当前事件，请将该事件转发给currentView，避免事件丢失（直接调用currentView的event即可）
 * 3.如果事件传递已经死循环，应尽可能做防护
 * @param[in]    event	 输入事件
 * @return       操作成功返回true
 */
bool MainWindow::event(QEvent* event)
{
    static QTime lockTime;
    switch (event->type())
    {
    case QEvent::KeyPress:
        {
            QKeyEvent            *keyEvent = static_cast<QKeyEvent *>(event);
            int                   key      = keyEvent->key();
            Qt::KeyboardModifiers modifier = keyEvent->modifiers();

            if (Qt::Key_Backspace == key || Qt::Key_Escape == key)
            {
                if (!keyEvent->isAutoRepeat())
                {
                    lockTime = QTime::currentTime();
                }
                if (keyEvent->isAutoRepeat() && lockTime.isValid() && lockTime.msecsTo(QTime::currentTime()) >= 3000)
                {
                    lockTime = QTime(100, 100, 100);
                    showScreenLock(!isScreenLocked(), this);
                }
                return true;
            }
            else if (Qt::Key_Home == key)
            {
                // 如果是HOME按键则返回主页
                navigateHome();
                return true;
            }
            else if (key >= Qt::Key_F1 && key <= Qt::Key_F12) 
            {
                // 如果是F1-F12则切换至预设值的快捷页面
                int index = key - Qt::Key_F1;
                if (index < m_functionViews.size())
                {
                    View* functionView = m_functionViews.at(index);
                    navigateTo(functionView);
                    return true;
                }
            }
            else //if (Qt::Key_Up == key || Qt::Key_Down == key || Qt::Key_Left == key || Qt::Key_Right == key)
            {
                // 如果是其他按键，则一律发送给中心子窗口，
                // 这种策略对于大多数应用是足够的，如果确实需要修改，可在子类中重载
                QWidget* currentWidget = m_currentView ? m_currentView->widget() : NULL;
                if (currentWidget) 
                {
                    return ((QObject*)currentWidget)->event(event);
                }
            }
        }
        break;
      
    case QEvent::KeyRelease:
        {
            QKeyEvent* keyEvent = static_cast<QKeyEvent *>(event);
            int key = keyEvent->key();
            if (Qt::Key_Backspace == key || Qt::Key_Escape == key)
            {
                if (!keyEvent->isAutoRepeat())
                {
                    if (lockTime.msecsTo(QTime::currentTime()) < 3000 
                        && !m_locked && m_screenLock.isHidden())
                    {
                        if (m_calibrationScreenProcess != NULL && m_calibrationScreenProcess->state() > 0)
                        {
                            m_calibrationScreenProcess->kill();
                        }
                        else
                        {
                            navigateBack();
                        }
                    }      
                }
                return true;
            }
        }
        break;

    default:
        break;
    }

    return QMainWindow::event(event);
}

/*!
 * 回到首页O
 * @return       操作成功返回true
 */
bool MainWindow::navigateHome()
{
    bool ok = false;

    if (currentActivity())
    {
        stopActivity();
    }

    m_viewStack.clear();

    if (m_homepage)
        ok = navigateTo(m_homepage, true);
    else
        qCritical() << "You MUST specify a homepage for the mainWindow.";

    return ok;
}

/*!
 * 向后导航
 * @return       操作成功返回true
 */
bool MainWindow::navigateBack()
{
    if (m_viewStack.size() <= 1)
        return true;

    View *curView = m_viewStack.pop();
    Q_UNUSED(curView);

    View* prevView = m_viewStack.top();
    m_viewStack.pop();
    
    return navigateTo(prevView, true);
}

/*! 
 * 槽函数，用于响应主界面中点击某个图标的点击信号，执行的操作为启动该图标对应的测量活动
 * @param[in]  activityIndex 图标对应的ActivityModel的模型索引
 */
void MainWindow::onHomepageActivated(const QModelIndex& activityIndex)
{
    const ActivityModel* model = qobject_cast<const ActivityModel*>(activityIndex.model());
    Q_ASSERT_X(model != NULL, "onHomepageActivated",
            "The model for homepage can only be an ActivityModel object");
    Activity* activity = model->activity(activityIndex);
    Q_ASSERT(activity);

    startActivity(activity);
}

bool MainWindow::setFullScreenWidget( QWidget *widget )
{
    if (!widget)
        return false;

    widget->setParent(m_centralWidget);
    while (m_centralLayout->takeAt(0));
    m_centralLayout->addWidget(widget);
    widget->setVisible(true);

    return true;
}

/*! 
 * 根据lock的状态显示锁
 * @return     void
 */
void MainWindow::showScreenLock(bool locked, QWidget* w)
{
    if (locked != isScreenLocked())
    {
        m_locked = locked;
        if (m_locked)
        {
            m_screenLock.setParent(w);
            grabMouse();
            m_screenLock.setPixmap(QPixmap("resources/icon/status/lock.png"));
        }
        else
        {
            releaseMouse();
            m_screenLock.setPixmap(QPixmap("resources/icon/status/unlock.png"));
        }
           
        m_screenLock.resize(m_screenLock.pixmap()->size());
        emit screenLocked(m_locked);
        m_screenLock.move(rect().center() - QPoint(m_screenLock.width() / 2, m_screenLock.height() / 2));
    }
     
    m_screenLock.show();
    //开启定时器，使图标在一定时间后消失
    m_lockTimer.start();
}

void MainWindow::showCaptureScreenIcon(QWidget* w /*= 0*/)
{
    //m_captureScreenIcon.setParent(w);
    m_captureScreenIcon.setPixmap(QPixmap("resources/icon/status/captureScreen.png"));
    m_captureScreenIcon.resize(m_captureScreenIcon.pixmap()->size());
    m_captureScreenIcon.move(rect().center() -
        QPoint(m_captureScreenIcon.width() / 2, m_captureScreenIcon.height() / 2));
    m_captureScreenIcon.show();
    //开启定时器，使图标在一定时间后消失
    m_captureScreenTimer.start();
}
/*! 
 *  校准屏幕
 */
void MainWindow::CalibrationScreen()
{
    QStringList args;
    args << "-f" << "-path" << "/sys/bus/platform/devices/e0001000.serial/";
    m_calibrationScreenProcess->start("./CalibrateScreen", args);
}

/*!
 * 当前是否处于锁屏状态
 * @return     bool
 */
bool MainWindow::isScreenLocked() const
{
    return m_locked;
}

/*! 
 * 判断屏幕锁是否可见
 * @return     bool
 */
bool MainWindow::isScreenLockVisible() const
{
    return m_screenLock.isVisible() ? true : false;
}

/*********************************************************************************************************
** End of file
*********************************************************************************************************/