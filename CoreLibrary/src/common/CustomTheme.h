/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  CustomTheme.h
** Latest Version:             
** Latest modified Date:       2015/08/12 
** Modified by:                SongYunhua
** Descriptions:               
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 Song Yunhua
** Created date:               2015/08/10 
** Descriptions:               
** 
*********************************************************************************************************/

#pragma once
#include <QProxyStyle>
#include <QFont>

class CustomTheme :
    public QProxyStyle
{
    Q_OBJECT
    Q_PROPERTY(QPixmap m_btnBackground READ getBtnBackground WRITE setBtnBackground)
    Q_PROPERTY(QPixmap m_btnSelectedImage READ getBtnSelectedImage WRITE setBtnSelectedImage)
    Q_PROPERTY(QPixmap m_spinArrow READ getSpinArrow WRITE setSpinArrow)
    Q_PROPERTY(QPixmap m_spinRightArrow READ getRightSpinArrow WRITE setRightSpinArrow)
    Q_PROPERTY(QPixmap m_comboBoxArrow READ getcomboBoxArrow WRITE setcomboBoxArrow)
    Q_PROPERTY(QPixmap m_uncheckedCheckBox READ getUncheckedCheckBox WRITE setUncheckedCheckBox)
    Q_PROPERTY(QPixmap m_checkedCheckBox READ getCheckedCheckBox WRITE setCheckedCheckBox)
    Q_PROPERTY(QPixmap m_radioOff READ getRadioOff WRITE setRadioOff)
    Q_PROPERTY(QPixmap m_radioOn READ getRadioOn WRITE setRadioOn)
    Q_PROPERTY(QPixmap m_scrollAddArrow READ getScrollAddArrow WRITE setScrollAddArrow)
    Q_PROPERTY(QPixmap m_scrollSubArrow READ getScrollSubArrow WRITE setScrollSubArrow)

    Q_PROPERTY(QColor m_baseColor READ getBaseColor WRITE setBaseColor)
    Q_PROPERTY(QColor m_borderColor READ getBorderColor WRITE setBorderColor)
    Q_PROPERTY(QColor m_fontColor READ getFontColor WRITE setFontColor)

    Q_PROPERTY(QFont m_font READ getFont WRITE setFont)

public:
    CustomTheme(void);
    ~CustomTheme(void);

    void setSrcProperty();
    /*!  
     * Q_PROPERTY相关函数的声明   
     */
    const QPixmap &getBtnBackground() const;
    void setBtnBackground(const QPixmap &);

    const QPixmap &getBtnSelectedImage() const;
    void setBtnSelectedImage(const QPixmap &);

    const QPixmap &getSpinArrow() const;
    void setSpinArrow(const QPixmap &);

    const QPixmap &getRightSpinArrow() const;
    void setRightSpinArrow(const QPixmap &);

    const QPixmap &getcomboBoxArrow() const;
    void setcomboBoxArrow(const QPixmap &);

    const QPixmap &getUncheckedCheckBox() const;
    void setUncheckedCheckBox(const QPixmap &);

    const QPixmap &getCheckedCheckBox() const;
    void setCheckedCheckBox(const QPixmap &);

    const QPixmap &getRadioOff() const;
    void setRadioOff(const QPixmap &);

    const QPixmap &getRadioOn() const;
    void setRadioOn(const QPixmap &);

    const QPixmap &getScrollAddArrow() const;
    void setScrollAddArrow(const QPixmap &);

    const QPixmap &getScrollSubArrow() const;
    void setScrollSubArrow(const QPixmap &);

    const QColor &getBaseColor() const;
    void setBaseColor(const QColor &);

    const QColor &getBorderColor() const;
    void setBorderColor(const QColor &);

    const QColor &getFontColor() const;
    void setFontColor(const QColor &);

    const QFont &getFont() const;
    void setFont(const QFont &);

    const QColor &getHeaderColor() const;
    void setHeaderColor(const QColor &);

    /*! 
     * 设置背景和前景 
     * @param[in] QPalette &palette
     */
    void polish(QPalette &palette) Q_DECL_OVERRIDE;
    void polish(QWidget *widget) Q_DECL_OVERRIDE;
    void unpolish(QWidget *widget) Q_DECL_OVERRIDE;

    /*! 
     * 设置提示组件样式    
     */
    int styleHint(StyleHint hint, const QStyleOption *option, const QWidget *widget = 0,
        QStyleHintReturn *returnData = 0) const Q_DECL_OVERRIDE;

    /*! 
     * 设置组件的大小距离等其他像素值   
     */
    int pixelMetric(PixelMetric metric, const QStyleOption *option, 
        const QWidget *widget = 0) const Q_DECL_OVERRIDE;

    /*! 
     * 对基本组件进行重绘    
     */
    void drawPrimitive(PrimitiveElement primitive, const QStyleOption *option, 
        QPainter *painter, const QWidget *widget = 0) const Q_DECL_OVERRIDE;

    /*! 
     * 绘制组件  
     */
    void drawControl(ControlElement element, const QStyleOption *option, 
        QPainter *painter, const QWidget *widget = 0) const Q_DECL_OVERRIDE;

    /*! 
     *  绘制组合组件，比如SpinBox、CheckBox   
     */
    void drawComplexControl(ComplexControl control, const QStyleOptionComplex *option,
        QPainter *painter, const QWidget *widget = 0) const Q_DECL_OVERRIDE;

    /*! 
     * 绘制子组件的位置，比如Spinbox的小箭头    
     */
    QRect subControlRect(ComplexControl control, const QStyleOptionComplex *option, 
        SubControl sc,const QWidget *widget = 0) const Q_DECL_OVERRIDE;

    /*! 
     * 绘制子元素的位置，比如TabWiget的TabBar的位置   
     */
    QRect subElementRect(SubElement element, const QStyleOption * option, 
        const QWidget * widget = 0) const Q_DECL_OVERRIDE;

private:
    void drawSpinBoxButton(SubControl sc, const QStyleOptionComplex *option,
        QPainter *painter) const;
    void drawSpinBoxEditField(SubControl sc, const QStyleOptionComplex *option,
        QPainter *painter) const;

    void drawComboBoxArrow(SubControl sc, const QStyleOptionComplex *option,
        QPainter *painter) const;
    void drawComboBoxEditField(SubControl sc, const QStyleOptionComplex *option,
        QPainter *painter) const;

    /*! 
     * 修改View的风格   
     */
    void drawComboBoxList(const QStyleOption *option, QPainter *painter) const;

    void drawScrollBarSlider(SubControl sc, const QStyleOptionComplex *option,
        QPainter *painter) const;
    void drawScrollBarPage(SubControl sc, const QStyleOptionComplex *option,
        QPainter *painter) const;
    void drawScrollBarArrow(SubControl sc, const QStyleOptionComplex *option,
        QPainter *painter) const;
    void drawScrollBarGroove(SubControl sc, const QStyleOptionComplex *option,
        QPainter *painter) const;

    void drawFont(ControlElement element, const QStyleOption *option, 
        QPainter *painter, const QWidget *widget = 0) const;

    void setComplexControlFont(ComplexControl cc, const QStyleOptionComplex *option,
        QPainter *painter, const QWidget *widget) const;

    void drawCheckBoxIndicator(const QStyleOption *option, QPainter *painter) const;
    void drawRadioIndicator(const QStyleOption *option, QPainter *painter) const;
    void drawFrameTabWidget(const QStyleOption *option, QPainter *painter, QWidget *widget) const;
    void drawTabBar(const QStyleOption *option, QPainter *painter, QWidget *widget) const;

    /*! 
     * 各组件的鼠标响应    
     */
    void mouseResponse(PrimitiveElement pe, const QStyleOption *option, QPainter *painter, const QWidget *widget = 0) const;

private:
    QString m_leftArrowPath;            //!< spinBox的左箭头
    QString m_rightArrowPath;           //!< spinBox的右箭头
    QString m_comboBoxImagePath;
    QString m_btnSelectedPath;
    QString m_btnUnSelectedPath;
    QString m_uncheckedImagePath;
    QString m_checkedImagePath;
    QString m_radioOnImagePath;
    QString m_radioOffImagePath;
    QString m_scrollAddImagePath;
    QString m_scrollSubImagePath;

    QPixmap m_btnSelectedImage;
    QPixmap m_btnBackground;
    QPixmap m_btnHasFocusBg;

    QPixmap m_spinArrow;                //!< spinBox的左箭头
    QPixmap m_spinRightArrow;           //!< spinBox的右箭头

    QPixmap m_comboBoxArrow;

    QPixmap m_uncheckedCheckBox;
    QPixmap m_checkedCheckBox;

    QPixmap m_radioOff;
    QPixmap m_radioOn;

    QPixmap m_scrollSubArrow;
    QPixmap m_scrollAddArrow;

    QColor m_borderColor;
    QColor m_baseColor;
    QColor m_fontColor;

    QFont m_font;
    QColor m_headerColor;           //!< 表头颜色
};

