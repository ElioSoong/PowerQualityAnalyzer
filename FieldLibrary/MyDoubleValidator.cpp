#include "MyDoubleValidator.h"
#include <QRegularExpression>
#include <QDebug>

MyDoubleValidator::MyDoubleValidator(QObject * parent /* = Q_NULLPTR */)
    : MyDoubleValidator(-HUGE_VAL, HUGE_VAL, 16, parent)
{
}

MyDoubleValidator::MyDoubleValidator(double bottom, double top, int decimals, QObject *parent /* = Q_NULLPTR */)
    : QDoubleValidator(bottom, top, decimals, parent)
    , m_decimals(decimals)
    , m_wordWidth(100)
{
    setDecimals(m_decimals);
}

QValidator::State MyDoubleValidator::validate(QString &str, int &cursorPos) const
{
    if (str.isEmpty())
    {
        return QValidator::Intermediate;
    }

    bool cOK = false;
    double val = str.toDouble(&cOK);

    if (!cOK)
    {
        return QValidator::Invalid;
    }

    int dotPos = str.indexOf(".");
    if (dotPos > 0)
    {
        if (str.length() > m_wordWidth+1)
        {
            return QValidator::Invalid;
        }
        if (str.right(str.length() - dotPos - 1).length() > decimals())
        {
            return QValidator::Invalid;
        }
        else if (str.length() == dotPos + 1)
        {
            return QValidator::Acceptable;
        }
    }

    if (cursorPos == 1 && (str.toInt() == int(bottom()) || str.toInt() == int(top())))
    {
        return QValidator::Acceptable;
    }

    if (abs(val - top()) <= EPSINON && abs(val - bottom()) > EPSINON)
    {
        return QValidator::Acceptable;
    }
    else if (abs(val - top()) > EPSINON && val - bottom() <= EPSINON)
    {
        return QValidator::Acceptable;
    }
    else if (val - top() < -EPSINON && val - bottom() > EPSINON)
    {
        return QValidator::Acceptable;
    }
    else if (val < EPSINON && val > -EPSINON)
    {
        return QValidator::Acceptable;
    }

    return QValidator::Invalid;
}
