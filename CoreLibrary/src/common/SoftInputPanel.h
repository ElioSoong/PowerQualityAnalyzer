/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  SoftInputPanel.h
** Latest Version:             V1.0.0
** Latest modified Date:       2015/12/01
** Modified by:                
** Descriptions:               
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 
** Created date:               2015/12/01
** Descriptions:               软件输入面板（SIP）封装类，通过系统输入法接口编辑控件
** 
*********************************************************************************************************/
#ifndef SOFTINPUTPANEL_H
#define SOFTINPUTPANEL_H

#include "corelibrary_global.h"
#include <QtWidgets/QWidget>
#include "gui/DigitKey.h"
#include "common/softinput.h"

/**
 *  @class SoftInputPanel
 *  @brief 软件输入面板（SIP）封装类
 */
class CORELIBRARY_EXPORT SoftInputPanel : public QWidget
{
    Q_OBJECT

public:
    SoftInputPanel(QWidget *parent = NULL);
    ~SoftInputPanel();

    /*! 
     * 获取当前正在使用输入法编辑的控件（仅用于调试）
     * @return     当前正在使用输入法编辑的控件
     */
    inline QWidget* editTarget() const
    {
        return m_editTarget;
    }

    /*! 
     * 设置当前正在使用输入法编辑的控件（需要在本控件弹出前设置）
     * @param[in]  target 当前正在使用输入法编辑的控件
     */
    void setEditTarget(QWidget* target);

    /*! 
     * 设置按键面板的显示/编辑模式
     * @param[in]  mode 按键面板的显示/编辑模式
     */
    void setEditMode(Qt::InputMethodHints mode);

protected slots:
    /*! 
     * [slot] 按键面板按下OK键或者ENTER键
     */
    void on_keypad_ok();

    /*! 
     * [slot] 按键面板按下CANCEL键或者ESC键
     */
    void on_keypad_cancel();

signals:
    void dataInputed();
    void inputCancelled();

protected:
    QWidget* m_editTarget;                  //!< 当前正在使用输入法编辑的控件，例如QLineEdit
    QWidget* m_keypad;                      //!< 内部封装的按键面板对象，实际为当前控件的唯一子控件
    QVBoxLayout* m_mainLayout; 
    static DigitKey*    m_digitKeypad;
    static SoftKeyPad*  m_fullKeypad;
};

#endif // SOFTINPUTPANEL_H
/*********************************************************************************************************
** End of file
*********************************************************************************************************/
