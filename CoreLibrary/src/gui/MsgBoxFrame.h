/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  ComMsgBox.h
** Latest Version:             
** Latest modified Date:       
** Modified by:                
** Descriptions:               
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 Song Yunhua
** Created date:               2016/03/23 
** Descriptions:               
** 
*********************************************************************************************************/
#ifndef MSGBOXFRAME_H
#define MSGBOXFRAME_H

#include "includes.h"
#include "corelibrary_global.h"
#include <QDialog>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QKeyEvent>
#include <QIcon>
#include <QStack>

#pragma warning(disable : 4482)
#define WIDGET_WIDTH 800
#define WIDGET_HEIGHT 480

#define RESTART_ICON         QIcon("resources/icon/buttons/restart.png")
#define OK_ICON             QIcon("resources/icon/buttons/ok.png")
#define CANCEL_ICON         QIcon("resources/icon/buttons/cancel.png")
#define RENAME_ICON         QIcon("resources/icon/buttons/rename.png")
#define SAVE_ICON           QIcon("resources/icon/buttons/save.png")
#define EXIT_ICON           QIcon("resources/icon/buttons/exit.png")

/**
 *  @Class       ComMsgBox
 *  @brief       通用弹出框，只需将自定义窗口设置到CentralWidget，就可显示
 */
class CORELIBRARY_EXPORT MsgBoxFrame : public QDialog
{
    Q_OBJECT

public:
    enum StdButton 
    {
        // keep this in sync with QDialogButtonBox::StandardButton and QPlatformDialogHelper::StandardButton
        NoButton           = 0x00000000,
        Ok                 = 0x00000400,
        Save               = 0x00000800,
        SaveAll            = 0x00001000,
        Open               = 0x00002000,
        Yes                = 0x00004000,
        YesToAll           = 0x00008000,
        No                 = 0x00010000,
        NoToAll            = 0x00020000,
        Abort              = 0x00040000,
        Retry              = 0x00080000,
        Ignore             = 0x00100000,
        Close              = 0x00200000,
        Cancel             = 0x00400000,
        Discard            = 0x00800000,
        Help               = 0x01000000,
        Apply              = 0x02000000,
        Reset              = 0x04000000,
        RestoreDefaults    = 0x08000000,

        FirstButton        = Ok,                // internal
        LastButton         = RestoreDefaults,   // internal
    };
    typedef StdButton Button;  // obsolete

    Q_DECLARE_FLAGS(StdButtons, StdButton)
    
    enum VisibleAreaPos
    {
        Center  =   1,
        Bottom,
        FullWidget,
    };

    MsgBoxFrame(QWidget *parent = NULL, VisibleAreaPos visiblePos = Bottom);
    MsgBoxFrame(VisibleAreaPos visiblePos, QWidget *widget = NULL, 
        MsgBoxFrame::StdButtons buttons = MsgBoxFrame::Ok, QWidget *parent = NULL);
    ~MsgBoxFrame();

    /*! 
     * 自定义窗口添加到可视区域
     */
    virtual void setCentralWidget(QWidget *widget);

    /*! 
     * 添加按钮
     */
    QPushButton* addButton(MsgBoxFrame::StdButton button);
    QPushButton* addButton(const QString &text, const QIcon icon = QIcon());
    void addButton(QAbstractButton *button);
    void addButtons(MsgBoxFrame::StdButtons buttons);

    /*! 
     * 获取按钮
     */
    QPushButton* button(MsgBoxFrame::StdButton btn);
    QPushButton* button(const QString &name);

    /*! 
     * 
     */
    void setVisiblePos(VisibleAreaPos pos);
    VisibleAreaPos visiblePos();

    /*! 
     * 获取被点击的按钮
     */
    QAbstractButton* clickedButton();

    /*! 
     * 获取按钮容器
     */
    QDialogButtonBox* buttonBox();
	/*! 
	 *  删除按钮
	 */
	void clear();

    void setTargetEvent(QWidget *widget);

    /*! 
     * 通用的弹出窗口，自定义窗口以弹出窗的形式显示
     * @param[in]  widget: 显示的自定义窗口， buttons: 默认的按钮
     * @return     返回点击的按钮
     */
    static MsgBoxFrame::StdButton commonMsgBox(VisibleAreaPos pos = Bottom, QWidget *widget = NULL, MsgBoxFrame::StdButtons buttons = MsgBoxFrame::Ok);

    /*!
    * 记录上一层界面
    */
    static void pushWidget(QDialog *widget);
    static QDialog* popWidget();
    static QDialog* topWidget();
    static QStack<QDialog *>& widgetStack();
    static void closeAllMsgBox();

    bool thingsToBeDone();

public slots:
    void onBtnClicked(QAbstractButton *button);
    void closeByOk();
    void closeByCancel();


signals:
    void buttonClicked(QAbstractButton *);

protected:
    /*! 
     * 重载事件，自定义处理
     */
    void paintEvent(QPaintEvent *e) Q_DECL_OVERRIDE;
    void showEvent(QShowEvent *event) Q_DECL_OVERRIDE;
    void hideEvent(QHideEvent *event) Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
    void keyReleaseEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
    void focusInEvent(QFocusEvent *event) Q_DECL_OVERRIDE;
    bool event(QEvent *event) Q_DECL_OVERRIDE;
    bool eventFilter(QObject *obj, QEvent *event)Q_DECL_OVERRIDE;

    /*! 
     * 初始化属性
     */
    void initMsgBoxFrame();

    /*! 
     * 设置按钮默认图标属性
     */
    void setButtonAttr(QDialogButtonBox::StandardButton button);

    /*! 
     * 初始化布局，分为两种布局：可视区域为在 中心 或 底部
     */
    void initLayout();

    /*! 
     * 可视区域在中心的布局处理
     */
    void centralLayout();

    /*! 
     * 可视区域在底部的处理
     */
    void bottomLayout();

    /*! 
     * 铺满屏幕
     */
    void fullWidgetLayout();

    /*! 
     * 初始化可视区域，背景，自适应大小属性等
     */
    void initCentralWidget();

    QWidget* centralWidget();

    void setundeletableWidget(QWidget *widget);

    /*! 
     * 翻译
     */
    void retranslateStrings();

protected:
    static QStack<QDialog*>     s_widgetStack;

private:
    QDialogButtonBox    *m_buttonBox;
    QWidget             *m_centralWidget;
    VisibleAreaPos       m_visiblePos;
    QVBoxLayout         *m_centralLayout;
    QAbstractButton     *m_clickedBtn;
    QWidget             *m_undeletableWidget;
    QWidget             *m_eventTarget;

    bool                m_isDone;
    QMap<QByteArray, QPushButton*> m_textToBtn;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(MsgBoxFrame::StdButtons)
#endif // MSGBOXFRAME_H

/*********************************************************************************************************
** End of file
*********************************************************************************************************/