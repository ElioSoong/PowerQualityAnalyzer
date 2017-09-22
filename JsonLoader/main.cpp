#include "includes.h"
#include <QtWidgets/QApplication>
#include <QList>
#include <QtWidgets/QWidget>
#include <QtWidgets/QLabel>
#include <QVariant>
#include <QDebug>
#include <QDateTime>

#include "JsonLoader.h"
#include "JsonScripter.h"

#include "Test.h"

#if defined(_MSC_VER) && !ENABLE_CUSTOM_OBJECT_FACTORY
// 这段代码用在模板参数时，gcc不支持 [5/6/2016 CHH513]
DECLARE_CLASS_WRAPPER(QObject)
DECLARE_CLASS_WRAPPER(QWidget)
DECLARE_CLASS_WRAPPER(QLabel)
DECLARE_CLASS_WRAPPER(QPushButton)
#endif

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

#if 0
    QList<int> list;
    list.push_back(1);
    list.push_back(2);
    QVariant var = QVariant::fromValue(list);
    int type = var.userType();
    Q_ASSERT(var.canConvert(QMetaType::QVariantList));
    QSequentialIterable iterable = var.value<QSequentialIterable>();
    // Can use foreach:
    foreach(const QVariant &v, iterable) {
        qDebug() << v;
    }
    var.convert(QMetaType::QVariantList);
    QVariantList varlist = var.toList();
    Q_ASSERT(!varlist.isEmpty());
#endif

    qDebug() << "Initializing ...";

    JsonLoader loader;

    const char* n1024 = QMetaType::typeName(1024);
    const char* n1025 = QMetaType::typeName(1025);
    const char* n1026 = QMetaType::typeName(1026);
    const char* n1027 = QMetaType::typeName(1027);
    const char* n1028 = QMetaType::typeName(1028);
    const char* n1029 = QMetaType::typeName(1029);
    const char* n1030 = QMetaType::typeName(1030);

#if !ENABLE_CUSTOM_OBJECT_FACTORY
    REGISTER_METATYPE_X(QObjectWrapper, "QObject");
    REGISTER_METATYPE_X(QWidgetWrapper, "QWidget");
    REGISTER_METATYPE_X(QLabelWrapper, "QLabel");
    REGISTER_METATYPE_X(QPushButtonWrapper, "QPushButton");
#else
    REGISTER_METATYPE(QObject);
    REGISTER_METATYPE(QWidget);
    REGISTER_METATYPE(QLabel);
    REGISTER_METATYPE(QPushButton);
#endif
    REGISTER_METATYPE(View);
    REGISTER_METATYPE(ChildView);
    REGISTER_METATYPE(OperationModeFlags);
    
    // 不知为何moc时不会尝试生成自动注册QObjectList属性类型的代码，导致属性写失败 [5/6/2016 CHENHONGHAO]
    qRegisterMetaType<QObjectList>("QObjectList");

    qRegisterMetaType<View>("View");

    int id1 = GET_METATYPE_ID_METHOD("QWidget");
    int id2 = GET_METATYPE_ID_METHOD("QWidget*");
    int id3 = GET_METATYPE_ID_METHOD("QPushButton");
    int id4 = GET_METATYPE_ID_METHOD("QPushButton*");
    int id5 = GET_METATYPE_ID_METHOD("QLineEdit*");
    QObject* obj1 = (QObject*)CREATE_METATYPE_OBJECT_METHOD(id1);
    QObject* obj2 = (QObject*)CREATE_METATYPE_OBJECT_METHOD(id2);



    REGISTER_GENERIC_LIST_PARSER(QPushButton*, loader);
    REGISTER_GENERIC_LIST_PARSER(QVariant, loader);
    REGISTER_GENERIC_LIST_PARSER(bool, loader);
    REGISTER_GENERIC_LIST_PARSER(int, loader);
    REGISTER_GENERIC_LIST_PARSER(double, loader);
    loader.registerArrayValueParser(new GenericContainerValueParser<QObject*, QObjectList>(&loader, GET_METATYPE_ID_METHOD("QObject"), QRegExp("QObjectList")));
    loader.registerArrayValueParser(new GenericVectorValueParser<QPushButton*>(&loader, GET_METATYPE_ID_METHOD("QPushButton")));
    
    QLineEdit* theLineEdit = new QLineEdit;
    theLineEdit->setObjectName("theLineEdit");
    theLineEdit->setText("DefaultText,WillBeModifiedByJsonLoaderRef");
    loader.addGlobalObject(theLineEdit);


    QString jsonFilePath = "test1.json";
    if (argc > 1) {
        jsonFilePath = a.arguments().at(1);
    }

    qDebug() << "Loading " << jsonFilePath << " ...";
    QVariant root = loader.load(jsonFilePath);

#if 1
    int labelType = GET_METATYPE_ID_METHOD("QLabel");
    //loader.setDefaultMetaType(labelType);
    QVariant labelVar = loader.load("test3.json", labelType);
    QObject* labelObj = labelVar.value<QObject*>();
    QLabel*  label    = qobject_cast<QLabel*>(labelObj);
    Q_ASSERT(label);
#endif
    
    if (root.isNull()) 
    {
        qCritical() << "Failed to load JSON from " << jsonFilePath;
        return -1;
    }
    qDebug() << "JSON loaded, root object: " << root;

    // JsonLoader的.ref关键字可以引用已经存在的对象，并在其基础上进行修改（prototype） [5/31/2016 CHENHONGHAO]
    Q_ASSERT(theLineEdit->text() == "JsonLoaderModifiedMe");

    if (obj1)
    {
        obj1->setObjectName("AGlobalObject");
        loader.addGlobalObject(obj1);
        if (loader.findObject("AGlobalObject") != obj1)
        {
            qCritical() << "Failed to add global object.";
        }
        loader.removeGlobalObject(obj1);
        if (loader.findObject("AGlobalObject") == obj1)
        {
            qCritical() << "Failed to remove global object.";
        }
    }
    else
    {
        qCritical() << "Failed to find obj1";
    }

    View* view1 = (View*)loader.findObject("testView1");
    View* view2 = loader.findObject<View*>("testView2");
    View* view3 = loader.findObject<View*>("testView3");
    View* view4 = loader.findObject<View*>("testView4");
    View* view5 = loader.findObject<View*>("testView5");
    Q_ASSERT(view5); // 测试.ref关键字与.copy关键字与.id关键字联合使用
    if (view4 && view3 && view2 && view1)
    {
        QWidget* view3Widget = view3->getWidget();
        if (!view3Widget)
        {
            qCritical() << "Failed to parse view3 widget.";
        }
        else
        {
            QString view3WidgetName = view3Widget->objectName();
            if (view3WidgetName != "儿子对象2")
            {
                qCritical() << "Failed to parse object name: " << view3WidgetName;
            }
        }

        emit view2->signal1();
        emit view2->signal2(1234);
        emit view2->signal3(true);
        emit view2->signal3();

        // 此段代码测试view2::boo2 -> view3::bool1的绑定关系
        view3->setProperty("bool1", false);
        Q_ASSERT(view2->property("bool2") == false);
        view3->setProperty("bool1", true);
        Q_ASSERT(view2->property("bool2") == true);

        QWidget* view4Widget = view4->getWidget();
        view4Widget->setGeometry(200, 200, 300, 100);
        view4Widget->show();

        qDebug() << "View3 dateTime:" << view3->m_dateTime;
        qDebug() << "View4 font:" << view4->m_font;
        qDebug() << "View4 dateTime:" << view4->m_dateTime;
        qDebug() << "View3 defaultButton.text:" << view3->m_defaultButton->text();

        // 测试this属性绑定
        Q_ASSERT(view5->property("bool1") == true);
        Q_ASSERT(view5->property("bool2") == true);

        // 测试载入QRegExp
        Q_ASSERT(view2->property("regExp").toRegExp().pattern() == ".*1");
    }


    if (!loader.createTranslationFile("jsonLoader.ts"))
    {
        qCritical() << "Failed to create TS file.";
    }

    if (!loader.translateAllStrings())
    {
        qCritical() << "Failed to translate strings.";
    }

    QList<ObjectRef> refs = loader.objectRefs();
    foreach (const ObjectRef&  ref, refs)
    {
        qDebug() << ref.object << ref.sourceObject << ref.name << ref.sourceName;
    }


    JsonScripter scripter;
    scripter.setOptions(JsonScripter::DisableQtSystemObjects);
#if 0
    scripter.addDisabledProperty(QRegExp("Q\\w+\\.\\w+"));
    scripter.addExternalObject(view1); // 使用到了
    scripter.addExternalObject(view4); // 没使用到
    scripter.addExternalObject(view5); // 没使用到
    scripter.addStandardaloneProperty(QRegExp("*.doubleList", Qt::CaseSensitive, QRegExp::Wildcard));
    scripter.addStandardaloneProperty(QRegExp("*.view", Qt::CaseSensitive, QRegExp::Wildcard));
    //scripter.addObjectRef(view3, view1, view3->objectName(), view1->objectName());
    scripter.addObjectRefs(loader.objectRefs());
    scripter.setMaxTreeDepth(10);
    if (!scripter.scriptToFile(view2, "view2.scripted.json"))
    {
        qCritical() << "Failed to save object 'view2' to JSON file.";
    }
#endif

    qDebug() << "========================================================";
    QSize sz(1, 2);
    QRect rect(3, 4, 5, 6);
    QColor color(0x80,0x40,0x20,0xff);
    QDateTime datetime = QDateTime::currentDateTime();
    QVariant szv(sz);
    QVariant rectv(rect);
    QVariant colorv(color);
    QVariant dtv(datetime);
    szv.convert(QVariant::String);
    rectv.convert(QVariant::String);
    QString szs = szv.toString();
    QString rects = rectv.toString();
    QString colors = colorv.toString();
    QString dts = dtv.toString();
    qDebug() << "========================================================";

    // 如果不需要进行profile，则在此处直接返回即可 [5/9/2016 CHENHONGHAO]
    return a.exec();

    REGISTER_METATYPE_X(Field, "Field");
    REGISTER_METATYPE_X(Field, "PacketField");
    REGISTER_METATYPE_X(Field, "PacketItemIndexField");
    REGISTER_METATYPE_X(Field, "VirtualField");
    REGISTER_METATYPE_X(Field, "SymlinkField");
    REGISTER_METATYPE_X(Field, "IntegerField");
    REGISTER_METATYPE_X(Field, "RealField");
    REGISTER_METATYPE_X(Field, "VoltageField");
    REGISTER_METATYPE_X(Field, "AmpereField");
    REGISTER_METATYPE_X(Field, "AngleField");
    REGISTER_METATYPE_X(Field, "PercentageField");
    REGISTER_METATYPE_X(Field, "HarmonicListField");
    REGISTER_METATYPE_X(Field, "FrequencyField");
    REGISTER_METATYPE_X(Field, "WattField");
    REGISTER_METATYPE_X(Field, "VAField");
    REGISTER_METATYPE_X(Field, "VarField");
    REGISTER_METATYPE_X(Field, "HcRmsField");
    REGISTER_METATYPE_X(Field, "EventListField");
    REGISTER_METATYPE_X(Field, "DurationField");
    REGISTER_METATYPE_X(Field, "WaveField");

    const char* n1044 = QMetaType::typeName(1044);
    const char* n1045 = QMetaType::typeName(1045);
    const char* n1046 = QMetaType::typeName(1046);
    const char* n1047 = QMetaType::typeName(1047);
    const char* n1048 = QMetaType::typeName(1048);
    const char* n1049 = QMetaType::typeName(1049);

    loader.registerArrayValueParser(new GenericListValueParser<Field*>(&loader, QMetaType::type("Field*")));

    Field* externalField = new Field;
    Field* externalLinkField = new Field;
    externalField->setObjectName("externalField");
    externalLinkField->setObjectName("externalLinkField");
    externalField->setLink(externalLinkField);
    loader.addGlobalObject(externalField);
    QVariant rootFieldVar = loader.load("test4.json");
    Field* rootFieldObj = qobject_cast<Field*>(rootFieldVar.value<QObject*>());
    Q_ASSERT(rootFieldObj);
    Q_ASSERT(rootFieldObj->children()[0]->link() == rootFieldObj->children()[1]);
    Q_ASSERT(rootFieldObj->children()[1]->link() == rootFieldObj->children()[0]);
    scripter.setMaxTreeDepth(10);
    scripter.addDisabledProperty(QRegExp("Field\\.(name|channelMask|min|max|dimension|digitals|linkPath|size|valid|maxHarmonicOrder|harmonicType|isInterHarmonic|isMoniterType)"));
    scripter.addExternalObject(externalField);
    scripter.scriptToFile(rootFieldObj, "root.scripted.json");

    return a.exec();

    //loader.setDefaultMetaTypeHint(QMetaType::UnknownType);

    QDateTime t1 = QDateTime::currentDateTime();
    for (int i = 0; i < 1000; i++)
    {
        loader.load("packet.json");
    }
    QDateTime t2 = QDateTime::currentDateTime();
    qint64 elapsed = t1.msecsTo(t2);
    qDebug() << "Average load time: " << elapsed << "us.";

    return 0;
}
