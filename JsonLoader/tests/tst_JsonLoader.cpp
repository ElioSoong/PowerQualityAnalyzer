#include "JsonLoader.h"
#include "Test.h"

#include <gtest/gtest.h>
#include <QtTest>
#include <QDomDocument>

struct _JsonLoader : JsonLoader
{

};

class tst_JsonLoader : public ::testing::Test
{
protected:
    virtual void SetUp() override
    {
        REGISTER_METATYPE(GeneralTestClass);

        QFileInfo thisFilePath(__FILE__);
        QString thisFileDir(thisFilePath.absoluteDir().absolutePath());
        QDir::setCurrent(thisFileDir);
    }


    virtual void TearDown() override
    {
        // TODO: NOT IMPLEMENTED YET
    }
};

#define LOAD_FIELD_CLASS(_loader, _file, _result) {                                              \
    QString errors;                                                                                     \
    _result = load<Field*>((_loader), (_file), &errors);                                     \
    QByteArray errorsBuffer = errors.toLocal8Bit();                                                     \
    const char* errorsString = errorsBuffer.constData();                                                \
    ASSERT_STREQ(errorsString, "");                                                                     \
    ASSERT_TRUE((_result) != NULL);                                                                     \
}

#define LOAD_GENERAL_TEST_CLASS(_loader, _file, _result) {                                              \
    QString errors;                                                                                     \
    _result = load<GeneralTestClass*>((_loader), (_file), &errors);                                     \
    QByteArray errorsBuffer = errors.toLocal8Bit();                                                     \
    const char* errorsString = errorsBuffer.constData();                                                \
    ASSERT_STREQ(errorsString, "");                                                                     \
    ASSERT_TRUE((_result) != NULL);                                                                     \
}


TEST_F(tst_JsonLoader, benchmark)
{
    JsonLoader loader;

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

    loader.registerArrayValueParser(new GenericListValueParser<Field*>(&loader, QMetaType::type("Field*")));

    QDateTime t1 = QDateTime::currentDateTime();
    for (int i = 0; i < 10; i++)
    {
        loader.load("tests/packet.json");
    }
    QDateTime t2 = QDateTime::currentDateTime();
    qint64 elapsed = t1.msecsTo(t2);
    qDebug() << "Average load time: " << elapsed << "us.";
}

TEST_F(tst_JsonLoader, addGlobalObject)
{
    JsonLoader loader;
    QWidget* widget = new QWidget();
    widget->setObjectName("!@#$%theGlobalObject^&*()");
    bool result = loader.addGlobalObject(widget);
    ASSERT_TRUE(result);
    QObject* resultObject = loader.findObject("!@#$%theGlobalObject^&*()");
    ASSERT_EQ(resultObject, widget);

    bool dumplicateResult = loader.addGlobalObject(widget);
    // 一个key下面允许放置多个相同的QObject，
    // 例如在使用了".ref"关键字的多个array元素有可能是完全相同的qObject [12/30/2016 CHENHONGHAO]
    // 在“global”下不允许放置相同的QObject
    ASSERT_TRUE(!dumplicateResult);

    widget->setObjectName("anotherGlobalObject");
    bool changeNameResult = loader.addGlobalObject(widget);
    ASSERT_TRUE(changeNameResult);

    bool removeResult = loader.removeGlobalObject(widget);
    ASSERT_TRUE(removeResult);
    QObject* resultObjectAfterRemove = loader.findObject("theGlobalObject");
    ASSERT_TRUE(resultObjectAfterRemove == NULL);
}

TEST_F(tst_JsonLoader, loadBasicNumericalMetatypes)
{
    JsonLoader loader;
    GeneralTestClass* result = NULL;

    // 目前不支持void*，这样也没什么意义，有类似需求时可以尝试QObject* [12/20/2016 CHENHONGHAO]
    //ASSERT_EQ(result->m_voidPtr, (void*)0x12345678);
#define ASSERTS()   \
    ASSERT_TRUE(result != NULL);    \
    ASSERT_EQ(result->m_int, 1);    \
    ASSERT_EQ(result->m_uint, 2);   \
    ASSERT_EQ(result->m_short, 3);  \
    ASSERT_EQ(result->m_ushort, 4); \
    ASSERT_EQ(result->m_char, 5);   \
    ASSERT_EQ(result->m_uchar, 6);  \
    ASSERT_EQ(result->m_float, 7);  \
    ASSERT_EQ(result->m_double, 8); \

    {
        LOAD_GENERAL_TEST_CLASS(loader, "tests/BasicNumerical.json", result);
        ASSERTS();
    }
    {
        LOAD_GENERAL_TEST_CLASS(loader, "tests/BasicNumericalFromString.json", result);
        ASSERTS();
    }
}

TEST_F(tst_JsonLoader, loadQString)
{
    JsonLoader loader;
    GeneralTestClass* result = NULL;

    LOAD_GENERAL_TEST_CLASS(loader, "tests/QStringContainer.json", result);
    ASSERT_EQ(result->m_string, "goodbye world");
}

TEST_F(tst_JsonLoader, loadQObject)
{
    JsonLoader loader;
    GeneralTestClass* result = NULL;

    LOAD_GENERAL_TEST_CLASS(loader, "tests/QObjectContainer.json", result);
    ASSERT_EQ(result->objectName(), "inno");
}

TEST_F(tst_JsonLoader, loadCustomObject)
{
    JsonLoader loader;
    GeneralTestClass* result = NULL;

    LOAD_GENERAL_TEST_CLASS(loader, "tests/QObjectContainer.json", result);
    ASSERT_EQ(result->m_customObject.m_value, "inno");
}

TEST_F(tst_JsonLoader, basicQVariantTests)
{
    QSize sz(1, 2);
    QRect rect(3, 4, 5, 6);
    QColor color(0x80, 0x40, 0x20, 0xff);
    QDateTime datetime(QDate(2016,1,2), QTime(3,4,5,6));
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

    ASSERT_EQ(szs, QString(""));     // QVariant不支持QSize
    ASSERT_EQ(rects, QString(""));   // QVariant不支持QRect
    ASSERT_EQ(colors, QString("#804020"));
    ASSERT_EQ(dts, QString("2016-01-02T03:04:05"));
}

TEST_F(tst_JsonLoader, loadQVariant)
{
    JsonLoader loader;
    GeneralTestClass* result = NULL;

    LOAD_GENERAL_TEST_CLASS(loader, "tests/VariantContainer.json", result);
    ASSERT_EQ(result->m_stringVariant, "hi");
    ASSERT_EQ(result->m_intVariant, 123);
}

TEST_F(tst_JsonLoader, loadQStringList)
{
    JsonLoader loader;
    GeneralTestClass* result = NULL;

    LOAD_GENERAL_TEST_CLASS(loader, "tests/StringListContainer.json", result);
    ASSERT_EQ(result->m_stringList.at(0), "hi");
    ASSERT_EQ(result->m_stringList.at(1), "hello");
    ASSERT_EQ(result->m_stringList.at(2), "hehe");
}

TEST_F(tst_JsonLoader, loadDateTime)
{
    JsonLoader loader;
    GeneralTestClass* result = NULL;

    LOAD_GENERAL_TEST_CLASS(loader, "tests/DateTimeContainer.json", result);
    QDateTime dt = result->m_dateTime;
    ASSERT_EQ(dt.toMSecsSinceEpoch(), QDateTime(QDate(2016, 1, 2), QTime(3, 4, 5)).toMSecsSinceEpoch());
}

TEST_F(tst_JsonLoader, loadFont)
{
    JsonLoader loader;
    GeneralTestClass* result = NULL;

    LOAD_GENERAL_TEST_CLASS(loader, "tests/FontContainer.json", result);
    QFont f = result->m_font;
    ASSERT_EQ(f.family(), "Microsoft Yahei");
    ASSERT_EQ(f.pixelSize(), 36);
    ASSERT_EQ(f.pointSize(), -1);
    ASSERT_EQ(f.bold(), true);
    ASSERT_EQ(f.italic(), true);
}

TEST_F(tst_JsonLoader, loadRegExp)
{
    JsonLoader loader;
    GeneralTestClass* result = NULL;

    LOAD_GENERAL_TEST_CLASS(loader, "tests/RegExp.json", result);

    QRegExp regExp = result->m_regExp;
    QString pattern = regExp.pattern();
    ASSERT_EQ(pattern, QString("^\\s*.1$"));
}

TEST_F(tst_JsonLoader, loadEnumWithEnumValue)
{
    // 来自M05项目的使用场景：QVariant类型的属性，Enum常量，要求赋值为int即可
    JsonLoader loader;
    GeneralTestClass* result = NULL;

    LOAD_GENERAL_TEST_CLASS(loader, "tests/LoadEnumWithEnumValue.json", result);

    ASSERT_EQ(result->m_enum, GeneralTestClass::EnumValue02);
    ASSERT_EQ(result->m_flag, GeneralTestClass::EnumValue01 | GeneralTestClass::EnumValue02);
}

TEST_F(tst_JsonLoader, loadIntWithEnumValue)
{
    // 来自M05项目的使用场景：QVariant类型的属性，Enum常量，要求赋值为int即可
    JsonLoader loader;
    GeneralTestClass* result = NULL;

    LOAD_GENERAL_TEST_CLASS(loader, "tests/LoadIntWithEnumValue.json", result);

    ASSERT_EQ(result->m_int, GeneralTestClass::EnumValue01 | GeneralTestClass::EnumValue02);
    ASSERT_EQ(result->m_uint, GeneralTestClass::EnumValue01 | GeneralTestClass::EnumValue02);
}

TEST_F(tst_JsonLoader, loadStringWithEnumValue)
{
    // 来自M05项目的使用场景：QVariant类型的属性，Enum常量，要求赋值为int即可
    JsonLoader loader;
    GeneralTestClass* result = NULL;

    LOAD_GENERAL_TEST_CLASS(loader, "tests/LoadStringWithEnumValue.json", result);

    ASSERT_EQ(result->m_string, QString("GeneralTestClass.EnumValue01 | GeneralTestClass.EnumValue02"));
}

TEST_F(tst_JsonLoader, loadVariantWithEnumValue)
{
    // 来自M05项目的使用场景：QVariant类型的属性，Enum常量，要求赋值为int即可
    JsonLoader loader;
    GeneralTestClass* result = NULL;

    LOAD_GENERAL_TEST_CLASS(loader, "tests/LoadVariantWithEnumValue.json", result);

    QVariant variant = result->m_variant;
    int type = variant.userType();
    ASSERT_NE(type, QMetaType::UnknownType);
    ASSERT_NE(type, QMetaType::QString);
    ASSERT_EQ(variant.toInt(), GeneralTestClass::EnumValue01 | GeneralTestClass::EnumValue02);
}

TEST_F(tst_JsonLoader, connectSignalSlot)
{
    // "mySignal" : "otherObject.mySlot"
    // "mySlot": "other.mySignal"
    // "mySignal": "other.mySignal"

    // 测试参数的自适应
    // mySignal(int param = 0):
    // mySignal(int)
    // mySignal()

    // "mySignal(int)": "other.mySlot(int)"
    // "mySignal()": "other.mySlot()"

    // ".connection"
    //ASSERT_TRUE(false);
    JsonLoader loader;
    REGISTER_METATYPE(ConnectSignalSlotTest);
    loader.load("tests/ConnectSignalSlot.json");

    // "mySignal" : "otherObject.mySlot"
    QObject* o = loader.findObject("obj1");
    ConnectSignalSlotTest* obj1 = qobject_cast<ConnectSignalSlotTest*>(o);
    o = loader.findObject("obj2");
    ConnectSignalSlotTest* obj2 = qobject_cast<ConnectSignalSlotTest*>(o);
    ASSERT_TRUE(obj1 != NULL);
    ASSERT_TRUE(obj2 != NULL);
    ASSERT_EQ(obj1->m_value, 0);
    ASSERT_EQ(obj2->m_value, 0);
    emit obj1->mysignal();
    ASSERT_EQ(obj1->m_value, 0);
    ASSERT_EQ(obj2->m_value, 1);

    // "mySlot": "other.mySignal"
    o = loader.findObject("obj3");
    ConnectSignalSlotTest* obj3 = qobject_cast<ConnectSignalSlotTest*>(o);
    o = loader.findObject("obj4");
    ConnectSignalSlotTest* obj4 = qobject_cast<ConnectSignalSlotTest*>(o);
    ASSERT_TRUE(obj3 != NULL);
    ASSERT_TRUE(obj4 != NULL);
    ASSERT_EQ(obj3->m_value, 0);
    ASSERT_EQ(obj4->m_value, 0);
    emit obj4->mysignal();
    ASSERT_EQ(obj3->m_value, 1);
    ASSERT_EQ(obj4->m_value, 0);

    // "mySignal(int)": "other.mySlot(int)"
    o = loader.findObject("obj5");
    ConnectSignalSlotTest* obj5 = qobject_cast<ConnectSignalSlotTest*>(o);
    o = loader.findObject("obj6");
    ConnectSignalSlotTest* obj6 = qobject_cast<ConnectSignalSlotTest*>(o);
    ASSERT_TRUE(obj5 != NULL);
    ASSERT_TRUE(obj6 != NULL);
    ASSERT_EQ(obj5->m_value, 0);
    ASSERT_EQ(obj6->m_value, 0);
    emit obj5->othersignal(3);
    ASSERT_EQ(obj5->m_value, 0);
    ASSERT_EQ(obj6->m_value, 3);

    // "mySignal()": "other.mySlot()"
    o = loader.findObject("obj7");
    ConnectSignalSlotTest* obj7 = qobject_cast<ConnectSignalSlotTest*>(o);
    o = loader.findObject("obj8");
    ConnectSignalSlotTest* obj8 = qobject_cast<ConnectSignalSlotTest*>(o);
    ASSERT_TRUE(obj7 != NULL);
    ASSERT_TRUE(obj8 != NULL);
    ASSERT_EQ(obj7->m_value, 0);
    ASSERT_EQ(obj8->m_value, 0);
    emit obj7->othersignal();
    ASSERT_EQ(obj7->m_value, 0);
    ASSERT_EQ(obj8->m_value, 1);
    
    // "mySignal": "other.mySignal"
    o = loader.findObject("obj9");
    ConnectSignalSlotTest* obj9 = qobject_cast<ConnectSignalSlotTest*>(o);
    o = loader.findObject("obj10");
    ConnectSignalSlotTest* obj10 = qobject_cast<ConnectSignalSlotTest*>(o);
    ASSERT_TRUE(obj9 != NULL);
    ASSERT_TRUE(obj10 != NULL);
    QSignalSpy spy(obj10, SIGNAL(mysignal()));
    ASSERT_EQ(obj9->m_value, 0);
    ASSERT_EQ(obj10->m_value, 0);
    emit obj9->mysignal();
    ASSERT_EQ(spy.count(), 1);
}

TEST_F(tst_JsonLoader, bindProperty)
{
    // "myProperty": "otherObjectA.myPropertyObject.myProperty"

    // notify/non-notify (QSignalSpy)

    JsonLoader loader;
    REGISTER_METATYPE(BindPropertyTest);
    loader.load("tests/BindProperty.json");
    BindPropertyTest* obj1 = qobject_cast<BindPropertyTest*>(loader.findObject("obj1"));
    BindPropertyTest* obj2 = qobject_cast<BindPropertyTest*>(loader.findObject("obj2"));
    ASSERT_TRUE(obj1 != NULL);
    ASSERT_TRUE(obj2 != NULL);
    QSignalSpy spy(obj1, SIGNAL(notifySignal()));
    obj2->setProperty("property", "inno");
    ASSERT_EQ(spy.count(), 1);
}

TEST_F(tst_JsonLoader, createTranslationFile)
{
    JsonLoader loader;
    REGISTER_METATYPE(TranslatableLabel);
    qRegisterMetaType<TranslatableLabel>("TranslatableLabel");
    loader.load("tests/TranslationTest.json");
    bool isOk = loader.createTranslationFile("translationFile.ts");
    ASSERT_TRUE(isOk);

    QDomDocument doc;
    QFile file("translationFile.ts");
    isOk = file.open(QIODevice::ReadOnly);
    ASSERT_TRUE(isOk);
    isOk = doc.setContent(&file);
    // 是否是可以被正确解析的xml文件
    ASSERT_TRUE(isOk);
    file.close();
    QByteArray xmlData = doc.toByteArray();
    // 是否包含所有要被翻译的内容
    ASSERT_TRUE(xmlData.contains("inno"));
    ASSERT_TRUE(xmlData.contains("一诺"));
}

TEST_F(tst_JsonLoader, translateStrings)
{
    // QPushButton.text = "中文"
    // QWidget.title = "`tr`English"

    // qApp->setTranslator
    // loader.translateAllStrings()
    // @F280 FieldModel().reset();
    // ASSERT()
    JsonLoader loader;
    REGISTER_METATYPE(TranslatableLabel);
    qRegisterMetaType<TranslatableLabel>("TranslatableLabel");
    loader.load("tests/TranslationTest.json");
    bool isOk = loader.createTranslationFile("tests/translationFile.ts");
    ASSERT_TRUE(isOk);
    QLabel* obj = qobject_cast<QLabel*>(loader.findObject("label0"));
    ASSERT_TRUE(obj != NULL);
    // 未安装翻译
    ASSERT_TRUE(obj->text() == "inno");

    QTranslator translator;
    isOk = translator.load("tests/translationFile");
    ASSERT_TRUE(isOk);
    qApp->installTranslator(&translator);
    // 已安装翻译
    loader.translateAllStrings();
    ASSERT_TRUE(obj->text() == "一诺");
}

TEST_F(tst_JsonLoader, refExternalObject)
{
    // ".ref": "myExternalObject" （.ref用来替换.id和.type）
    // ".ref": "myExternalObject.myProperty"

    // TODO: 测试对象引用以及引用对象的属性修改
    // ".copy": true/false （必须有Copy Constructor)

    JsonLoader loader;
    REGISTER_METATYPE(RefExternalObjectTest);
    // 如果需要保证.copy关键字可以正常使用，则必须在Qt原生的Meta-System中注册当前类型，
    // 因为当使用了自定义的对象工厂时(ENABLE_CUSTOM_OBJECT_FACTORY)，拷贝构造函数无法使用。
    // [12/30/2016 CHENHONGHAO]
    qRegisterMetaType<RefExternalObjectTest>("RefExternalObjectTest");
    // 外部对象obj1
    RefExternalObjectTest* obj1 = new RefExternalObjectTest();
    obj1->setObjectName("inno1");
    loader.addGlobalObject(obj1);
    ASSERT_EQ(obj1->objectName(), "inno1");
    // 外部对象obj2
    RefExternalObjectTest* obj2 = new RefExternalObjectTest();
    obj2->setObjectName("inno2");
    loader.addGlobalObject(obj2);
    ASSERT_EQ(obj2->objectName(), "inno2");

    loader.load("tests/RefExternalObject.json");

    // ref
    auto ref = qobject_cast<RefExternalObjectTest*>(loader.findObject("inno1_sub"));
    ASSERT_EQ(ref, obj1);
    ASSERT_TRUE(obj1->objectName() == "inno1_sub");
    ASSERT_TRUE(ref->objectName() == "inno1_sub");
    ref->m_value = 123;
    ASSERT_EQ(obj1->m_value, 123);
    ASSERT_EQ(ref->m_value, 123);

    // copy
    auto copy = qobject_cast<RefExternalObjectTest*>(loader.findObject("inno2_sub"));
    ASSERT_TRUE(copy != obj2);
    ASSERT_TRUE(obj2->objectName() == "inno2");
    ASSERT_TRUE(copy->objectName() == "inno2_sub");
    copy->m_value = 123;
    ASSERT_EQ(obj2->m_value, 0);
    ASSERT_EQ(copy->m_value, 123);
}

TEST_F(tst_JsonLoader, getObjectRefs)
{
    // JsonLoader.objectRefs();
    // QList<ObjectRef>
    // objectRef.name/sourceName/object/sourceObject/overridedProperties
    JsonLoader loader;
    REGISTER_METATYPE(GetObjectRefsTest);
    qRegisterMetaType<GetObjectRefsTest>("GetObjectRefsTest");
    loader.load("tests/GetObjectRefs.json");
    GetObjectRefsTest* obj = qobject_cast<GetObjectRefsTest*>(loader.findObject("obj"));
    ASSERT_TRUE(obj != NULL);

    const QList<ObjectRef> refs = loader.objectRefs();
    ASSERT_EQ(refs.size(), 2);
    // copy == true
    ObjectRef ref0 = refs.at(0);
    ASSERT_EQ(ref0.name, "obj_copy");
    ASSERT_EQ(ref0.sourceName, "obj");
    ASSERT_TRUE(ref0.object != ref0.sourceObject);
    ASSERT_EQ(ref0.overridedProperties.size(), 2);
    ASSERT_EQ(ref0.overridedProperties.at(0), "size");
    ASSERT_EQ(ref0.overridedProperties.at(1), "title");
    // copy == false
    ObjectRef ref1 = refs.at(1);
    ASSERT_EQ(ref1.name, "obj");
    ASSERT_EQ(ref1.sourceName, "obj");
    ASSERT_TRUE(ref1.object == ref1.sourceObject);
    ASSERT_EQ(ref1.overridedProperties.size(), 1);
    ASSERT_EQ(ref1.overridedProperties.at(0), "size");
}

TEST_F(tst_JsonLoader, keyParsersKeepInOrder)
{
    // 此处不必测试了，因为以下原本用QHash的容器，已经改成了QList
    //JsonLoader loader;
    //loader.m_arrayValueParsers;
    //loader.m_stringValueParsers;
    SUCCEED();
}

TEST_F(tst_JsonLoader, arrayValuleParsersKeepInOrder)
{
    // 此处不必测试了，因为以下原本用QHash的容器，已经改成了QList
    //JsonLoader loader;
    //loader.m_arrayValueParsers;
    //loader.m_stringValueParsers;
    SUCCEED();
}

TEST_F(tst_JsonLoader, nestedFileTest)
{
    JsonLoader loader;
    REGISTER_METATYPE(TranslatableLabel);
    qRegisterMetaType<TranslatableLabel>("TranslatableLabel");
    loader.load("tests/NestedFile.json");
    QLabel* obj0 = qobject_cast<QLabel*>(loader.findObject("obj0"));
    ASSERT_TRUE(obj0 != NULL);
    QLabel* obj1 = qobject_cast<QLabel*>(loader.findObject("obj1"));
    ASSERT_TRUE(obj1 != NULL);
}

TEST_F(tst_JsonLoader, propertyAppendValueTest)
{

    JsonLoader loader;
    REGISTER_GENERIC_LIST_PARSER(int, loader);
    //loader.registerArrayValueParser(new GenericListValueParser<int>(&loader, QMetaType::Int));
    REGISTER_METATYPE(PropertyAppendValue);
    qRegisterMetaType<PropertyAppendValue>("PropertyAppendValue");
    REGISTER_METATYPE(PropertyClass);
    qRegisterMetaType<PropertyClass>("PropertyClass");
    REGISTER_GENERIC_LIST_PARSER(PropertyClass*, loader);
    loader.load("tests/PropertyAppendValue.json");
    PropertyAppendValue* obj = qobject_cast<PropertyAppendValue*>(loader.findObject("obj"));
    ASSERT_TRUE(obj != NULL);

    // 嵌套的json文件在前
    ASSERT_TRUE(obj->m_list.size() == 4);
    ASSERT_TRUE(!obj->m_list.contains("tests/StringList.json"));

    // 嵌套的json文件在后
    ASSERT_TRUE(obj->m_otherList.size() == 5);
    ASSERT_TRUE(!obj->m_otherList.contains("tests/StringList.json"));

    // int类型数组元素测试
    ASSERT_TRUE(obj->m_intList.size() == 7);
    ASSERT_TRUE(obj->m_intList.contains(123));
    ASSERT_TRUE(obj->m_intList.contains(987));

    // 自定义数据类型
    ASSERT_TRUE(obj->m_propertyList.size() == 5);
}

TEST_F(tst_JsonLoader, loadSameJsonTwiceTest)
{
    JsonLoader loader;
    REGISTER_METATYPE(Field);
    qRegisterMetaType<Field>("Field");
    REGISTER_GENERIC_LIST_PARSER(Field*, loader);
    loader.load("tests/loadObjectTwice.json");
    Field* obj = qobject_cast<Field*>(loader.findObject("root"));
    ASSERT_TRUE(obj != NULL);
    Field* child = qobject_cast<Field*>(loader.findObject("1-1"));
    ASSERT_TRUE(child != NULL);
    //loader.removeObject("root");
    // delete
    delete obj;
    // reload
    loader.load("tests/loadObjectTwice.json");
    child = qobject_cast<Field*>(loader.findObject("1-1"));
    ASSERT_TRUE(child != NULL);
}

TEST_F(tst_JsonLoader, refAllTest)
{
    JsonLoader loader;
    REGISTER_METATYPE(Field);
    qRegisterMetaType<Field>("Field");
    REGISTER_GENERIC_LIST_PARSER(Field*, loader);
    Field* field = new Field;
    field->setObjectName("first");
    loader.addGlobalObject(field);
    Field* field1 = new Field;
    field1->setObjectName("second");
    loader.addGlobalObject(field1);

    Field* obj1 = qobject_cast<Field*>(loader.findObject("first"));
    ASSERT_TRUE(obj1 != NULL);
    Field* obj2 = qobject_cast<Field*>(loader.findObject("second"));
    ASSERT_TRUE(obj2 != NULL);

    //loader.load("tests/allRef.json");
    Field* result = NULL;
    LOAD_FIELD_CLASS(loader, "tests/allRef.json", result);

    obj1 = qobject_cast<Field*>(loader.findObject("first"));
    ASSERT_TRUE(obj1 != NULL);
    obj2 = qobject_cast<Field*>(loader.findObject("second"));
    ASSERT_TRUE(obj2 != NULL);
}

TEST_F(tst_JsonLoader, globalObjectParentTest)
{
    JsonLoader loader;
    REGISTER_METATYPE(Field);
    qRegisterMetaType<Field>("Field");
    Field* parent = new Field;
    parent->setObjectName("parent");
    loader.addGlobalObject(parent);
    ASSERT_TRUE(parent->children().count() == 0);
    // add a child
    Field* child1 = new Field;
    child1->setObjectName("child1");
    child1->setParent(parent);
    ASSERT_TRUE(parent->QObject::children().count() == 1);
    ASSERT_TRUE(child1->parent()->objectName() == "parent");
    loader.addGlobalObject(child1);
    ASSERT_TRUE(parent->QObject::children().count() == 1);
    ASSERT_TRUE(child1->parent()->objectName() == "parent");
    // add a child
    Field* child2 = new Field;
    child2->setObjectName("child2");
    child2->setParent(parent);
    ASSERT_TRUE(parent->QObject::children().count() == 2);
    ASSERT_TRUE(child2->parent()->objectName() == "parent");
    loader.addGlobalObject(child2);
    ASSERT_TRUE(parent->QObject::children().count() == 2);
    ASSERT_TRUE(child2->parent()->objectName() == "parent");
}

// 测试.objects
TEST_F(tst_JsonLoader, objectsTest)
{
    JsonLoader loader;
    REGISTER_METATYPE(Field);
    qRegisterMetaType<Field>("Field");
    REGISTER_GENERIC_LIST_PARSER(Field*, loader);
    loader.load("tests/objectsKey.json");
    Field* obj = qobject_cast<Field*>(loader.findObject("root"));
    ASSERT_TRUE(obj != NULL);
    ASSERT_TRUE(obj->link() != NULL);
    Field* obj1 = qobject_cast<Field*>(loader.findObject("sharedObject1"));
    ASSERT_TRUE(obj1 != NULL);
    Field* obj2 = qobject_cast<Field*>(loader.findObject("sharedObject2"));
    ASSERT_TRUE(obj2 != NULL);
    Field* obj3 = qobject_cast<Field*>(loader.findObject("sharedObject3"));
    ASSERT_TRUE(obj3 != NULL);
    Field* obj4 = qobject_cast<Field*>(loader.findObject("sharedObject4"));
    ASSERT_TRUE(obj4 != NULL);
}

// 测试：属性的值用json文件来赋值的情况
TEST_F(tst_JsonLoader, jsonToProperty)
{
    JsonLoader loader;
    REGISTER_METATYPE(Field);
    loader.load("tests/jsonToProperty.json");
    
    Field* obj = qobject_cast<Field*>(loader.findObject("obj"));
    ASSERT_TRUE(obj != NULL);
    ASSERT_TRUE(obj->link() != NULL);
}

// 测试：属性的类型是QVariant类型，且想要赋给它的值是某个对象的属性（“obj.pro”）时的情况
TEST_F(tst_JsonLoader, qvariantPropertyTest)
{
    JsonLoader loader;
    REGISTER_METATYPE(QVariantProperty);
    QVariantProperty* outer = new QVariantProperty;
    outer->m_prop = QVariant(12345);
    outer->setObjectName("outer");
    loader.addGlobalObject(outer);
    loader.load("tests/qvariantStyleProperty.json");

    QVariantProperty* obj = qobject_cast<QVariantProperty*>(loader.findObject("obj"));
    ASSERT_TRUE(obj != NULL);
    int propValue = obj->m_prop.toInt();
    ASSERT_TRUE(12345 == propValue);
}

// 测试：将对象ID作为属性值是否可以正确赋值
TEST_F(tst_JsonLoader, objectIdToPropertyTest)
{
    JsonLoader loader;
    REGISTER_METATYPE(Field);
    loader.load("tests/ObjectIdToProperty.json");

    Field* obj = qobject_cast<Field*>(loader.findObject("obj"));
    ASSERT_TRUE(obj != NULL);
    ASSERT_TRUE(obj->link() != NULL);
    ASSERT_TRUE(obj == obj->link());
}

TEST_F(tst_JsonLoader, fullBindTest)
{
    JsonLoader loader;
    REGISTER_METATYPE(BindPropertyTest);
    BindPropertyTest* outer = new BindPropertyTest;
    outer->setObjectName("outer");
    loader.addGlobalObject(outer);
    loader.load("tests/fullBind.json");
    BindPropertyTest* obj = qobject_cast<BindPropertyTest*>(loader.findObject("obj"));
    ASSERT_TRUE(obj != NULL);

    obj->setProperty("property", "inno");
    Q_ASSERT(obj->m_property == "inno");
    Q_ASSERT(outer->m_property == "inno");

    outer->setProperty("property", "yujia");
    Q_ASSERT(obj->m_property == "yujia");
    Q_ASSERT(outer->m_property == "yujia");
}

// 测试：jsonLoader提供的属性全绑定的静态函数
TEST_F(tst_JsonLoader, fullBindWrapperTest)
{
    // 此测试会阻塞其他测试用例，故注释掉，可打开单独运行
#if 0
    QWidget w;
    QLineEdit l1(&w);
    QLineEdit l2(&w);
    l1.setText("inno");
    l1.move(100, 50);
    l2.move(100, 100);
    w.resize(300, 200);

    ASSERT_TRUE(PropertyBinding::fullBind(&l1, "text", &l2, "text"));

    w.show();
    QApplication::instance()->exec();
#endif
}

// 测试：格式为：".ref": "tests/keyValueObject1.json:obj1"能否被正确解析
TEST_F(tst_JsonLoader, keyValueObjectTest)
{
    JsonLoader loader;
    REGISTER_METATYPE(Field);
    loader.load("tests/keyValueObject1.json");
    Field* obj1 = qobject_cast<Field*>(loader.findObject("obj1"));
    ASSERT_TRUE(obj1 != NULL);

    loader.load("tests/keyValueObject2.json");
    Field* obj2 = qobject_cast<Field*>(loader.findObject("obj2"));
    ASSERT_TRUE(obj2 != NULL);
    Q_ASSERT(obj2 == obj1);

    loader.load("tests/keyValueObject3.json");
    Field* obj3 = qobject_cast<Field*>(loader.findObject("obj3"));
    ASSERT_TRUE(obj3 != NULL);

    loader.load("tests/keyValueObject4.json");
    Field* obj4 = qobject_cast<Field*>(loader.findObject("obj4"));
    ASSERT_TRUE(obj4 != NULL);
    ASSERT_TRUE(obj4->m_link == obj3->m_link);
}