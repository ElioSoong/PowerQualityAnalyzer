/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  FocusManager.h
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
#ifndef FOCUSMANAGER_H
#define FOCUSMANAGER_H

#include <QObject>
#include <QWidget>
#include <QList>
#include <QLineEdit>
#include <QRadioButton>
#include <QHash>
#include <QKeyEvent>

#include "corelibrary_global.h"

class CORELIBRARY_EXPORT FocusManager : public QObject
{
    Q_OBJECT

private:
    FocusManager(QObject *parent = 0);
    ~FocusManager();

public:
    /*! 
     * 遍历子组件
     */
    QList<QWidget *>& traversingChildObj(QObject *obj);

    /*! 
     * 过滤子组件
     */
    bool filterComponent(QWidget *widget);

    /*! 
     * 初始化组件有效子组件列表
     */
    void initWidgetList(QObject *obj);

    void resetFocus(QObject *obj);

protected:
    bool event(QEvent *e);

public:
    static FocusManager* instance();
    void moveNextPrevFocus(bool next);
    void moveUpDownFocus(QLayout *layout, bool next);
    void setCurObj(QObject *obj);
    bool widgetEnableEdit();
    void resetFocus();

signals:
    void showKeyboard(QWidget *editTarget);

private:
    QList<QWidget *>     m_childrenList;        //!< 保存子对象
    QHash<QObject*, int> m_markHash;            //!< 标记当前有焦点的子组件
    int                  m_mark;
    static FocusManager *s_instance;            //!< 焦点管理器
    QObject             *m_curObject;
};

#endif // FOCUSMANAGER_H
/*********************************************************************************************************
** End of file
*********************************************************************************************************/
