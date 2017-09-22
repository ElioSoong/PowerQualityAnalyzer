#ifndef __MYDOUBLEVALIDATOR_H
#define __MYDOUBLEVALIDATOR_H

#include <QDoubleValidator>
#define EPSINON 0.0000000000000001

class MyDoubleValidator : public QDoubleValidator
{
    Q_OBJECT
public:
    explicit MyDoubleValidator(QObject * parent = Q_NULLPTR);
    MyDoubleValidator(double bottom, double top, int decimals, QObject *parent = Q_NULLPTR);
    ~MyDoubleValidator(){}
    void setWordWidth(int val){ m_wordWidth = val; }
    int wordWidth() const { return m_wordWidth; }
    QValidator::State validate(QString &, int &) const Q_DECL_OVERRIDE;

private:
    int m_decimals;     //!< 小数位数
    int m_wordWidth;    //!< 有效位数
};
#endif // __MYDOUBLEVALIDATOR_H