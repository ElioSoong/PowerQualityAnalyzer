/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  Test.h
** Latest Version:             V1.0.0
** Latest modified Date:       2016/3/1
** Modified by:                
** Descriptions:               
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 
** Created date:               2016/2/29
** Descriptions:               
** 
*********************************************************************************************************/
#ifndef __TEST_H__
#define __TEST_H__

#include "includes.h"
#include <QDateTime>
#include <QObjectList>
#include <QtWidgets/QWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QLabel>

class JsonLoader;
class QSignalSpy;

/** 
 * 用于单元测试的帮助函数，可以从文件中加载指定类型的对象并输出加载错误
 * @param[in]  loader       JsonLoader加载器
 * @param[in]  filePath     JSON脚本路径
 * @param[out] errorMessage 加载错误信息
 * @return     T 加载得到的指定类型的对象
 */
template <typename T>
T load(JsonLoader& loader, const QString& filePath, QString* errorMessage = NULL)
{
    QSignalSpy errorSpy(&loader, SIGNAL(error(int, const QString&)));
    T result = loader.load<T>(filePath);
    if (errorMessage)
    {
        errorMessage->clear();
        for (auto iter = errorSpy.cbegin(); iter != errorSpy.cend(); ++iter)
        {
            int code = iter->at(0).toInt();
            QString message = iter->at(1).toString();
            if (code > 0)
                errorMessage->append(message);
        }
    }

    return result;
}

class QObjectWrapper : public QObject
{
    Q_OBJECT
public:
    QObjectWrapper(){}
    QObjectWrapper(const QObjectWrapper&){}
};

class QWidgetWrapper : public QWidget
{
    Q_OBJECT
public:
    QWidgetWrapper(){}
    QWidgetWrapper(const QWidgetWrapper&){}
};

class QPushButtonWrapper : public QPushButton
{
    Q_OBJECT
public:
    QPushButtonWrapper(){}
    QPushButtonWrapper(const QPushButtonWrapper&){}
};

class QLabelWrapper : public QLabel
{
    Q_OBJECT
public:
    QLabelWrapper(){}
    QLabelWrapper(const QLabelWrapper&){}
};

class OperationModeFlags : public QObject
{
    Q_OBJECT
    Q_ENUMS(OperationMode)
    Q_FLAGS(OperationModes)

public:
    OperationModeFlags() {}
    OperationModeFlags(const OperationModeFlags&) {}

    /**
     *  @enum  OperationMode
     *  @brief 操作模式，主要指方向键的操作模式
     */
    enum OperationMode
    {
        Disabled = 0,                                                       //!< 禁用该操作
        PagingMode,                                                         //!< 翻页模式
        ZoomMode,                                                           //!< 缩放模式
        CursorMode,                                                         //!< 移动光标模式
        LayoutMode,                                                         //!< 切换布局模式
        NavigateMode                                                        //!< 切换页面模式
    };
    Q_DECLARE_FLAGS(OperationModes, OperationMode);
};
Q_DECLARE_OPERATORS_FOR_FLAGS(OperationModeFlags::OperationModes);

class MyViewFlags : public QObject
{
    Q_OBJECT
    Q_ENUMS(ViewFlag)
    Q_FLAGS(ViewFlags)

public:
    MyViewFlags() {}
    MyViewFlags(const MyViewFlags&) {}

    /**
     *  @enum  ViewFlags
     *  @brief 视图选项相关标志位
     */
    enum ViewFlag 
    {
        Default = 0x00000000,                                               //!< 默认选项
        ShowNavigationBar = 0x00000010,                                     //!< 显示导航栏
        ShowMarkerBar = 0x00000020,                                         //!< 显示标签栏
        ShowDecorationBar = 0x00000040,                                     //!< 显示底部装饰条
        ShowPopWidget = 0x00000080,                                          //!< 显示弹出窗口
        AlignLeft = 0x00000100,                                             //!< 视图位于屏幕左侧
        AlignRight = 0x00000200,                                            //!< 视图位于屏幕右侧
        AlignTop = 0x00000400,                                              //!< 视图位于屏幕上方
        AlignBottom = 0x00000800,                                           //!< 视图位于屏幕下方
        AnimateDarker = 0x00001000,                                         //!< 视图出现时，背景变暗（动画）
        AnimateShift = 0x00002000,                                          //!< 视图出现时，背景平移至当前视图的旁边
        AnimateEnablePreviousView = 0x00008000,                             //!< 视图出现时，仍然使能上一个视图的UI交互
    };
    Q_DECLARE_FLAGS(ViewFlags, ViewFlag);
};
Q_DECLARE_OPERATORS_FOR_FLAGS(MyViewFlags::ViewFlags);

class ChildView : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name MEMBER m_name)
    Q_PROPERTY(QPushButton* defaultButton MEMBER m_defaultButton)

public:
    ChildView() : QObject(), m_defaultButton(new QPushButton())
    {

    }

    ChildView(const ChildView& other)
    {

    }

public:
    QString      m_name;
    QPushButton* m_defaultButton;
};

/**
 * @class View
 * @brief 视图基类，代表单窗口UI的一个页面
 */
class View : public QObject
{
    Q_OBJECT
    //Q_ENABLE_COPY(View);
    Q_PROPERTY(QString 	title	 MEMBER m_title)
    Q_PROPERTY(QString 	subTitle	 MEMBER m_subTitle)
    Q_PROPERTY(int 	flags	 MEMBER m_flags)
    Q_PROPERTY(QWidget* 	widget	 MEMBER m_widget)
    Q_PROPERTY(QVector<QPushButton*> 	buttons	 MEMBER m_buttons)
    Q_PROPERTY(QPushButton* defaultButton MEMBER m_defaultButton)
    Q_PROPERTY(QString defaultButtonText READ defaultButtonText WRITE setDefaultButtonText NOTIFY defaultButtonTextChanged)
    Q_PROPERTY(QStringList 	stringList1	 MEMBER m_stringList1)
    Q_PROPERTY(QList<int> 	intList	 MEMBER m_intList)
    Q_PROPERTY(QList<double> 	doubleList	 MEMBER m_doubleList)
    Q_PROPERTY(QList<QString> 	stringList2	 MEMBER m_stringList2)
    Q_PROPERTY(QList<bool> 	boolList MEMBER m_boolList)
    Q_PROPERTY(QList<QVariant> varList MEMBER m_varList)
    Q_PROPERTY(QObjectList 	objectList1 MEMBER m_objectList1)
    Q_PROPERTY(QList<QObject*> 	objectList2 MEMBER m_objectList2)
    Q_PROPERTY(bool bool1 MEMBER m_bool1 NOTIFY bool1Changed)
    Q_PROPERTY(bool bool2 READ bool2 WRITE setBool2 NOTIFY bool2Changed)
    Q_PROPERTY(QPixmap icon READ icon WRITE setIcon)
    Q_PROPERTY(MyViewFlags::ViewFlags viewFlags MEMBER m_viewFlags)
    Q_PROPERTY(QFont font MEMBER m_font)
    Q_PROPERTY(QSize size MEMBER m_size)
    Q_PROPERTY(QRect rect MEMBER m_rect)
    Q_PROPERTY(QColor color MEMBER m_color)
    Q_PROPERTY(QDateTime dateTime MEMBER m_dateTime)
    Q_PROPERTY(QLineEdit* lineEdit MEMBER m_lineEdit)
    Q_PROPERTY(ChildView* view MEMBER m_view)
    Q_PROPERTY(QRegExp regExp MEMBER m_regExp)
    Q_PROPERTY(QVariant var MEMBER m_var)

public: 
    /**
     * Constructor
     */
    View();
    View(const View& other);

    /**
     * 
     * @param[$paramDirection]title	$paramDocumentRaw
     */
    void setTitle(QString title);

    /**
     * 
     * @return      
     */
    QString getTitle() const;

    /**
     * 
     * @param[$paramDirection]subTitle	$paramDocumentRaw
     */
    void setSubTitle(QString subTitle);

    /**
     * 
     * @return      
     */
    QString getSubTitle() const;

    /**
     * 
     * @param[$paramDirection]flags	$paramDocumentRaw
     */
    void setFlags(int flags);

    /**
     * 
     * @return      
     */
    int getFlags() const;

    /**
     * 
     * @param[$paramDirection]widget	$paramDocumentRaw
     */
    void setWidget(QWidget* widget);

    /**
     * 
     * @return      
     */
    QWidget* getWidget() const;

    QPixmap icon() const
    {
        return m_icon;
    }

    void setIcon(const QPixmap& icon)
    {
        m_icon = icon;
    }

    bool bool1() const
    {
        return m_bool1;
    }

    bool bool2() const
    {
        return m_bool2;
    }

    void setBool1(bool b)
    {
        m_bool1 = b;
        emit bool1Changed();
    }

    void setBool2(bool b)
    {
        m_bool2 = b;
        emit bool2Changed(b);
    }

    QString defaultButtonText() const
    {
        return m_defaultButtonText;
    }

    void setDefaultButtonText(const QString& text);

    Q_SIGNAL void defaultButtonTextChanged();

signals:
    void signal1();
    void signal2(int a) const;
    void signal3(bool checked = false);
    void bool1Changed();
    void bool2Changed(bool bool2);

public slots:
    void slot1(bool checked = false);
    bool slot2(const QString& param1);
    void slot2(const QString& param1, int param2);
    void slot3(int a) const;


protected: 
    QString 		m_title;                      //!< 当前视图的标题，可能显示于标题栏等位置
    QString 		m_subTitle;                   //!< 当前视图的副标题，可能显示于标题栏等位置
    /**
     * 当前视图的选项，用于控制视图风格等
     * 该字段是按位定义的，基类中并不规定任何特殊选项，
     * 若需要使用，请在子类中定义并在MainWindow的子类中进行解析
     */
    int 		m_flags;
    QWidget* 		m_widget;                    //!< 当前视图对应的主控件，通常显示于主窗口的中心区域
    QVector<QPushButton*> m_buttons;
    QString     m_defaultButtonText;

public: 
    QStringList     m_stringList1;
    QList<int>      m_intList;
    QList<double>   m_doubleList;
    QList<QString>  m_stringList2;
    QList<bool>     m_boolList;
    QList<QVariant> m_varList;
    QObjectList     m_objectList1;
    QList<QObject*> m_objectList2;
    bool            m_bool1;
    bool            m_bool2;
    QPixmap         m_icon;
    MyViewFlags::ViewFlags m_viewFlags;
    QFont           m_font;
    QSize           m_size;
    QRect           m_rect;
    QColor          m_color;
    QDateTime       m_dateTime;
    QLineEdit*      m_lineEdit;
    QPushButton*    m_defaultButton;
    ChildView*      m_view;
    QRegExp         m_regExp;
    QVariant        m_var;
};

class Field : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString id READ name WRITE setName STORED false)
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QString channelMask MEMBER m_channelMask)
    Q_PROPERTY(QList<Field*> children MEMBER m_children)
    Q_PROPERTY(qreal min MEMBER m_min)
    Q_PROPERTY(qreal max MEMBER m_max)
    Q_PROPERTY(QString dimension MEMBER m_dimension)
    Q_PROPERTY(int digitals MEMBER m_digitals)
    Q_PROPERTY(QString linkPath MEMBER m_linkPath)
    Q_PROPERTY(Field* size MEMBER m_size)
    Q_PROPERTY(Field* valid MEMBER m_valid)
    Q_PROPERTY(Field* link MEMBER m_link)

    Q_PROPERTY(int maxHarmonicOrder MEMBER m_maxHarmonicOrder)
    Q_PROPERTY(QString harmonicType MEMBER m_harmonicType)
    Q_PROPERTY(bool isInterHarmonic MEMBER m_isInterHarmonic)
    Q_PROPERTY(bool isMoniterType MEMBER m_isMotinerType)

public:
    Field() : m_size(NULL), m_valid(NULL), m_link(NULL)
    {

    }

    Field(const Field& other)
    {

    }

    QString name() const
    {
        return m_name;
    }

    void setName(const QString& name)
    {
        m_name = name;
        setObjectName(name);
    }

    /*!
     * Getter/Setter for link
     */
    Field* link() const
    {
        return m_link;
    }
    void setLink(Field* link)
    {
        m_link = link;
    }
    /*!
     * Getter/Setter for children
     */
    QList<Field *> children() const
    {
        return m_children;
    }
    void setChildren(const QList<Field *>& children)
    {
        m_children = children;
    }
public:
    QString m_name;
    QString m_channelMask;
    QList<Field*> m_children;
    qreal m_min;
    qreal m_max;
    QString m_dimension;
    int m_digitals;
    QString m_linkPath;
    Field* m_size;
    Field* m_valid;
    Field* m_link;

    int m_maxHarmonicOrder;
    QString m_harmonicType;
    bool m_isInterHarmonic;
    bool m_isMotinerType;
};


class PlanView : public View
{
    Q_OBJECT
    Q_PROPERTY(QList<QAction*> actions READ actions WRITE setActions)

public:
    PlanView() : View()
    {

    }

    /*!  
     * Getter/Setter for actions
     */
    QList<QAction*> actions() const
    {
        if (!m_widget)
            return QList<QAction*>();

        return m_widget->actions();
    }
    void setActions(const QList<QAction*>& actions)
    {
        if (!m_widget)
            return;

        m_widget->addActions(actions);
    }
};

class CustomObject : public QObject
{
    Q_OBJECT
public:
    CustomObject(const QString& value = "inno")
    {
        m_value = value;
    }
    CustomObject(const CustomObject& other)
    {
        m_value = other.m_value;
    }

public:
    QString m_value;
};

/*
 * @brief 用于各种通用测试的公共测试类，简化处理，避免不同的单元测试使用大量不同的测试类
 */
class GeneralTestClass : public QObject
{
    Q_OBJECT
    Q_ENUMS(GeneralTestEnum)
    Q_FLAGS(GeneralTestFlag)
    Q_PROPERTY(int _int MEMBER m_int)
    Q_PROPERTY(uint _uint MEMBER m_uint)
    Q_PROPERTY(short _short MEMBER m_short)
    Q_PROPERTY(ushort _ushort MEMBER m_ushort)
    Q_PROPERTY(char _char MEMBER m_char)
    Q_PROPERTY(uchar _uchar MEMBER m_uchar)
    Q_PROPERTY(float _float MEMBER m_float)
    Q_PROPERTY(double _double MEMBER m_double)
    Q_PROPERTY(void* _voidPtr MEMBER m_voidPtr)
    Q_PROPERTY(bool _bool MEMBER m_bool)
    Q_PROPERTY(QString string MEMBER m_string)
    Q_PROPERTY(QRegExp regExp MEMBER m_regExp)
    Q_PROPERTY(QVariant variant MEMBER m_variant)
    Q_PROPERTY(GeneralTestClass::GeneralTestFlag _flag MEMBER m_flag)
    Q_PROPERTY(GeneralTestClass::GeneralTestEnum _enum MEMBER m_enum)
    Q_PROPERTY(QFont font MEMBER m_font)
    Q_PROPERTY(QDateTime dateTime MEMBER m_dateTime)
    Q_PROPERTY(QStringList stringList MEMBER m_stringList)
    Q_PROPERTY(QVariant stringVariant MEMBER m_stringVariant)
    Q_PROPERTY(QVariant intVariant MEMBER m_intVariant)

public:
    enum GeneralTestEnum
    {
        None = 0,
        EnumValue01 = 0x01,
        EnumValue02 = 0x02,
        EnumValue04 = 0x04
    };
    Q_DECLARE_FLAGS(GeneralTestFlag, GeneralTestEnum);

public:
    GeneralTestClass() 
        : m_int(0)
        , m_uint(0)
        , m_short(0)
        , m_ushort(0)
        , m_char(0)
        , m_uchar(0)
        , m_float(0)
        , m_double(0)
        , m_voidPtr(0)
        , m_bool(false)
        , m_string()
        , m_regExp()
        , m_variant()
        , m_flag(None)
        , m_enum(None)
        , m_font()
        , m_dateTime()
        , m_stringList()
        , m_stringVariant()
        , m_intVariant()
    {

    }

    GeneralTestClass(const GeneralTestClass& other)
    {

    }

public:
    int m_int;
    uint m_uint;
    short m_short;
    ushort m_ushort;
    char m_char;
    uchar m_uchar;
    float m_float;
    double m_double;
    void* m_voidPtr;
    bool m_bool;
    QString m_string;
    QRegExp m_regExp;
    QVariant m_variant;
    GeneralTestEnum m_enum;
    GeneralTestFlag m_flag;
    QFont m_font;
    QDateTime m_dateTime;
    QStringList m_stringList;
    QVariant m_stringVariant;
    QVariant m_intVariant;
    CustomObject m_customObject;
};
Q_DECLARE_OPERATORS_FOR_FLAGS(GeneralTestClass::GeneralTestFlag);

class ConnectSignalSlotTest : public QObject
{
    Q_OBJECT
public:
    ConnectSignalSlotTest()
        : m_value(0)
    {

    }
    ConnectSignalSlotTest(const ConnectSignalSlotTest& other)
        : m_value(other.m_value)
    {

    }

signals:
    void mysignal();
    void othersignal(int value = 1);

public slots:
    void myslot()
    {
        ++m_value;
    }

    void otherslot(int value = 1)
    {
        m_value += value;
    }

public:
    int m_value;
};

class RefExternalObjectTest : public QObject
{
    Q_OBJECT
public:
    RefExternalObjectTest()
        : m_value(0)
    {

    }
    RefExternalObjectTest(const RefExternalObjectTest& other)
        : m_value(other.m_value)
    {

    }

public:
    int m_value;
};

class BindPropertyTest : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString property MEMBER m_property NOTIFY notifySignal)
public:
    BindPropertyTest()
    {

    }
    BindPropertyTest(const BindPropertyTest& other)
    {

    }

signals:
    void notifySignal();

public:
    QString m_property;
};

class GetObjectRefsTest : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString title MEMBER m_title)
    Q_PROPERTY(int size MEMBER m_size)
public:
    GetObjectRefsTest()
    {

    }
    GetObjectRefsTest(const GetObjectRefsTest& other)
    {

    }

public:
    QString m_title;
    int m_size;
};

class TranslatableLabel : public QLabel
{
    Q_OBJECT
public:
    TranslatableLabel()
    {

    }
    TranslatableLabel(const TranslatableLabel& other)
    {

    }
};

class CustomWidget : public QWidget
{
    Q_OBJECT
public:
    CustomWidget(QWidget* parent = nullptr)
        : QWidget(parent)
    {

    }
    CustomWidget(const CustomWidget& other)
    {

    }
};

class NestedWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QWidget* widget MEMBER m_widget)
public:
    NestedWidget(QWidget* parent = nullptr)
        : QWidget(parent)
    {

    }
    NestedWidget(const NestedWidget& other)
    {

    }

public:
    QWidget* m_widget;
};

class LastWidget : public QWidget
{
    Q_OBJECT
public:
    LastWidget(QWidget* parent = nullptr)
    {

    }
    LastWidget(const LastWidget& other)
    {

    }
};

class PropertyClass : public QObject
{
    Q_OBJECT
        Q_PROPERTY(int size MEMBER m_size)
        Q_PROPERTY(QString str MEMBER m_str)
public:
    PropertyClass()
    {

    }
    PropertyClass(const PropertyClass& other)
    {
        m_size = other.m_size;
        m_str = other.m_str;
    }

    bool operator ==(const PropertyClass& other)
    {
        m_size = other.m_size;
        m_str = other.m_str;
        return true;
    }

public:
    int m_size;
    QString m_str;
};
//Q_DECLARE_METATYPE(PropertyClass)

class PropertyAppendValue : public QObject
{
    Q_OBJECT
        Q_PROPERTY(QList<QString> list MEMBER m_list)
        Q_PROPERTY(QList<QString> otherList MEMBER m_otherList)
        Q_PROPERTY(QList<int> intList MEMBER m_intList)
        Q_PROPERTY(QList<PropertyClass*> propertyList MEMBER m_propertyList)
public:
    PropertyAppendValue()
    {

    }
    PropertyAppendValue(const PropertyAppendValue& other)
    {

    }

public:
    QList<QString> m_list;
    QList<QString> m_otherList;
    QList<int> m_intList;
    QList<PropertyClass*> m_propertyList;
};

class QVariantProperty : public QObject
{
    Q_OBJECT
        Q_PROPERTY(QVariant prop MEMBER m_prop)
public:
    QVariant m_prop;
};

#endif
/*********************************************************************************************************
** End of file
*********************************************************************************************************/