/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  FocusManager.cpp
** Latest Version:             
** Latest modified Date:       
** Modified by:                
** Descriptions:               
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 Song Yunhua
** Created date:               2015/11/25 
** Descriptions:               管理组件焦点
** 
*********************************************************************************************************/
#include "FocusManager.h"
#include "Application.h"
#include <QAbstractSpinBox>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QAbstractItemView>

FocusManager::FocusManager(QObject *parent)
    :QObject(parent)
{
}

FocusManager::~FocusManager()
{
    if (s_instance)
    {
        delete s_instance;
        s_instance = NULL;
    }
}

FocusManager* FocusManager::s_instance = NULL;
FocusManager* FocusManager::instance()
{
    if (!s_instance)
    {
        s_instance = new FocusManager;
    }
    return s_instance;
}

/*! 
 * 遍历传入对象的子组件
 */
QList<QWidget *>& FocusManager::traversingChildObj(QObject *obj)
{
    if (obj)
    {
        QWidget *widget = qobject_cast<QWidget *>(obj);

        if (filterComponent(widget))
        {
            m_childrenList.append(widget);
        }
        //遍历子组件
        QObjectList childrenList = obj->children();
        if (!childrenList.isEmpty())
        {
            for (int i = 0; i < childrenList.size(); i++)
            {
                traversingChildObj(childrenList.at(i));
            }
        }
    }
    return m_childrenList;
}

/*! 
 * 过滤掉不需要焦点的组件
 */
bool FocusManager::filterComponent(QWidget *widget)
{
    if (widget)
    {
        QString objName = widget->objectName();
        QString className = widget->metaObject()->className();
        if (objName.contains("qt_") || objName == "radioLineEdit" || 
            className == "QComboBoxListView" || !widget->isVisible() || 
            className == "QScrollArea" || className == "QToolButton" || 
            className == "QTabWidget" || className == "DateTimeEdit")
        {
            return false;
        }
        else if (className == "TitleExpand" || className == "myLabel")
        {
            return true;
        }
        else if (widget && widget->isVisible() && widget->focusPolicy() != Qt::NoFocus)
        {
            return true;
        }
    }
    return false;
}

/*! 
 * 初始化组件列表，使得第一个可获得焦点的组件获取焦点
 */
void FocusManager::initWidgetList(QObject *obj)
{
    m_childrenList.clear();
    traversingChildObj(obj);
    if (!m_childrenList.isEmpty())
    {
        m_mark = m_markHash.keys().contains(obj) ? m_markHash.value(obj) : -1;
        //判断当前焦点位于哪个子组建
        for (int i = 0; i < m_childrenList.size(); i++)
        {
            QWidget *temp = m_childrenList.at(i);
            if (temp && temp->hasFocus())
            {
                m_mark = i;
                return;
            }
        }
    }
}

/*! 
 * 移动焦点到下一个组件
 */
void FocusManager::moveNextPrevFocus(bool next)
{
    if (!m_childrenList.isEmpty())
    {
        //继续移动焦点到下一个组件
        if (next)
        {
            if (++m_mark >= m_childrenList.size())
            {
                m_mark = 0;
            }
        }
        else
        {
            if (--m_mark < 0 )
            {
                m_mark = m_childrenList.size() -1;
            }
        }

        QWidget *widget = m_childrenList.at(m_mark);
        Q_ASSERT_X(widget,"FocusManager", "widget is invalid");
        widget->activateWindow();
        widget->setFocus();
        if (widget && !widget->isEnabled() && widget != m_childrenList.last())
        {
            moveNextPrevFocus(next);
        }
    }

    m_markHash[m_curObject] = m_mark;
}

/*! 
 * 上下按键来移动焦点 
 */
void FocusManager::moveUpDownFocus(QLayout *layout, bool next)
{
    QGridLayout *gridLayout = qobject_cast<QGridLayout *>(layout);

    if (gridLayout)
    {
        QString pWidgetClsName = gridLayout->parentWidget()->metaObject()->className();
        
        //遍历layout找出第一个和左后一个有效的widget
        QWidget *firstWidget = NULL;
        QWidget *lastWidget = NULL;
        int curIndex = gridLayout->indexOf(Application::focusWidget());
        int curRow = 0;
        int curColumn = 0;
        for (int row = 0; row < gridLayout->rowCount(); ++row)
        {
            for (int col = 0; col < gridLayout->columnCount(); ++col)
            {
                QLayoutItem *tempItem = gridLayout->itemAtPosition(row, col);
                if (tempItem && tempItem->widget() && tempItem->widget()->focusPolicy() != Qt::NoFocus)
                {
                    if (!firstWidget)
                    {
                        firstWidget = tempItem->widget();
                    }
                    lastWidget = tempItem->widget();

                    //找到当前焦点的row， column
                    if (curIndex == gridLayout->indexOf(tempItem->widget()))
                    {
                        curRow = row;
                        curColumn = col;
                    }
                }
            }
        }

        int minMark = m_childrenList.indexOf(firstWidget);
        int maxMark = m_childrenList.indexOf(lastWidget);
        if (next)
        {
            QLayoutItem* nextItem = NULL;
            for (int row = curRow+1 ; row < gridLayout->rowCount(); ++row)
            {
                nextItem = gridLayout->itemAtPosition(row, curColumn);
                if (nextItem)
                {
                    break;
                }
            }

            if (nextItem && nextItem->widget())
            {
                if (m_childrenList.contains(nextItem->widget()))
                {
                    m_mark = m_childrenList.indexOf(nextItem->widget());
                }
            }
            else
            {
                m_mark = maxMark+1;
            }
            if (m_mark >= m_childrenList.size())
            {
                m_mark = 0;
            }
        }
        else
        {
            QLayoutItem* prevItem = NULL;
            for (int row = curRow-1 ; row >= 0; --row)
            {
                prevItem = gridLayout->itemAtPosition(row, curColumn);
                if (prevItem)
                {
                    break;
                }
            }
            
            if (prevItem && prevItem->widget())
            {
                m_mark = m_childrenList.indexOf(prevItem->widget());
            }
            else
            {
                m_mark = minMark-1;
            }
            if (m_mark < 0 )
            {
                m_mark = m_childrenList.size() -1;
            }
        }
        QWidget *widget = m_childrenList.at(m_mark);
        Q_ASSERT_X(widget,"FocusManager", "widget is invalid");
        widget->activateWindow();
        widget->setFocus();

        if (widget && !widget->isEnabled())
        {
            moveUpDownFocus(layout, next);
        }
    }
    m_markHash[m_curObject] = m_mark;
}

void FocusManager::setCurObj(QObject *obj)
{
    m_curObject = obj;
}

/*! 
 * 组件是否可编辑
 */
bool FocusManager::widgetEnableEdit()
{
    QWidget *focusWidget = Application::focusWidget();
    QLineEdit *lineEdit = NULL;
    QAbstractSpinBox *spinBox = NULL;
    QRadioButton *radioButton = NULL;
    QCheckBox *checkBox = NULL;
    if (focusWidget)
    {
        lineEdit = qobject_cast<QLineEdit *>(focusWidget);
        spinBox = qobject_cast<QAbstractSpinBox *>(focusWidget);
        radioButton = qobject_cast<QRadioButton *>(focusWidget);
        checkBox = qobject_cast<QCheckBox *>(focusWidget);
    }
    
    if (lineEdit)
    {
        emit showKeyboard(lineEdit);
        return true;
    }

    if (spinBox && spinBox->metaObject()->className() != QString("ColorSpinBox"))
    {
        if (spinBox->inputMethodHints() & (Qt::ImhDate | Qt::ImhTime))
        {
            return true;
        }
        emit showKeyboard(spinBox);
        return true;
    }

    if (radioButton)
    {
        radioButton->setChecked(true);
        return true;
    }

    if (checkBox)
    {
        checkBox->setChecked(!checkBox->isChecked());
        emit checkBox->clicked();
    }
    return false;
}

bool FocusManager::event(QEvent *e)
{
    if (e->type() == QEvent::KeyPress)
    {
        qDebug() << "#####FocusManager#####: key pressed";
        initWidgetList(m_curObject);
        if (m_childrenList.isEmpty())
        {
            return true;
        }
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(e);
        QWidget *widget = Application::focusWidget();
        if (!widget)
        {
            widget = m_childrenList.last();
        }
        QWidget *parent = widget->parentWidget();
        QLayout *layout = NULL;
        QString layoutType = "";
        if (parent)
        {
            layout = widget->parentWidget()->layout();
        }
        if (layout)
        {
            layoutType = layout->metaObject()->className();
        }
        QString className = widget->metaObject()->className();
        QString objName = m_curObject->metaObject()->className();
        QAbstractSpinBox *spinBox = qobject_cast<QAbstractSpinBox *>(widget);
        QComboBox *comboBox = qobject_cast<QComboBox *>(widget);

#if 0
        // ComboBox 子组件popupView 显示时候，焦点移动应关闭
        if (widget && !m_childrenList.contains(widget)) {
            QWidget * w = qobject_cast<QWidget *>(widget->parent());
            if (w && !m_childrenList.contains(w))
                if (w->isVisible())
                    w->setVisible(false);
            else {
                if (widget->isVisible())
                    widget->setVisible(false);
            }
        }
#endif

        if(keyEvent->key() == Qt::Key_Left || keyEvent->key() == Qt::Key_Right)
        {
            //判断spinbox是否接受左右按键
            if ( spinBox && qobject_cast<QObject *>(widget)->event(e))
            {
                return true;
            }

            //移动焦点
            switch(keyEvent->key())
            {
            case Qt::Key_Left:
                moveNextPrevFocus(false);
                break;

            case Qt::Key_Right:
                moveNextPrevFocus(true);
                break;
            }
            return true;
        }
        else if (keyEvent->key() == Qt::Key_Up || keyEvent->key() == Qt::Key_Down)
        {
#if 1
            //如果是QAbstractItemView,做特殊处理,如：setupView的左边标题切换
            QAbstractItemView *view = qobject_cast<QAbstractItemView *>(m_curObject);
            if (view)
            {
                QAbstractItemModel *model = view->model();
                QModelIndex curIndex = view->currentIndex();
                if (model)
                {
                    if (keyEvent->key() == Qt::Key_Down)
                    {
                        if(curIndex == model->index(model->rowCount()-1, curIndex.column()))
                        {
                            view->setCurrentIndex(model->index(0, 0));
                        }
                        else 
                        {
                            view->setCurrentIndex(model->index(curIndex.row()+1, curIndex.column()));
                        }
                    }
                    else if (keyEvent->key() == Qt::Key_Up)
                    {
                        if (curIndex == model->index(0, 0))
                        {
                            view->setCurrentIndex(model->index(model->rowCount()-1, curIndex.column()));
                        }
                        else 
                        {
                            view->setCurrentIndex(model->index(curIndex.row()-1, curIndex.column()));
                        }
                    }
                    return true;
                }
            }
#endif
            //移动焦点
            switch (keyEvent->key())
            {
                case Qt::Key_Up:
                    {
                        qDebug() << "#####FocusManager#####: key_up pressed";
                        //判断组件所在Layout是否包含其他可获取焦点组件
                        if (layoutType == "QGridLayout" && !spinBox && !comboBox)
                        {
                            moveUpDownFocus(layout, false);
                        }
                        else
                        {
                            moveNextPrevFocus(false);
                        }
                    }
                    break;
                case Qt::Key_Down:
                    {
                        qDebug() << "#####FocusManager#####: key_down pressed";
                        //判断组件所在Layout是否包含其他可获取焦点组件
                        if (layoutType == "QGridLayout" && !spinBox && !comboBox)
                        {
                            moveUpDownFocus(layout, true);
                        }
                        else
                        {
                            moveNextPrevFocus(true);
                        }
                    }
                    break;
            }
            return true;
        }
        else
        {
            if (widget && widget->hasFocus()/* && m_childrenList.contains(widget)*/)
            {
                qobject_cast<QObject *>(widget)->event(e);

                if (keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return)
                {
                    widgetEnableEdit();
                }
                return true;
            }
        }
        return true;
    }
    return m_curObject->event(e);
}

void FocusManager::resetFocus(QObject *obj)
{
    setCurObj(obj);
    initWidgetList(obj);
    m_mark = 0;
    QWidget *widget = m_childrenList.at(m_mark);
    Q_ASSERT_X(widget,"FocusManager", "widget is invalid");
    widget->activateWindow();
    widget->setFocus();
}
/*********************************************************************************************************
** End of file
*********************************************************************************************************/
