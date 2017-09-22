/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  DigitKey.cpp 
** Latest Version:             V1.0.0
** Latest modified Date:       2015/08/04
** Modified by:                
** Descriptions:               数字键盘控件
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 Zhang Wenbo
** Created date:               2015/08/05
** Descriptions:               
** 
*********************************************************************************************************/
#include "DigitKey.h"
#include <QKeyEvent>


DigitKey::DigitKey(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::digitKey)
    , groupDigit(new QButtonGroup)
    , m_timer (new QTimer())
{
    ui->setupUi(this);
    groupDigit->addButton(ui->pushButton_1, 1);
    groupDigit->addButton(ui->pushButton_2, 2);
    groupDigit->addButton(ui->pushButton_3, 3);
    groupDigit->addButton(ui->pushButton_4, 4);
    groupDigit->addButton(ui->pushButton_5, 5);
    groupDigit->addButton(ui->pushButton_6, 6);
    groupDigit->addButton(ui->pushButton_7, 7);
    groupDigit->addButton(ui->pushButton_8, 8);
    groupDigit->addButton(ui->pushButton_9, 9);
    groupDigit->addButton(ui->pushButton_10, 10);
    groupDigit->addButton(ui->pushButton_11, 11);
    groupDigit->addButton(ui->pushButton_12, 12);
    groupDigit->addButton(ui->pushButton_13, 13);
    groupDigit->addButton(ui->pushButton_14, 14);
    groupDigit->addButton(ui->pushButton_15, 15);

    this->setFocusPolicy(Qt::NoFocus);
    setDefaultStyle();

    connect(groupDigit, SIGNAL(buttonClicked(int)), this, SLOT(onGroupDigitClicked(int)));
    connect(ui->pushButton_ok, SIGNAL(clicked()), this, SIGNAL(sigOkPressSignal()));
    connect(ui->pushButton_Cancle, SIGNAL(pressed()), this, SLOT(onCancleButtonPressed()));
    connect(ui->pushButton_Cancle, SIGNAL(released()), this, SLOT(onCancleButtonReleased()));
    connect(ui->lineEdit, SIGNAL(cursorPositionChanged(int, int)), this, SLOT(onCursorPositionChange(int, int)));
    ui->pushButton_Cancle->installEventFilter(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(onButtonLongPressed()));
    ui->lineEdit->installEventFilter(this);
}

QString DigitKey::getInputStr()
{
    m_inputStr = ui->lineEdit->text();
    return m_inputStr;
};

void DigitKey::onGroupDigitClicked(int id)
{
    //科学计数法显示的时候
    QString text = ui->lineEdit->text();
    int mark = text.contains("+") ? text.indexOf("+") : text.indexOf("-");
    int size = text.size();

    // 不能超过9个字符
    if (size >= 9)
        return;

    if (mark != -1 && mark == size-2 && text.at(size - 1) == '0')
    {
        text.remove("0");
        ui->lineEdit->setText(text);
    }

    QString str = groupDigit->button(id)->text();
    QString preText = ui->lineEdit->text();

    if (str == "m")
    {
        str = QString::number(ui->lineEdit->text().toDouble() * 0.001);
    }
    else if (str == "k")
    {
        str = QString::number(ui->lineEdit->text().toDouble() * 1000);
    }
    else if (str == "N/A")
    {
        str = "N/A";
    }
    else
    {
        if (preText == "N/A")
            ui->lineEdit->clear();
        ui->lineEdit->insert(str);
        str = ui->lineEdit->text();
    }

    // 小数点位数精度判断超限处理
    if (m_inputStr.contains("."))
    {
        int pos = m_inputStr.indexOf(".");
        int digits = m_inputStr.length() - (pos + 1);
        if (str.contains("."))
        {
            int curPos = str.indexOf(".");
            int curDigits = str.length() - (curPos + 1);
            if (curDigits > digits && ui->lineEdit->selectedText().isEmpty())
            {
                ui->lineEdit->setText(preText);
                return;
            }
                
        }
    }

    //if (str.contains("e"))
    //{
    //    ui->lineEdit->setMaxLength(str.size() + 1);
    //}

    ui->lineEdit->setText(str);
}

#define COMMON_STYLE_SHEET "background-color: rgb(239,246,255);color: rgb(0, 0, 0);border-width: 1px; font-family:'Microsoft YaHei';font-size:15px;"
void DigitKey::setDefaultStyle()
{
    setStyleSheet("QWidget{background-color: rgb(231, 238, 246);}"\
        "QLineEdit{background-color: rgb(255, 255, 255);font-family:'Microsoft YaHei';font-size:12px;}"\
        "QPushButton{background-color: qlineargradient(x1: 0, y1: 0,"\
        " x2: 0, y2: 1, stop: 0 rgb(241, 250, 255), stop: 0.13 "\
        "rgb(211, 238, 255), stop: 0.14 rgb(182, 227, 255), stop: 1 "\
        "rgb(211, 248, 255));"\
        "border-width: 1px; border-style: solid; border-color: rgb(168, 177, 186);"\
        "border-radius: 4px; font-family:'Microsoft YaHei';font-size:12px;}"\
        "QPushButton:pressed {background-color: qlineargradient(x1: 0, y1: 0,"\
        " x2: 0, y2: 1, stop: 0 rgb(236, 251, 254), stop: 0.13 "\
        "rgb(187, 241, 253), stop: 0.14 rgb(139, 234, 254), stop: 1 "\
        "rgb(156, 251, 255));"\
        "border-width: 1px; border-style: solid; border-color: rgb(168, 177, 186);"\
        "border-radius: 4px; font-family:'Microsoft YaHei';font-size:12px;}"\
        "#ButtonCancel{background-color: qlineargradient(x1: 0, y1: 0,"\
        " x2: 0, y2: 1, stop: 0 rgb(159, 222, 255), stop: 0.13 "\
        "rgb(20, 158, 249), stop: 0.14 rgb(0, 143, 248), stop: 1 "\
        "rgb(5, 117, 253));color: rgb(255, 255, 255);"\
        "border-width: 1px; border-style: solid; border-color: rgb(0, 7, 132);"\
        "border-radius: 6px; font-family:'Microsoft YaHei';font-size:12px;}"\
        "#ButtonCancel:pressed {background-color: qlineargradient(x1: 0, y1: 0,"\
        " x2: 0, y2: 1, stop: 0 rgb(255, 255, 255), stop: 0.13 "\
        "rgb(113, 212, 254), stop: 0.14 rgb(58, 197, 255), stop: 1 "\
        "rgb(55, 250, 254));color: rgb(255, 255, 255);"\
        "border-width: 1px; border-style: solid; border-color: rgb(0, 7, 132);"\
        "border-radius: 6px; font-family:'Microsoft YaHei';font-size:12px;}"\
        "#ButtonOK{background-color: qlineargradient(x1: 0, y1: 0,"\
        " x2: 0, y2: 1, stop: 0 rgb(159, 222, 255), stop: 0.13 "\
        "rgb(20, 158, 249), stop: 0.14 rgb(0, 143, 248), stop: 1 "\
        "rgb(5, 117, 253));;color: rgb(255, 255, 255);"\
        "border-width: 1px; border-style: solid; border-color: rgb(0, 7, 132);"\
        "border-radius: 6px; font-family:'Microsoft YaHei';font-size:12px;}"\
        "#ButtonOK:pressed {background-color: qlineargradient(x1: 0, y1: 0,"\
        " x2: 0, y2: 1, stop: 0 rgb(255, 255, 255), stop: 0.13 "\
        "rgb(113, 212, 254), stop: 0.14 rgb(58, 197, 255), stop: 1 "\
        "rgb(55, 250, 254));color: rgb(255, 255, 255);"\
        "border-width: 1px; border-style: solid; border-color: rgb(0, 7, 132);"\
        "border-radius: 6px; font-family:'Microsoft YaHei';font-size:12px;}");

    for (int i = 1; i <= 15; ++i) {
        groupDigit->button(i)->setStyleSheet(COMMON_STYLE_SHEET);
    }
    ui->pushButton_ok->setStyleSheet(COMMON_STYLE_SHEET);
    ui->pushButton_Cancle->setStyleSheet(COMMON_STYLE_SHEET);
}


DigitKey::~DigitKey()
{
    delete ui;
    delete m_timer;
}

void DigitKey::setshow(QString str)
{
    ui->lineEdit->clear();
    ui->lineEdit->setText(str);
    m_inputStr = str;
}

void DigitKey::onCancleButtonPressed()
{
    QString selectStr = ui->lineEdit->selectedText();
    QString text = ui->lineEdit->text();
    int mark = text.contains("+") ? text.indexOf("+") : text.indexOf("-");
    int size = text.size();
    int cursorPos = ui->lineEdit->cursorPosition();

    if (ui->lineEdit->text() == "N/A")
        ui->lineEdit->clear();
    QKeyEvent deletePress(QEvent::KeyPress, Qt::Key_Backspace, Qt::NoModifier);
    QCoreApplication::sendEvent(ui->lineEdit, &deletePress);
    
    if (mark != -1 && mark == size-2)
    {
        if(text.at(size - 1) == '0')
        {
            text = text.contains("+") ? 
                ui->lineEdit->text().remove("e+") : ui->lineEdit->text().remove("e-");
        }
        else
        {
            text = ui->lineEdit->text().append("0");
        }
        ui->lineEdit->setText(text);
    }

    if (cursorPos == mark + 1)
    {
        text = ui->lineEdit->text().remove(text.indexOf("e"), size);
        ui->lineEdit->setText(text);
    }

    if (!selectStr.isEmpty())
    {
        ui->lineEdit->activateWindow();
        ui->lineEdit->setFocus();
        ui->pushButton_Cancle->activateWindow();
        ui->pushButton_Cancle->setFocus();
    }
}

void DigitKey::onCancleButtonReleased()
{
    QKeyEvent deleteRelease(QEvent::KeyRelease, Qt::Key_Backspace, Qt::NoModifier);
    QCoreApplication::sendEvent(ui->lineEdit, &deleteRelease);
}

void DigitKey::onCursorPositionChange(int oldPos, int newPos)
{
    int mark = ui->lineEdit->text().indexOf("+");
    if (newPos == mark)
    {
        ui->lineEdit->setCursorPosition(oldPos);
    }
}
/*********************************************************************************************************
** End of file
*********************************************************************************************************/
