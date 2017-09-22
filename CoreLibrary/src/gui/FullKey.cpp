/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  FullKey.cpp 
** Latest Version:             V1.0.0
** Latest modified Date:       2015/10/28
** Modified by:                Song Yunhua
** Descriptions:               全键盘控件
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 Zhang Wenbo
** Created date:               2015/08/04
** Descriptions:               
** 
*********************************************************************************************************/
#include "includes.h"
#include "FullKey.h"
#include "KeyButton.h"


FullKey::FullKey(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::fullKey)
{
    ui->setupUi(this);
    ui->key_backspace->setIcon(QIcon("resources/icon/buttons/backspace.png"));
    ui->key_backspace->setIconSize(QSize(32, 32));
//     ui->key_backspace->setStyleSheet("#key_backspace{border-image: url(resources/icon/buttons/backspace.png);}\
//                                      #key_backspace:pressed{border-image: url(resources/icon/buttons/backspacepressed.png);}");
    setFocusProxy(ui->key_backspace);
    
    setAutoFillBackground(true);
    QPalette p = palette();
    p.setColor(QPalette::Background, QColor(241,247,255));
    setPalette(p);

    connect(ui->func_4, SIGNAL(clicked()), this, SLOT(onSymbolClicked()));
    connect(ui->key_a, SIGNAL(keyClicked(const QString &)), this, SLOT(onCharBtnClicked(const QString &)));
    connect(ui->key_b, SIGNAL(keyClicked(const QString &)), this, SLOT(onCharBtnClicked(const QString &)));
    connect(ui->key_c, SIGNAL(keyClicked(const QString &)), this, SLOT(onCharBtnClicked(const QString &)));
    connect(ui->key_d, SIGNAL(keyClicked(const QString &)), this, SLOT(onCharBtnClicked(const QString &)));
    connect(ui->key_e, SIGNAL(keyClicked(const QString &)), this, SLOT(onCharBtnClicked(const QString &)));
    connect(ui->key_f, SIGNAL(keyClicked(const QString &)), this, SLOT(onCharBtnClicked(const QString &)));
    connect(ui->key_g, SIGNAL(keyClicked(const QString &)), this, SLOT(onCharBtnClicked(const QString &)));
    connect(ui->key_h, SIGNAL(keyClicked(const QString &)), this, SLOT(onCharBtnClicked(const QString &)));
    connect(ui->key_i, SIGNAL(keyClicked(const QString &)), this, SLOT(onCharBtnClicked(const QString &)));
    connect(ui->key_j, SIGNAL(keyClicked(const QString &)), this, SLOT(onCharBtnClicked(const QString &)));
    connect(ui->key_k, SIGNAL(keyClicked(const QString &)), this, SLOT(onCharBtnClicked(const QString &)));
    connect(ui->key_l, SIGNAL(keyClicked(const QString &)), this, SLOT(onCharBtnClicked(const QString &)));
    connect(ui->key_m, SIGNAL(keyClicked(const QString &)), this, SLOT(onCharBtnClicked(const QString &)));
    connect(ui->key_n, SIGNAL(keyClicked(const QString &)), this, SLOT(onCharBtnClicked(const QString &)));
    connect(ui->key_o, SIGNAL(keyClicked(const QString &)), this, SLOT(onCharBtnClicked(const QString &)));
    connect(ui->key_p, SIGNAL(keyClicked(const QString &)), this, SLOT(onCharBtnClicked(const QString &)));
    connect(ui->key_q, SIGNAL(keyClicked(const QString &)), this, SLOT(onCharBtnClicked(const QString &)));
    connect(ui->key_r, SIGNAL(keyClicked(const QString &)), this, SLOT(onCharBtnClicked(const QString &)));
    connect(ui->key_s, SIGNAL(keyClicked(const QString &)), this, SLOT(onCharBtnClicked(const QString &)));
    connect(ui->key_t, SIGNAL(keyClicked(const QString &)), this, SLOT(onCharBtnClicked(const QString &)));
    connect(ui->key_u, SIGNAL(keyClicked(const QString &)), this, SLOT(onCharBtnClicked(const QString &)));
    connect(ui->key_v, SIGNAL(keyClicked(const QString &)), this, SLOT(onCharBtnClicked(const QString &)));
    connect(ui->key_w, SIGNAL(keyClicked(const QString &)), this, SLOT(onCharBtnClicked(const QString &)));
    connect(ui->key_x, SIGNAL(keyClicked(const QString &)), this, SLOT(onCharBtnClicked(const QString &)));
    connect(ui->key_y, SIGNAL(keyClicked(const QString &)), this, SLOT(onCharBtnClicked(const QString &)));
    connect(ui->key_z, SIGNAL(keyClicked(const QString &)), this, SLOT(onCharBtnClicked(const QString &)));
    connect(ui->digit_0, SIGNAL(keyClicked(const QString &)), this, SLOT(onCharBtnClicked(const QString &)));
    connect(ui->digit_1, SIGNAL(keyClicked(const QString &)), this, SLOT(onCharBtnClicked(const QString &)));
    connect(ui->digit_2, SIGNAL(keyClicked(const QString &)), this, SLOT(onCharBtnClicked(const QString &)));
    connect(ui->digit_3, SIGNAL(keyClicked(const QString &)), this, SLOT(onCharBtnClicked(const QString &)));
    connect(ui->digit_4, SIGNAL(keyClicked(const QString &)), this, SLOT(onCharBtnClicked(const QString &)));
    connect(ui->digit_5, SIGNAL(keyClicked(const QString &)), this, SLOT(onCharBtnClicked(const QString &)));
    connect(ui->digit_6, SIGNAL(keyClicked(const QString &)), this, SLOT(onCharBtnClicked(const QString &)));
    connect(ui->digit_7, SIGNAL(keyClicked(const QString &)), this, SLOT(onCharBtnClicked(const QString &)));
    connect(ui->digit_8, SIGNAL(keyClicked(const QString &)), this, SLOT(onCharBtnClicked(const QString &)));
    connect(ui->digit_9, SIGNAL(keyClicked(const QString &)), this, SLOT(onCharBtnClicked(const QString &)));
    connect(ui->symbol_1, SIGNAL(keyClicked(const QString &)), this, SLOT(onCharBtnClicked(const QString &)));
    connect(ui->symbol_2, SIGNAL(keyClicked(const QString &)), this, SLOT(onCharBtnClicked(const QString &)));
    connect(ui->symbol_3, SIGNAL(keyClicked(const QString &)), this, SLOT(onCharBtnClicked(const QString &)));
    connect(ui->symbol_4, SIGNAL(keyClicked(const QString &)), this, SLOT(onCharBtnClicked(const QString &)));
    connect(ui->symbol_5, SIGNAL(keyClicked(const QString &)), this, SLOT(onCharBtnClicked(const QString &)));
    connect(ui->symbol_6, SIGNAL(keyClicked(const QString &)), this, SLOT(onCharBtnClicked(const QString &)));
    connect(ui->key_backspace, SIGNAL(clicked()), this, SLOT(onBacksapceClicked()));
    connect(ui->func_3, SIGNAL(keyClicked(const QString &)), this, SLOT(onCharBtnClicked(const QString &)));
    connect(ui->func_5, SIGNAL(clicked()), this, SLOT(onEnterClicked()));
}


FullKey::~FullKey()
{
    delete ui;
}

static int m_flag = 0;
void FullKey::onSymbolClicked()
{
    if (m_flag == 0) 
    {
        ui->key_a->setText("\"");
        ui->key_b->setText("(");
        ui->key_c->setText("{");
        ui->key_d->setText(".");
        ui->key_e->setText("@");
        ui->key_f->setText(":");
        ui->key_g->setText("\?");
        ui->key_h->setText("=");
        ui->key_i->setText("&&");
        ui->key_j->setText("-");
        ui->key_k->setText("/");
        ui->key_l->setText("\\");
        ui->key_m->setText("·");
        ui->key_n->setText(")");
        ui->key_o->setText("*");
        ui->key_p->setText("℃");
        ui->key_q->setText("~");
        ui->key_r->setText("#");
        ui->key_s->setText(",");
        ui->key_t->setText("$");
        ui->key_u->setText("^");
        ui->key_v->setText("}");
        ui->key_w->setText("!");
        ui->key_x->setText(">");
        ui->key_y->setText("%");
        ui->key_z->setText("<");
        ui->symbol_1->setText("|");
        ui->symbol_2->setText("。");
        ui->symbol_3->setText("×");
        ui->symbol_4->setText("÷");
        m_flag = 1;
    }
    else 
    {
        ui->key_a->setText("A");
        ui->key_b->setText("B");
        ui->key_c->setText("C");
        ui->key_d->setText("D");
        ui->key_e->setText("E");
        ui->key_f->setText("F");
        ui->key_g->setText("G");
        ui->key_h->setText("H");
        ui->key_i->setText("I");
        ui->key_j->setText("J");
        ui->key_k->setText("K");
        ui->key_l->setText("L");
        ui->key_m->setText("M");
        ui->key_n->setText("N");
        ui->key_o->setText("O");
        ui->key_p->setText("P");
        ui->key_q->setText("Q");
        ui->key_r->setText("R");
        ui->key_s->setText("S");
        ui->key_t->setText("T");
        ui->key_u->setText("U");
        ui->key_v->setText("V");
        ui->key_w->setText("W");
        ui->key_x->setText("X");
        ui->key_y->setText("Y");
        ui->key_z->setText("Z");
        ui->symbol_1->setText("：");
        ui->symbol_2->setText("，");
        ui->symbol_3->setText("。");
        ui->symbol_4->setText("；");
        m_flag = 0;
    }
}

void FullKey::onCharBtnClicked(const QString &text)
{
    if (text == "SPACE")
    {
        m_labelText.append(" ");
    }
    else
    {
        m_labelText.append(text);
    }
    ui->lineEdit->setText(m_labelText);
}

void FullKey::onBacksapceClicked()
{
    m_labelText.remove(m_labelText.count()-1, 1);
    ui->lineEdit->setText(m_labelText);
}

void FullKey::onEnterClicked()
{
    QString text = ui->lineEdit->text();
    emit enterClicked(text);
}

/*********************************************************************************************************
** End of file
*********************************************************************************************************/