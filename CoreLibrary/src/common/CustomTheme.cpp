#include "CustomTheme.h"
#include <QAbstractButton>
#include <QStyleFactory>
#include <QPainter>
#include <QStyleOption>
#include <QComboBox>
#include <QDebug>
//#include "StorageView.h"

CustomTheme::CustomTheme(void) :
QProxyStyle(QStyleFactory::create("windows"))
{
    setSrcProperty();
    setBtnBackground(QPixmap(m_btnUnSelectedPath));
    setBtnSelectedImage(QPixmap(m_btnSelectedPath));

    setSpinArrow(QPixmap(m_leftArrowPath));
    setRightSpinArrow(QPixmap(m_rightArrowPath));

    setcomboBoxArrow(QPixmap(m_comboBoxImagePath));

    setUncheckedCheckBox(QPixmap(m_uncheckedImagePath));
    setCheckedCheckBox(QPixmap(m_checkedImagePath));

    setRadioOff(QPixmap(m_radioOffImagePath));
    setRadioOn(QPixmap(m_radioOnImagePath));

    setScrollAddArrow(QPixmap(m_scrollAddImagePath));
    setScrollSubArrow(QPixmap(m_scrollSubImagePath));

    setBaseColor(QColor(243,246,251));
    setBorderColor(QColor(200,206,220));
    setFontColor(QColor(63,131,255));

    setFont(QFont(QString::fromLocal8Bit("Arial Unicode MS"), 11));
    setHeaderColor(QColor(100,104,115));
}

CustomTheme::~CustomTheme(void)
{
}

void CustomTheme::setSrcProperty()
{
    this->m_btnSelectedPath = "resources/icon/buttons/selected.png";
    this->m_btnUnSelectedPath = "resources/icon/buttons/unselected.png";
    this->m_btnHasFocusBg = QPixmap("resources/icon/buttons/hasFocus.png");
    this->m_leftArrowPath = "resources/icon/buttons/leftArrow.png";
    this->m_rightArrowPath = "resources/icon/buttons/rightArrow.png";
    this->m_uncheckedImagePath = "resources/icon/buttons/unchecked.png";
    this->m_checkedImagePath = "resources/icon/buttons/checked.png";
    this->m_radioOnImagePath = "resources/icon/buttons/radioOn.png";
    this->m_radioOffImagePath = "resources/icon/buttons/radioOff.png";
    this->m_comboBoxImagePath = "resources/icon/buttons/pullDownArrow.png";
    this->m_scrollAddImagePath = "resources/icon/buttons/pullDownArrow.png";
    this->m_scrollSubImagePath = "resources/icon/buttons/pullUpArrow.png";
}

void CustomTheme::polish(QPalette &palette)
{
    QColor background(255,255,255);

    palette.setBrush(QPalette::Background, background);
    palette.setBrush(QPalette::Base, m_baseColor);
}

void CustomTheme::polish(QWidget *widget)
{
    if(qobject_cast<QAbstractButton*>(widget)||qobject_cast<QAbstractSpinBox*>(widget))
    {
        widget->setAttribute(Qt::WA_Hover,true);
    }
}

void CustomTheme::unpolish(QWidget *widget)
{
    if(qobject_cast<QAbstractButton*>(widget)||qobject_cast<QAbstractSpinBox*>(widget))
    {
        widget->setAttribute(Qt::WA_Hover,false);
    }
}

int CustomTheme::styleHint(StyleHint hint, const QStyleOption *option, const QWidget *widget,
    QStyleHintReturn *returnData) const 
{
    return QProxyStyle::styleHint(hint, option, widget,returnData);
}

int CustomTheme::pixelMetric(PixelMetric metric, const QStyleOption *option, 
    const QWidget *widget) const
{
    switch (metric)
    {
    case PM_TabBarScrollButtonWidth:
        return 0;

    case PM_DefaultFrameWidth:
        return 1;

    case PM_ComboBoxFrameWidth:
        return 5;

    case PM_IndicatorHeight:
    case PM_IndicatorWidth:

    case PM_ExclusiveIndicatorWidth:
    case PM_ExclusiveIndicatorHeight:
        return 20;

    case PM_CheckBoxLabelSpacing:
    case PM_RadioButtonLabelSpacing:
        return 10;

    case PM_TabBarTabVSpace:
        return 28;
//     case PM_TabBarTabHSpace:
//         return 75;

    case PM_ScrollBarSliderMin:
        return 100;
    case PM_ScrollBarExtent:
        return QProxyStyle::pixelMetric(metric, option, widget)/2; 

    default:
        return QProxyStyle::pixelMetric(metric, option, widget);
    }
}

void CustomTheme::drawPrimitive(PrimitiveElement primitive, const QStyleOption *option, 
    QPainter *painter, const QWidget *widget) const
{
    painter->setRenderHint(QPainter::SmoothPixmapTransform);

    switch (primitive)
    {
    case PE_FrameDefaultButton:
        break;
    case PE_PanelButtonCommand:
        {
            const_cast<QWidget *>(widget)->setFont(m_font);
            //设置背景
            painter->drawPixmap(option->rect, m_btnBackground);
            if (option->state & QStyle::State_HasFocus)
            {
                painter->drawPixmap(option->rect, m_btnHasFocusBg);
            }
            mouseResponse(primitive, option, painter, widget);
        }
        break;

    case PE_IndicatorCheckBox:
        drawCheckBoxIndicator(option, painter);
        break;

    case PE_IndicatorRadioButton:
        drawRadioIndicator(option, painter);
        break;

    case PE_FrameTabWidget:
        drawFrameTabWidget(option, painter, const_cast<QWidget *>(widget));
        break;

    case PE_FrameFocusRect:
        painter->setPen(QPen(QColor(107,181,255), 2, Qt::SolidLine));
        painter->setBrush(QColor(107,181,255, 25));
        painter->drawRect(option->rect);
        break;

    case PE_PanelLineEdit:
    //    const_cast<QWidget *>(widget)->setFont(QFont(QString::fromLocal8Bit("Arial Unicode MS")));
    //    //painter->fillRect(option->rect, QColor(236, 238, 240));
    //    painter->setPen(QPen(Qt::gray, 2.0));
    //    painter->drawRect(option->rect);

        if (option->state & State_HasFocus)
        {
            painter->setPen(QPen(QColor(107,181,255), 2, Qt::SolidLine));
            painter->setBrush(QColor(107,181,255, 25));
            painter->drawRect(option->rect);
        }
        else 
        {
            QProxyStyle::drawPrimitive(primitive, option, painter, widget);
        }
        break;

    case PE_FrameLineEdit:
        painter->fillRect(option->rect, QColor(236, 238, 240));
        painter->setPen(QPen(QColor(200, 208, 221), 1));
        painter->drawRect(option->rect);
        break;

    default:
        QProxyStyle::drawPrimitive(primitive, option, painter, widget);
        break;
    }
}

void CustomTheme::drawControl(ControlElement element, const QStyleOption *option, 
    QPainter *painter, const QWidget *widget) const
{
    switch (element)
    {
    case CE_TabBarTabShape:
        drawTabBar(option, painter, const_cast<QWidget *>(widget));
        break;

    case CE_RadioButton:
        QProxyStyle::drawControl(element, option, painter, widget);
        break;

    case CE_ComboBoxLabel:
    case CE_RadioButtonLabel:
    case CE_CheckBoxLabel:
        drawFont(element, option, painter, widget);
        break;

    //表格格式化
    case CE_Header:
        
        if (widget->parent()->metaObject()->className() == QString("StorageView"))
            return QProxyStyle::drawControl(element,option, painter,widget);
        painter->setPen(Qt::NoPen);
        painter->setBrush(QColor(Qt::white));
        if (option->state & State_Horizontal)
        {
            painter->setBrush(QColor(231,231,231));
        }

        painter->drawRect(option->rect);

        if (option->state & (State_On | State_Sunken | State_Selected))
        {
            painter->setBrush(QColor(228,228,228));
            painter->drawRect(option->rect.adjusted(-1,0,-1,0));
        }
        drawFont(element, option, painter);
        break;
    default:
        QProxyStyle::drawControl(element, option, painter, widget);
        break;
    }
}

void CustomTheme::drawComplexControl(ComplexControl control, const QStyleOptionComplex *option,
    QPainter *painter, const QWidget *widget) const
{
    if (control == CC_SpinBox)
    {
        setComplexControlFont(control, option, painter, widget);
        drawSpinBoxButton(SC_SpinBoxUp, option, painter);
        drawSpinBoxButton(SC_SpinBoxDown, option, painter);
        drawSpinBoxEditField(SC_SpinBoxEditField, option, painter);
        if (option->state & (QStyle::State_Editing | QStyle::State_HasFocus))
        {
//             painter->setPen(QPen(QColor(107,181,255), 2, Qt::SolidLine));
//             painter->drawRect(option->rect);
        }
    }
    else if (control == CC_ComboBox)
    {
        //setComplexControlFont(control, option, painter, widget);
        drawComboBoxArrow(SC_ComboBoxArrow, option, painter);
        drawComboBoxEditField(SC_ComboBoxEditField, option, painter);
        if (option->state & QStyle::State_HasFocus)
        {
            painter->setPen(QPen(QColor(107,181,255), 2, Qt::SolidLine));
            painter->setBrush(QColor(107,181,255, 25));
            painter->drawRect(option->rect);
        }
    }
    else if (control == CC_ScrollBar)
    {
        drawScrollBarGroove(SC_ScrollBarGroove, option, painter);
        drawScrollBarSlider(SC_ScrollBarSlider, option, painter);
        drawScrollBarPage(SC_ScrollBarSubPage, option, painter);
        drawScrollBarPage(SC_ScrollBarAddPage, option, painter);
        drawScrollBarArrow(SC_ScrollBarSubLine, option, painter);
        drawScrollBarArrow(SC_ScrollBarAddLine, option, painter);
    }
    else
    {
        QProxyStyle::drawComplexControl(control, option, painter, widget);
    }
}

QRect CustomTheme::subControlRect(ComplexControl control, const QStyleOptionComplex *option, 
    SubControl sc,const QWidget *widget) const
{
    if (control == CC_SpinBox)
    {
        int frameWidth = pixelMetric(PM_DefaultFrameWidth, option, widget);
        int btnWidth = option->rect.height();
        if (option->rect.width() <= option->rect.height()*3)
        {
            btnWidth = option->rect.width()/3;
        }
        switch (sc)
        {
        case SC_SpinBoxFrame:
            return option->rect;
            break;
        case SC_SpinBoxEditField:
            return option->rect.adjusted(+btnWidth, +frameWidth, -btnWidth, -frameWidth);
            break;

        case SC_SpinBoxUp:
            return visualRect(option->direction, option->rect, 
                QRect(option->rect.right()-btnWidth,option->rect.top(), btnWidth+1, option->rect.height()));
            break;

        case SC_SpinBoxDown:
            return visualRect(option->direction, option->rect, 
                QRect(option->rect.left(), option->rect.top(), btnWidth+1, option->rect.height()));
            break;

        default:
            return QRect();

        }
    }
    else if (control == CC_ComboBox)
    {
        QRect rect = QProxyStyle::subControlRect(control, option, sc, widget);
        int arrowWidth = option->rect.height();
        if (option->rect.width() <= option->rect.height() * 3)
        {
            arrowWidth = option->rect.width() / 3;
        }
        switch (sc)
        {
        case SC_ComboBoxFrame:
            return option->rect;

        case SC_ComboBoxEditField:
            return option->rect.adjusted(0, 0, -arrowWidth, 0);

        case SC_ComboBoxArrow:
            return visualRect(option->direction, option->rect, 
                QRect(option->rect.right()-arrowWidth, option->rect.top(), arrowWidth, option->rect.height()));

        case SC_ComboBoxListBoxPopup:
            return rect.adjusted(-1, 0, -1, 0);

        default:
            return QRect();
        }
    }
    else if (control == CC_ScrollBar)
    {
        QRect rect = QProxyStyle::subControlRect(control, option, sc, widget);
        switch (sc)
        {
        case SC_ScrollBarSlider:
            return rect.adjusted(+rect.width()/4, -3, -rect.width()/4, +3);

        default:
            return QProxyStyle::subControlRect(control, option, sc, widget);
        }
    }

    return QProxyStyle::subControlRect(control, option, sc, widget);
}

QRect CustomTheme::subElementRect(SubElement element, const QStyleOption * option, 
    const QWidget * widget) const
{ 
    switch (element)
    {
    case SE_TabWidgetLeftCorner:
    case SE_TabWidgetRightCorner:
        return QRect();

    case SE_RadioButtonClickRect:
        return option->rect;
   }
    return QProxyStyle::subElementRect(element, option, widget);
}

void CustomTheme::drawSpinBoxButton(SubControl sc, const QStyleOptionComplex *option,
    QPainter *painter) const
{
    QRect arrowRect = subControlRect(CC_SpinBox, option, sc);
    //绘制箭头
    if (sc == SC_SpinBoxUp)
    {
        painter->drawPixmap(arrowRect, m_spinRightArrow);
    }
    else if (sc == SC_SpinBoxDown)
    {
        painter->drawPixmap(arrowRect, m_spinArrow);
    }


    painter->setPen(QPen(QBrush(m_borderColor), 1.0));
    painter->drawLine(arrowRect.topLeft(), arrowRect.topRight());
    painter->drawLine(arrowRect.topLeft(), arrowRect.bottomLeft());
    painter->drawLine(arrowRect.topRight(), arrowRect.bottomRight());
    painter->drawLine(arrowRect.bottomLeft(), arrowRect.bottomRight());

    //鼠标响应
    QStyleOption btnOption(*option);
    btnOption.rect = arrowRect;
    if(!(option->activeSubControls & sc))
    {
        btnOption.state &= ~(State_MouseOver|State_On|State_Sunken);
    }
    if (sc == SC_SpinBoxUp)
    {
        mouseResponse(PE_IndicatorSpinUp, &btnOption, painter);
    }
    else if (sc == SC_SpinBoxDown)
    {
        mouseResponse(PE_IndicatorSpinDown, &btnOption, painter);
    }
    
}

void CustomTheme::drawSpinBoxEditField(SubControl sc, const QStyleOptionComplex *option,
    QPainter *painter) const
{
    painter->setPen(QPen(QBrush(m_borderColor), 1.0));
    QRect editFieldRect = subControlRect(CC_SpinBox, option, sc).adjusted(-1, -1, 1, 1);
    painter->drawLine(editFieldRect.topLeft(), editFieldRect.topRight());
    painter->drawLine(editFieldRect.topLeft(), editFieldRect.bottomLeft());
    painter->drawLine(editFieldRect.topRight(), editFieldRect.bottomRight());
    painter->drawLine(editFieldRect.bottomLeft(), editFieldRect.bottomRight());
}

void CustomTheme::drawComboBoxArrow(SubControl sc, const QStyleOptionComplex *option,
    QPainter *painter) const
{
    QRect arrowRect = subControlRect(CC_ComboBox, option, sc);
    painter->drawPixmap(arrowRect, m_comboBoxArrow);
    //鼠标响应
    QStyleOption btnOption(*option);
    if(!(option->activeSubControls & sc))
    {
        btnOption.state &= ~(State_MouseOver|State_On|State_Sunken);
    }
}

void CustomTheme::drawComboBoxEditField(SubControl sc, const QStyleOptionComplex *option,
    QPainter *painter) const
{
    QRect editRect = subControlRect(CC_ComboBox, option, sc);
    editRect = editRect.adjusted(0,0,-1,0);
    painter->fillRect(editRect, QColor(243,246,251));

    painter->setPen(QPen(QBrush(m_borderColor), 1.0));
    painter->drawLine(editRect.topLeft(), editRect.topRight());
    painter->drawLine(editRect.topLeft(), editRect.bottomLeft());
    painter->drawLine(editRect.topRight(), editRect.bottomRight());
    painter->drawLine(editRect.bottomLeft(), editRect.bottomRight());
}

//修改View
#if 0
void CustomTheme::drawComboBoxList(const QStyleOption *option, QPainter *painter) const
{
    QStyleOptionViewItem viewOption;
    const QStyleOptionViewItem *tempOption = qstyleoption_cast<const QStyleOptionViewItem *>(option);
    viewOption = *tempOption;
    painter->setPen(QColor(63,131,255));
    painter->drawText(tempOption->rect, Qt::AlignCenter , tempOption->text );
}
#endif

void CustomTheme::drawScrollBarSlider(SubControl sc, const QStyleOptionComplex *option,
    QPainter *painter) const
{
    QRect roundRect = subControlRect(CC_ScrollBar, option, SC_ScrollBarSlider);
    if(!roundRect.isValid())  
        return;  

    int diameter=12;  
    int cx=100*diameter/roundRect.width();  
    int cy=100*diameter/roundRect.height();  

    painter->setPen(Qt::NoPen);  
    painter->setBrush(m_borderColor);
    painter->drawRoundRect(roundRect,cx,cy);  
}

void CustomTheme::drawScrollBarPage(SubControl sc, const QStyleOptionComplex *option,
    QPainter *painter) const
{
    QRect pageRect = subControlRect(CC_ScrollBar, option, SC_ScrollBarSubPage);
    if (sc == SC_ScrollBarAddPage)
    {
        pageRect = subControlRect(CC_ScrollBar, option, SC_ScrollBarAddPage);
    }

    painter->setPen(Qt::NoPen);
    painter->setBrush(Qt::white);
    painter->drawRect(pageRect);
}

void CustomTheme::drawScrollBarArrow(SubControl sc, const QStyleOptionComplex *option,
    QPainter *painter) const
{
    QRect arrowRect = subControlRect(CC_ScrollBar, option, SC_ScrollBarSubLine);
    if (sc == SC_ScrollBarAddLine)
    {
        arrowRect = subControlRect(CC_ScrollBar, option, SC_ScrollBarAddLine);
        painter->drawPixmap(arrowRect, m_scrollAddArrow);
    }
    else if (sc == SC_ScrollBarSubLine)
    {
        painter->drawPixmap(arrowRect, m_scrollSubArrow);
    } 

    if (option->state & (State_On | State_Sunken))
    {
        painter->setClipRect(arrowRect);
        QColor slightlyOpaqueBlack(0, 0, 0, 63);
        painter->fillRect(arrowRect, slightlyOpaqueBlack);  
    }
}

void CustomTheme::drawScrollBarGroove(SubControl sc, const QStyleOptionComplex *option,
    QPainter *painter) const
{
    QRect rect = subControlRect(CC_ScrollBar, option, SC_ScrollBarGroove);
    painter->setPen(Qt::NoPen);
    painter->setBrush(Qt::white);
    painter->drawRect(rect);
}

void CustomTheme::drawCheckBoxIndicator(const QStyleOption *option, QPainter *painter) const
{
    painter->setRenderHint(QPainter::Antialiasing,true);  
    painter->drawPixmap(option->rect.adjusted(+1, +1, -1, -1), m_uncheckedCheckBox);
    if (option->state & (State_On | State_Sunken))
    {
        painter->drawPixmap(option->rect, m_checkedCheckBox);
    }
}

void CustomTheme::drawRadioIndicator(const QStyleOption *option, QPainter *painter) const
{
    painter->setRenderHint(QPainter::Antialiasing,true);  
    painter->drawPixmap(option->rect, m_radioOff);
    if (option->state & (State_On | State_Sunken))
    {
        painter->drawPixmap(option->rect, m_radioOn);
    }
}

void CustomTheme::drawFrameTabWidget(const QStyleOption *option, QPainter *painter, QWidget *widget) const
{
    QColor penColor(m_borderColor);
    painter->setPen(penColor);
    widget->setFixedWidth(730);
    painter->drawLine(option->rect.topLeft(), option->rect.topRight());
    painter->drawLine(option->rect.topLeft(), option->rect.bottomLeft());
    painter->drawLine(option->rect.topRight(), option->rect.bottomRight());
    painter->drawLine(option->rect.bottomLeft(), option->rect.bottomRight());
}

void CustomTheme::drawTabBar(const QStyleOption *option, QPainter *painter, QWidget *widget) const
{
    QColor tabBarBg(231,243,255);
    QColor tabBarSelected(255, 255, 255);
    QColor penColor(m_borderColor);
    //widget->setFixedWidth(730);
    painter->setPen(penColor);

    if (option->state & (State_HasFocus | State_Selected | State_On | State_Sunken ))
    {
        painter->setBrush(tabBarSelected);
        painter->drawRect(option->rect);
    }
    else
    {
        painter->setBrush(tabBarBg);
        painter->drawRect(option->rect);
        painter->drawLine(option->rect.bottomLeft(), option->rect.bottomRight());
    }
    painter->drawLine(option->rect.topLeft(), option->rect.topRight());
    painter->drawLine(option->rect.topLeft(), option->rect.bottomLeft());
    painter->drawLine(option->rect.topRight(), option->rect.bottomRight());
}

void CustomTheme::drawFont(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    painter->setPen(m_fontColor);
    painter->setFont(m_font);

    if (element == CE_ComboBoxLabel)
    {
        if (!(option->state & State_Enabled))
        {
            painter->setPen(Qt::gray);
        }

        QStyleOptionComboBox comboBoxOption;
        const QStyleOptionComboBox *tempOption = qstyleoption_cast<const QStyleOptionComboBox *>(option);
        comboBoxOption = *tempOption;
        painter->drawText(comboBoxOption.rect.adjusted(+5,0,0,0), Qt::AlignVCenter, comboBoxOption.currentText);
    }
    else if (element == CE_RadioButtonLabel || element == CE_CheckBoxLabel)
    {
        QStyleOptionButton btnOption;
        const QStyleOptionButton *tempOption = qstyleoption_cast<const QStyleOptionButton *>(option);
        btnOption = *tempOption;
        
        btnOption.palette.setBrush(QPalette::WindowText, m_fontColor);

        if (!(option->state & State_Enabled))
        {
            btnOption.palette.setBrush(QPalette::WindowText, Qt::gray);
        }

        QWidget *tempWidget = const_cast<QWidget *>(widget);
        tempWidget->setFont(m_font);

        QProxyStyle::drawControl(element, &btnOption, painter, widget);
    }
    else if (element == CE_Header)
    {
        QFont font;
        font.setBold(false);
        font.setPointSize(13);
        painter->setFont(font);
        painter->setPen(QColor(m_headerColor));
        QStyleOptionHeader headerOption;
        const QStyleOptionHeader *tempOption = qstyleoption_cast<const QStyleOptionHeader *>(option);
        headerOption = *tempOption;
        if (option->state & State_Horizontal)
        {
            painter->setPen(Qt::black);
            painter->drawText(headerOption.rect.adjusted(-1, 0, 0, 0), Qt::AlignCenter, headerOption.text);
        }
        else
        {
            painter->drawText(headerOption.rect.adjusted(-1, 0, 0, 0), Qt::AlignLeft | Qt::AlignVCenter, headerOption.text);
        }
    }
}

void CustomTheme::setComplexControlFont(ComplexControl cc, const QStyleOptionComplex *option,
    QPainter *painter, const QWidget *widget) const
{
    QWidget *tempWidget = const_cast<QWidget *>(widget);
    tempWidget->setFont(m_font);
    QProxyStyle::drawComplexControl(cc, option, painter, widget);
}

void CustomTheme::mouseResponse(PrimitiveElement pe, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    if (pe == PE_IndicatorSpinUp || pe == PE_IndicatorSpinDown)
    {
        if (option->state & State_MouseOver)
        {
            QColor slightlyOpaqueBlack(0,0,0,30);  
            painter->setBrush(slightlyOpaqueBlack);  
            painter->drawRect(option->rect);
        }

        if (option->state & (State_On | State_Sunken))
        {
            QColor slightlyOpaqueBlack(0,0,0,63);  
            painter->setBrush(slightlyOpaqueBlack);  
            painter->drawRect(option->rect);
        }
    }
    else if (pe == PE_PanelButtonCommand)
    {
        if (option->state & (State_On | State_Sunken))
        {
            painter->drawPixmap(option->rect, m_btnSelectedImage);
        }
    }   
}

//set properties' values
const QPixmap & CustomTheme::getBtnBackground() const
{
    return m_btnBackground;
}

void CustomTheme::setBtnBackground(const QPixmap &pixmap)
{
    this->m_btnBackground = pixmap;
}

const QPixmap &CustomTheme::getBtnSelectedImage() const
{
    return m_btnSelectedImage;
}

void CustomTheme::setBtnSelectedImage(const QPixmap &pixmap)
{
    this->m_btnSelectedImage = pixmap;
}

const QPixmap & CustomTheme::getSpinArrow() const
{
    return m_spinArrow;
}

void CustomTheme::setSpinArrow(const QPixmap &pixmap)
{
    this->m_spinArrow = pixmap;
}

const QPixmap& CustomTheme::getRightSpinArrow() const
{
    return m_spinRightArrow;
}

void CustomTheme::setRightSpinArrow(const QPixmap &pixmap)
{
    this->m_spinRightArrow = pixmap;
}

const QPixmap& CustomTheme::getcomboBoxArrow() const
{
    return m_comboBoxArrow;
}

void CustomTheme::setcomboBoxArrow(const QPixmap &pixmap)
{
    this->m_comboBoxArrow = pixmap;
}

const QPixmap& CustomTheme::getUncheckedCheckBox() const
{
    return m_uncheckedCheckBox;
}

void CustomTheme::setUncheckedCheckBox(const QPixmap &pixmap)
{
    this->m_uncheckedCheckBox = pixmap;
}

const QPixmap& CustomTheme::getCheckedCheckBox() const
{
    return m_checkedCheckBox;
}

void CustomTheme::setCheckedCheckBox(const QPixmap &pixmap)
{
    this->m_checkedCheckBox = pixmap;
}

const QPixmap &CustomTheme::getRadioOff() const
{
    return m_radioOff;
}

void CustomTheme::setRadioOff(const QPixmap &pixmap)
{
    this->m_radioOff = pixmap;
}

const QPixmap &CustomTheme::getRadioOn() const
{
    return m_radioOn;
}

void CustomTheme::setRadioOn(const QPixmap &pixmap)
{
    this->m_radioOn = pixmap;
}

const QPixmap &CustomTheme::getScrollAddArrow() const
{
    return m_scrollAddArrow;
}

void CustomTheme::setScrollAddArrow(const QPixmap &pixmap)
{
    this->m_scrollAddArrow = pixmap;
}

const QPixmap &CustomTheme::getScrollSubArrow() const
{
    return m_scrollSubArrow;
}

void CustomTheme::setScrollSubArrow(const QPixmap &pixmap)
{
    this->m_scrollSubArrow = pixmap;
}

const QColor &CustomTheme::getBaseColor() const
{
    return m_baseColor;
}

void CustomTheme::setBaseColor(const QColor &baseColor)
{
    m_baseColor = baseColor;
}

const QColor &CustomTheme::getBorderColor() const
{
    return m_borderColor;
}

void CustomTheme::setBorderColor(const QColor &borderColor)
{
    m_borderColor = borderColor;
}

const QColor &CustomTheme::getHeaderColor() const
{
    return m_headerColor;
}

void CustomTheme::setHeaderColor(const QColor &headerColor)
{
    m_headerColor = headerColor;
}

const QColor &CustomTheme::getFontColor() const
{
    return m_fontColor;
}

void CustomTheme::setFontColor(const QColor &fontColor)
{
    m_fontColor = fontColor;
}

const QFont &CustomTheme::getFont() const
{
    return m_font;
}

void CustomTheme::setFont(const QFont &font)
{
    m_font = font;
}