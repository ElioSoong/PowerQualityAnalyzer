/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  SoftInputPanel.cpp
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
#include "SoftInputPanel.h"

#include <QVariant>
#include <QtGui/QInputMethodEvent>
#include "Application.h"
#include <QSpinBox>
#include <QDoubleSpinBox>

DigitKey* SoftInputPanel::m_digitKeypad = NULL;
SoftKeyPad* SoftInputPanel::m_fullKeypad = NULL;

SoftInputPanel::SoftInputPanel(QWidget *parent)
    : QWidget(parent),
    m_editTarget(NULL),
    m_keypad(NULL)
{ 
    m_mainLayout = new QVBoxLayout();
    m_mainLayout->setMargin(0);
    setLayout(m_mainLayout);
}

SoftInputPanel::~SoftInputPanel()
{
    m_editTarget = NULL;
    m_keypad = NULL;
}

/*! 
 * 设置当前正在使用输入法编辑的控件（需要在本控件弹出前设置）
 * @param[in]  target 当前正在使用输入法编辑的控件
 */
void SoftInputPanel::setEditTarget(QWidget* target)
{
    if ((m_editTarget = target) == NULL)
        return;

    setEditMode(m_editTarget->inputMethodHints());
    qDebug() << this;

    QString text  = target->inputMethodQuery(Qt::ImSurroundingText).toString();
    int     hints = target->inputMethodQuery(Qt::ImHints).toInt();

    if (m_editTarget->inputMethodHints() & Qt::ImhDigitsOnly)
    {
        QString valStr;
        QDoubleSpinBox *spinBox = qobject_cast<QDoubleSpinBox*>(m_editTarget);
        if (spinBox)
        {
            int digits = spinBox->decimals();
            float max = spinBox->maximum();
            float val = text.toFloat();
            QString formate = QString("%.%1f").arg(digits);
            valStr.sprintf(formate.toLatin1().constData(), val);
            if (qIsNull(max - val))
            {
                valStr = spinBox->textFromValue(val);
                if (valStr.contains("."))
                    valStr.sprintf(formate.toLatin1().constData(), val);
            }
        }
        else
        {
            valStr = text;
        }
        
        static_cast<DigitKey*>(m_keypad)->setshow(valStr);
    }
    else
    {
        static_cast<SoftKeyPad*>(m_keypad)->setshow(text);
    }
    
    Q_UNUSED(hints);
}

/*! 
 * 设置按键面板的显示/编辑模式
 * @param[in]  mode 按键面板的显示/编辑模式
 */
void SoftInputPanel::setEditMode(Qt::InputMethodHints mode)
{
    // 按键面板分为数字面板和全键盘，可在此处进行切换
    if (mode == Qt::ImhDigitsOnly)
    {
        if (!m_digitKeypad)
        {
            m_digitKeypad = new DigitKey(this);
            Q_ASSERT(m_digitKeypad);

            connect(m_digitKeypad, SIGNAL(sigOkPressSignal()), this, SLOT(on_keypad_ok()));
            connect(m_digitKeypad, SIGNAL(sigCancelPressSignal()), this, SLOT(on_keypad_cancel()));
        }
        m_keypad = m_digitKeypad;
    }
    else
    {
        if (!m_fullKeypad)
        {
            m_fullKeypad = new SoftKeyPad(this);
            Q_ASSERT(m_fullKeypad);

            connect(m_fullKeypad, SIGNAL(sigOkPressSignal()), this, SLOT(on_keypad_ok()));
            connect(m_fullKeypad, SIGNAL(sigCancelPressSignal()), this, SLOT(on_keypad_cancel()));
        }

        m_keypad = m_fullKeypad;
        setMinimumSize(m_keypad->minimumSize());
    }

    int itemCount = m_mainLayout->count();
    while(itemCount)
    {
        m_mainLayout->takeAt(--itemCount);
    }
    m_mainLayout->addWidget(m_keypad);
}

/*! 
 * [slot] 按键面板按下OK键或者ENTER键
 */
void SoftInputPanel::on_keypad_ok()
{
    QString text = "";
    if (m_editTarget->inputMethodHints() & Qt::ImhDigitsOnly)
    {
        text = static_cast<DigitKey*>(m_keypad)->getInputStr();
    }
    else
    {
        text = static_cast<SoftKeyPad*>(m_keypad)->getInputStr();
    }
    if (text.isEmpty())
    {
        emit dataInputed();
        return;
    }

    if (m_editTarget)
    {
        m_editTarget->setAttribute(Qt::WA_InputMethodEnabled);
        QDoubleSpinBox *spinBox = qobject_cast<QDoubleSpinBox*>(m_editTarget);
        QLineEdit *lineEdit = qobject_cast<QLineEdit *>(m_editTarget);     
        if (spinBox)
        {
            float value = text.toFloat();
            float maxVal = spinBox->maximum();
            float minVal = spinBox->minimum();

            if (text == "N/A")
            {
                float realVal = spinBox->valueFromText(text);
                text = QString::number(realVal);
            }
            if (value > maxVal)
            {
                text = QString::number(maxVal);
            }
            if (value < minVal)
            {
                text = QString::number(minVal);
            }
        }
        if (lineEdit)
        {
            lineEdit->clear();
        }
        QInputMethodEvent e;
        e.setCommitString(text, 0, 0);
        ((QObject*)m_editTarget)->event(&e);
    }
    emit dataInputed();
}

/*! 
 * [slot] 按键面板按下CANCEL键或者ESC键
 */
void SoftInputPanel::on_keypad_cancel()
{
#if 0
    QString text = static_cast<SoftKeyPad*>(m_keypad)->getInputStr();
    if (!text.isEmpty())
    {
        // TODO: 此处可以弹框提示是否放弃编辑
    }
#endif
    emit inputCancelled();
}
/*********************************************************************************************************
** End of file
*********************************************************************************************************/

