#include "ComWidget.h"
#include "MyDoubleValidator.h"
#include "IntegerField.h"
#include "RealField.h"
#include <QHBoxLayout>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QMouseEvent>
#include <QIntValidator>
#include <QFocusEvent>
#include <QToolTip>
/**
* Constructor
*/
LineEdit::LineEdit(QWidget* parent)
    : LineEdit("", parent)
{
}

LineEdit::LineEdit(const QString& contents, QWidget* parent)
    : QLineEdit(contents, parent)
    , IFieldWidget(this, "textFinished")
    , m_intValidator(nullptr)
    , m_doubleValidator(nullptr)
    , m_EditText("")
{
    QObject::connect(this, &QLineEdit::editingFinished, this, &LineEdit::editingTextFinished);
    QObject::connect(this, &LineEdit::editingTextFinished, this, &LineEdit::slotEditTextFinished);
    this->installEventFilter(this);
}

LineEdit::~LineEdit()
{
    if (m_intValidator)
    {
        delete m_intValidator; m_intValidator = NULL;
    }
    if (m_doubleValidator)
    {
        delete m_doubleValidator; m_doubleValidator = NULL;
    }
}

void LineEdit::setMax(const QVariant& max)
{
    if (this->validator())
    {
        return;
    }
    if (this->m_IField->property("typeName").toString() == IntegerField().typeName()\
        || this->m_IField->property("typeName").toString() == LongIntegerField().typeName())
    {
        m_intValidator = new QIntValidator();
        m_intValidator->setTop(max.toInt());
        m_intValidator->setBottom(min().toInt());
        if (m_intValidator->bottom() != m_intValidator->top())
        {
            this->setValidator(m_intValidator);
        }
        else
        {
            delete m_intValidator; m_intValidator = nullptr;
        }
    }
    else if (this->m_IField->property("typeName").toString() == RealField().typeName())
    {
        m_doubleValidator = new MyDoubleValidator();
        setDigitals(this->m_IField->property("digitals").toInt());
        setWordWidth(this->m_IField->property("wordWidth").toInt());
        m_doubleValidator->setTop(max.toDouble());
        m_doubleValidator->setBottom(min().toDouble());

        if (abs(m_doubleValidator->top() - m_doubleValidator->bottom()) > 0.0000000000000001)
        {
            this->setValidator(m_doubleValidator);
        }
        else
        {
            delete m_doubleValidator; m_doubleValidator = nullptr;
        }
    }
}

void LineEdit::setMin(const QVariant& min)
{
    if (this->validator())
    {
        return;
    }
    if (this->m_IField->property("typeName").toString() == IntegerField().typeName()\
        || this->m_IField->property("typeName").toString() == LongIntegerField().typeName())
    {
        m_intValidator = new QIntValidator();
        m_intValidator->setBottom(min.toInt());
        m_intValidator->setTop(max().toInt());
        if (m_intValidator->bottom() != m_intValidator->top())
        {
            this->setValidator(m_intValidator);
        }
        else
        {
            delete m_intValidator; m_intValidator = nullptr;
        }
    }
    else if (this->m_IField->property("typeName").toString() == RealField().typeName())
    {
        m_doubleValidator = new MyDoubleValidator();
        setDigitals(this->m_IField->property("digitals").toInt()); // 默认值3，RealField构造赋值
        setWordWidth(this->m_IField->property("wordWidth").toInt());
        m_doubleValidator->setBottom(min.toDouble());
        m_doubleValidator->setTop(max().toDouble());

        if (abs(m_doubleValidator->top() - m_doubleValidator->bottom()) > 0.0000000000000001)
        {
            this->setValidator(m_doubleValidator);
        }
        else
        {
            delete m_doubleValidator; m_doubleValidator = nullptr;
        }
    }
}

void LineEdit::setDigitals(int digt)
{
    if (m_doubleValidator)
    {
        m_doubleValidator->setDecimals(digt);
    }
}

void LineEdit::setWordWidth(int width)
{
    if (m_doubleValidator)
    {
        m_doubleValidator->setWordWidth(width);
    }
}

void LineEdit::slotEditTextFinished()
{
    if (m_doubleValidator)
    {
        if (this->text().toDouble() - m_doubleValidator->bottom() < -0.0000000000000001)
        {
            this->setText(QString::number(m_doubleValidator->bottom(), 'f', this->m_IField->property("digitals").toInt()));
        }
    }
    else if (m_intValidator)
    {
        if (this->text().toInt() < m_intValidator->bottom())
        {

            this->setText(QString::number(m_intValidator->bottom()));
        }
    }

    QPalette p = QPalette();
    p.setColor(QPalette::Base, Qt::white);
    setPalette(p);
}

void LineEdit::focusInEvent(QFocusEvent *e)
{
    QPalette p = QPalette();
    p.setColor(QPalette::Base, Qt::gray);
    setPalette(p);
    return QLineEdit::focusInEvent(e);
}

void LineEdit::focusOutEvent(QFocusEvent *e)
{
    if (e->lostFocus())
    {
        if (m_doubleValidator)
        {
            if (this->text().toDouble() - m_doubleValidator->bottom() < -0.0000000000000001)
            {
                this->setText(QString::number(m_doubleValidator->bottom(), 'f', this->m_IField->property("digitals").toInt()));
            }
        }
        else if (m_intValidator)
        {
            if (this->text().toInt() < m_intValidator->bottom())
            {
                this->setText(QString::number(m_intValidator->bottom()));
            }
        }
    }
    return QLineEdit::focusOutEvent(e);
}

bool LineEdit::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::ToolTip)
    {
        QHelpEvent *helpEvent = static_cast<QHelpEvent *>(event);
        QToolTip::showText(helpEvent->globalPos(), this->text(), this);
    }
    return QWidget::eventFilter(obj, event);
}
//*******************************Label**************************************
/**
* Constructor
*/
Label::Label(QWidget* parent)
    : Label("", parent)
{
}

Label::Label(const QString& text, QWidget* parent)
    : QLabel(text, parent)
    , IFieldWidget(this, "text")
{
}

Label::~Label()
{
}

QString Label::text() const
{
    return m_text;
}

void Label::setText(const QString& text)
{
    if (text != m_text)
    {
        m_text = text;
        QLabel::setText(m_text);
        emit textChanged();
    }
}

//******************************Switch***************************************
Switch::Switch(QWidget* parent)
    : QWidget(parent)
    , IFieldWidget(this, "isChecked")
    , m_isChecked(false)
{
    m_mainLayout = new QHBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(10);

    m_switch = new QPushButton();
    m_switch->installEventFilter(this);
    m_switch->setCheckable(true);
    m_switch->setFixedWidth(43);
    m_switch->setFixedHeight(23);

    m_Spacer = new QSpacerItem(m_switch->width(), m_switch->height(), QSizePolicy::Expanding, QSizePolicy::Minimum);

    m_mainLayout->addWidget(m_switch);
    m_mainLayout->addItem(m_Spacer);

    m_mainLayout->setStretch(0, 0);
    m_mainLayout->setStretch(1, 1);
    setImage(":/Others/Off.png", ":/Others/On.png");

    m_label = new QLabel();
}

Switch::~Switch()
{
    delete m_mainLayout;
    m_mainLayout = NULL;
    m_Spacer = NULL;
    m_switch = NULL;
    if (text().isEmpty())
    {
        delete m_label;
    }
    m_label = NULL;
}

void Switch::setImage(const QString& unCheckedImage, const QString& checkedImage)
{
    if (!checkedImage.isEmpty() && !unCheckedImage.isEmpty())
    {
        QString style = QString("QPushButton{border-image:url(%1);}\
                                                                 QPushButton:checked{border-image:url(%2);}\
                                                                                                ").arg(unCheckedImage).arg(checkedImage);
        m_switch->setStyleSheet(style);
    }
}

void Switch::setText(const QString& text)
{
    if (!text.isEmpty())
    {
        m_label->setText(text);
        m_mainLayout->removeItem(m_Spacer);
        m_mainLayout->removeWidget(m_switch);
        m_mainLayout->addWidget(m_label);
        m_mainLayout->addWidget(m_switch);
        m_mainLayout->addSpacerItem(m_Spacer);
    }
}

QString Switch::text() const
{
    return m_label->text();
}

bool Switch::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonRelease)
    {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::LeftButton)
        {
            if (qobject_cast<QPushButton*>(obj) == m_switch && m_switch->isEnabled())
            {
                m_isChecked = !m_isChecked;
                emit switchState(!m_switch->isChecked());
            }
        }
    }

    return QWidget::eventFilter(obj, event);
}

/***********************************RadioButton*****************************************************/
RadioButton::RadioButton(QWidget *parent)
    : RadioButton("", parent)
{
}

RadioButton::RadioButton(const QString &text, QWidget *parent)
    : QRadioButton(text, parent)
    , IFieldWidget(this, "clicked")
{
    connect(this, SIGNAL(clicked()), SIGNAL(buttonClicked()));
}

RadioButton::~RadioButton()
{
}

QVariant RadioButton::getDefault() const
{
    return QVariant(this->isChecked());
}

void RadioButton::setDefault(const QVariant& def)
{
    this->setChecked(def.toBool());
}

//******************************Button***************************************
Button::Button(QWidget* parent)
    : Button("", parent)
{
}

Button::Button(const QString& text, QWidget* parent)
    : QPushButton(text, parent)
    , IFieldWidget(this, "isClicked")
    , m_isClicked(false)
{
}

Button::~Button()
{
}

QVariant Button::getDefault() const
{
    return QVariant(this->isDefault());
}

void Button::setDefault(const QVariant& def)
{
    QPushButton::setDefault(def.toBool());
}

void Button::mouseReleaseEvent(QMouseEvent *e)
{
    if (e->button() != Qt::LeftButton)
    {
        e->ignore();
        return;
    }

    if (isCheckable())
    {
        m_isClicked = !m_isClicked;
    }
    emit clickedChanged(m_isClicked);
    return QPushButton::mouseReleaseEvent(e);
}
//********************************ComboBox*************************************

ComboBox::ComboBox(QWidget* parent)
    : QComboBox(parent)
    , IFieldWidget(this, "currentText")
{
}

ComboBox::~ComboBox()
{
}

QVariant ComboBox::max() const
{
    return QVariant(this->maxVisibleItems());
}

void ComboBox::setMax(const QVariant& max)
{
    this->setMaxVisibleItems(max.toInt());
}

QVariantList ComboBox::choices() const
{
    QVariantList varlst;
    for (int index = 0; index < this->count(); ++index)
    {
        varlst.append(QVariant(this->itemText(index)));
    }
    return varlst;
}

void ComboBox::setChoices(const QVariantList& choices)
{
    blockSignals(true);
    this->clear();
    blockSignals(false);
    QStringList items;
    foreach(QVariant str, choices)
    {
        items << str.toString();
    }
    this->insertItems(0, items);
}
//********************************SpinBox*************************************

SpinBox::SpinBox(QWidget* parent)
    : QSpinBox(parent)
    , IFieldWidget(this, "value")
{
}

SpinBox::~SpinBox()
{
}

QVariant SpinBox::max() const
{
    return QVariant(this->maximum());
}

void SpinBox::setMax(const QVariant& max)
{
    this->setMaximum(max.toInt());
}

QVariant SpinBox::min() const
{
    return QVariant(this->minimum());
}

void SpinBox::setMin(const QVariant& min)
{
    this->setMinimum(min.toInt());
}

QVariant SpinBox::step() const
{
    return QVariant(this->singleStep());
}

void SpinBox::setStep(const QVariant& step)
{
    this->setSingleStep(step.toInt());
}

//********************************DoubleSpinBox*************************************

DoubleSpinBox::DoubleSpinBox(QWidget* parent)
    : QDoubleSpinBox(parent)
    , IFieldWidget(this, "value")
{
}

DoubleSpinBox::~DoubleSpinBox()
{
}

QVariant DoubleSpinBox::max() const
{
    return QVariant(this->maximum());
}

void DoubleSpinBox::setMax(const QVariant& max)
{
    this->setMaximum(max.toDouble());
}

QVariant DoubleSpinBox::min() const
{
    return QVariant(this->minimum());
}

void DoubleSpinBox::setMin(const QVariant& min)
{
    this->setMinimum(min.toDouble());
}

QVariant DoubleSpinBox::step() const
{
    return QVariant(this->singleStep());
}

void DoubleSpinBox::setStep(const QVariant& step)
{
    this->setSingleStep(step.toDouble());
}

CheckBox::CheckBox(QWidget* parent)
    : CheckBox("", parent)
{
}

CheckBox::CheckBox(const QString& text, QWidget* parent)
    : QCheckBox(text, parent)
    , IFieldWidget(this, "checked")
{
}