#pragma once
#include "IFieldWidget.h"
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QRadioButton>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QSpacerItem>
#include <QCheckBox>

class QHBoxLayout;
class QIntValidator;
class MyDoubleValidator;
class QFocusEvent;

Q_PLUGIN_METADATA(IID IFieldWidget_IID FILE "loadIFieldWidget.json")
class FIELDLIBRARY_EXPORT LineEdit : public QLineEdit, public IFieldWidget
{
    Q_OBJECT
        Q_PROPERTY(QString textFinished READ text WRITE setText NOTIFY editingTextFinished)
        Q_INTERFACES(IFieldWidget)
public:
    /**
    * Constructor
    */
    explicit LineEdit(QWidget* parent = NULL);
    explicit LineEdit(const QString& contents, QWidget* parent = NULL);
    ~LineEdit();

    void setText(const QString& text) Q_DECL_OVERRIDE
    {
        QLineEdit::setText(text);
        m_EditText = text;
    }

    QString text() const Q_DECL_OVERRIDE
    {
        return  QLineEdit::text();
    }

    void setEnabled(bool enabled) Q_DECL_OVERRIDE
    {
        QLineEdit::setEnabled(enabled);
    }

    bool isEnabled() const Q_DECL_OVERRIDE
    {
        return QLineEdit::isEnabled();
    }

    void setMax(const QVariant& max) Q_DECL_OVERRIDE;
    QVariant max() const Q_DECL_OVERRIDE
    {
        return this->m_IField->property("max");
    }

    void setMin(const QVariant& min) Q_DECL_OVERRIDE;
    QVariant min() const Q_DECL_OVERRIDE
    {
        return this->m_IField->property("min");
    }

    /*!
     * 编辑框的验证器为double类型时，限制字符串小数部分有效位数
     */
    void setDigitals(int digt);
    /*!
     * 编辑框的验证器为double类型时，限制字符串整数部分与小数部分总长度
     */
    void setWordWidth(int);
    Q_SIGNAL void editingTextFinished();
public slots:
    void slotEditTextFinished();

protected:
    void focusInEvent(QFocusEvent *e);
    void focusOutEvent(QFocusEvent *e);
    bool eventFilter(QObject *obj, QEvent *event);
    QString m_EditText;

private:
    QIntValidator*         m_intValidator;
    MyDoubleValidator*     m_doubleValidator;
};

class FIELDLIBRARY_EXPORT Label : public QLabel, public IFieldWidget
{
    Q_OBJECT
        Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
        Q_INTERFACES(IFieldWidget)
public:
    /**
    * Constructor
    */
    explicit Label(QWidget *parent = NULL);
    explicit Label(const QString &text, QWidget *parent = NULL);
    ~Label();

    Q_SIGNAL void textChanged();

    QString text() const Q_DECL_OVERRIDE;
    void setText(const QString& text) Q_DECL_OVERRIDE;
private:
    QString m_text;
};

class FIELDLIBRARY_EXPORT Switch : public QWidget, public IFieldWidget
{
    Q_OBJECT
        Q_INTERFACES(IFieldWidget)
        Q_PROPERTY(bool isChecked READ isChecked WRITE setChecked NOTIFY switchState)
public:
    explicit Switch(QWidget* parent = NULL);
    virtual ~Switch();
    void setImage(const QString& unCheckedImage, const QString& checkedImage);

    bool isChecked() const
    {
        return m_isChecked;
    }

    void setChecked(bool checked)
    {
        m_switch->setChecked(checked);
        m_isChecked = checked;
        emit switchState(m_isChecked);
    }
    void setDefault(const QVariant& value) Q_DECL_OVERRIDE
    {
        setChecked(value.toBool());
    }
    QVariant getDefault() const Q_DECL_OVERRIDE
    {
        return QVariant(isChecked());
    }
    QString text() const Q_DECL_OVERRIDE;
    void setText(const QString& text) Q_DECL_OVERRIDE;
    void setEnabled(bool enabled) Q_DECL_OVERRIDE
    {
        m_switch->setEnabled(enabled);
    }

    bool isEnabled() const Q_DECL_OVERRIDE
    {
        return m_switch->isEnabled();
    }

Q_SIGNALS:
    void switchState(bool checked);

protected:
    bool eventFilter(QObject *obj, QEvent *event);

private:
    QSpacerItem*    m_Spacer;
    QPushButton*    m_switch;
    QHBoxLayout*    m_mainLayout;
    bool            m_isChecked;
    QLabel*         m_label;
};

// class Switch;
class FIELDLIBRARY_EXPORT RadioButton : public QRadioButton, public IFieldWidget
{
    Q_OBJECT
        Q_INTERFACES(IFieldWidget)
        Q_PROPERTY(bool clicked MEMBER m_isClicked NOTIFY buttonClicked)
public:
    /**
    * Constructor
    */
    explicit RadioButton(QWidget *parent = Q_NULLPTR);
    explicit RadioButton(const QString &text, QWidget *parent = Q_NULLPTR);
    ~RadioButton();
    void setText(const QString &text) Q_DECL_OVERRIDE
    {
        QRadioButton::setText(text);
    }

    QString text() const Q_DECL_OVERRIDE
    {
        return QRadioButton::text();
    }
    void setDefault(const QVariant& def) Q_DECL_OVERRIDE;
    QVariant getDefault() const Q_DECL_OVERRIDE;
    void setEnabled(bool enabled) Q_DECL_OVERRIDE
    {
        QRadioButton::setEnabled(enabled);
    }

    bool isEnabled() const Q_DECL_OVERRIDE
    {
        return QRadioButton::isEnabled();
    }
    Q_SIGNAL void buttonClicked();
private:
    bool m_isClicked;
};

class FIELDLIBRARY_EXPORT Button : public QPushButton, public IFieldWidget
{
    Q_OBJECT
        Q_PROPERTY(bool isClicked MEMBER m_isClicked NOTIFY clickedChanged)
        Q_INTERFACES(IFieldWidget)
public:
    /**
    * Constructor
    */
    explicit Button(QWidget* parent = NULL);
    explicit Button(const QString& text, QWidget* parent = NULL);
    ~Button();

    void setText(const QString &text) Q_DECL_OVERRIDE
    {
        QPushButton::setText(text);
    }

    QString text() const Q_DECL_OVERRIDE
    {
        return QPushButton::text();
    }

    void setDefault(const QVariant& def) Q_DECL_OVERRIDE;
    QVariant getDefault() const Q_DECL_OVERRIDE;
    void setEnabled(bool enabled) Q_DECL_OVERRIDE
    {
        QPushButton::setEnabled(enabled);
    }

    bool isEnabled() const Q_DECL_OVERRIDE
    {
        return QPushButton::isEnabled();
    }
    Q_SIGNAL void clickedChanged(bool checked = false);

protected:
    void mouseReleaseEvent(QMouseEvent *e) Q_DECL_OVERRIDE;

private:
    bool        m_isClicked;//!<    QPushButton::isChecked
};
// __interface IFieldWidget;

class FIELDLIBRARY_EXPORT ComboBox : public QComboBox, public IFieldWidget
{
    Q_OBJECT
        Q_INTERFACES(IFieldWidget)
public:
    /**
    * Constructor
    */
    explicit ComboBox(QWidget* parent = NULL);
    ~ComboBox();

    void setMax(const QVariant& max) Q_DECL_OVERRIDE;
    QVariant max() const Q_DECL_OVERRIDE;

    void setText(const QString& text) Q_DECL_OVERRIDE
    {
        setCurrentText(text);
    }

    QString text() const Q_DECL_OVERRIDE
    {
        return currentText();
    }

    QVariantList choices() const Q_DECL_OVERRIDE;
    void setChoices(const QVariantList& choices) Q_DECL_OVERRIDE;
    void setEnabled(bool enabled) Q_DECL_OVERRIDE
    {
        QComboBox::setEnabled(enabled);
    }

    bool isEnabled() const Q_DECL_OVERRIDE
    {
        return QComboBox::isEnabled();
    }
};

class FIELDLIBRARY_EXPORT SpinBox : public QSpinBox, public IFieldWidget
{
    Q_OBJECT
        Q_INTERFACES(IFieldWidget)

public:
    /**
    * Constructor
    */
    explicit SpinBox(QWidget* parent = NULL);
    ~SpinBox();

    void setMax(const QVariant& max) Q_DECL_OVERRIDE;
    QVariant max() const Q_DECL_OVERRIDE;

    QVariant min() const Q_DECL_OVERRIDE;
    void setMin(const QVariant& max) Q_DECL_OVERRIDE;

    QVariant step() const Q_DECL_OVERRIDE;
    void setStep(const QVariant& step) Q_DECL_OVERRIDE;
    void setEnabled(bool enabled) Q_DECL_OVERRIDE
    {
        QSpinBox::setEnabled(enabled);
    }

    bool isEnabled() const Q_DECL_OVERRIDE
    {
        return QSpinBox::isEnabled();
    }
};

class FIELDLIBRARY_EXPORT DoubleSpinBox : public QDoubleSpinBox, public IFieldWidget
{
    Q_OBJECT
        Q_INTERFACES(IFieldWidget)
public:
    explicit DoubleSpinBox(QWidget* parent = NULL);
    ~DoubleSpinBox();

    void setMax(const QVariant& max) Q_DECL_OVERRIDE;
    QVariant max() const Q_DECL_OVERRIDE;

    QVariant min() const Q_DECL_OVERRIDE;
    void setMin(const QVariant& max) Q_DECL_OVERRIDE;

    QVariant step() const Q_DECL_OVERRIDE;
    void setStep(const QVariant& step) Q_DECL_OVERRIDE;
    void setEnabled(bool enabled) Q_DECL_OVERRIDE
    {
        QDoubleSpinBox::setEnabled(enabled);
    }

    bool isEnabled() const Q_DECL_OVERRIDE
    {
        return QDoubleSpinBox::isEnabled();
    }
};

class FIELDLIBRARY_EXPORT CheckBox : public QCheckBox, public IFieldWidget
{
    Q_OBJECT
        Q_INTERFACES(IFieldWidget)
public:
    explicit CheckBox(QWidget* parent = NULL);
    explicit CheckBox(const QString& text, QWidget* parent);
    void setText(const QString &text) Q_DECL_OVERRIDE
    {
        QCheckBox::setText(text);
    }

    QString text() const Q_DECL_OVERRIDE
    {
        return QCheckBox::text();
    }
    void setEnabled(bool enabled) Q_DECL_OVERRIDE
    {
        QCheckBox::setEnabled(enabled);
    }

    bool isEnabled() const Q_DECL_OVERRIDE
    {
        return QCheckBox::isEnabled();
    }

    void setDefault(const QVariant& value) Q_DECL_OVERRIDE
    {
        setChecked(value.toBool());
    }

    QVariant getDefault() const Q_DECL_OVERRIDE
    {
        return QVariant(isChecked());
    }
};