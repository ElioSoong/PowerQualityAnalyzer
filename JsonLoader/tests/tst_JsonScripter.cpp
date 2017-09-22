#include "JsonScripter.h"
#include "JsonLoader.h"
#include "Test.h"

#include <gtest/gtest.h>
#include <QtTest>

#include <QtWidgets/QWidget>
#include <QtWidgets/QAction>
#include <QPluginLoader>

struct _JsonScripter : JsonScripter
{
    using JsonScripter::findAllObjects;
    using JsonScripter::enumToString;
    using JsonScripter::variantToList;
};

class tst_JsonScripter : public ::testing::Test
{
protected:
    virtual void SetUp() override
    {
        // TODO: NOT IMPLEMENTED YET
    }


    virtual void TearDown() override
    {
        // TODO: NOT IMPLEMENTED YET
    }

    bool compareField(Field* fielda, Field* fieldb)
    {
        if (!fielda || !fieldb)
            return fielda == fieldb ? true : false;

        if (fielda->objectName() != fieldb->objectName())
            return false;

        return true;
    }

    bool compareFieldTree(Field* roota, Field* rootb)
    {
        if (!compareField(roota, rootb))
            return false;

        Field* linka = roota->link();
        Field* linkb = rootb->link();
        // 目前如果link也递归比较，则会导致无限递归，因此不敢递归，但实际应该递归 [12/29/2016 CHENHONGHAO]
        if (!compareField(linka, linkb))
            return false;

        QList<Field*> childrena = roota->children();
        QList<Field*> childrenb = rootb->children();
        if (childrena.count() != childrenb.count())
            return false;
        for (int i = 0; i < childrena.count(); ++i)
        {
            if (!compareFieldTree(childrena[i], childrenb[i]))
                return false;
        }

        return true;
    }
};

TEST_F(tst_JsonScripter, DISABLED_benchmark)
{
    ASSERT_TRUE(false);
}

TEST_F(tst_JsonScripter, setMaxTreeDepth)
{
    JsonLoader loader;
    REGISTER_METATYPE(NestedWidget);
    qRegisterMetaType<NestedWidget>("NestedWidget");
    REGISTER_METATYPE(LastWidget);
    qRegisterMetaType<LastWidget>("LastWidget");
    QString errors;
    QWidget* root = load<QWidget*>(loader, "tests/MaxTreeDepthTest.json", &errors);
    ASSERT_TRUE(root != 0);
    _JsonScripter scripter;
    scripter.setMaxTreeDepth(9);
    ASSERT_TRUE(scripter.scriptToFile(root, "tests/setMaxTreeDepth.json"));

    QFile file("tests/setMaxTreeDepth.json");
    bool isOk = file.open(QIODevice::ReadOnly);
    ASSERT_TRUE(isOk);
    QByteArray data = file.readAll();
    file.close();
    ASSERT_TRUE(data.contains("level9"));
    ASSERT_TRUE(!data.contains("level10"));
}

TEST_F(tst_JsonScripter, addDisabledProperty)
{
    JsonLoader loader;
    REGISTER_METATYPE(CustomWidget);
    qRegisterMetaType<CustomWidget>("CustomWidget");
    QString errors;
    QWidget* root = load<QWidget*>(loader, "tests/AddDisabledProperty.json", &errors);
    ASSERT_TRUE(root != 0);
    _JsonScripter scripter;
    scripter.addDisabledProperty(QRegExp("C\\w+\\.m\\w+"));
    scripter.addDisabledProperty(QRegExp("C\\w+\\.w\\w+"));
    scripter.addDisabledProperty(QRegExp("C\\w+\\.f\\w+"));
    ASSERT_TRUE(scripter.scriptToFile(root, "tests/abc.json"));

    QFile file("tests/abc.json");
    bool isOk = file.open(QIODevice::ReadOnly);
    ASSERT_TRUE(isOk);
    QByteArray data = file.readAll();
    file.close();
    ASSERT_TRUE(!data.contains("minimumWidth"));
    ASSERT_TRUE(!data.contains("modal"));
    ASSERT_TRUE(!data.contains("whatsThis"));
    ASSERT_TRUE(!data.contains("windowFilePath"));
    ASSERT_TRUE(!data.contains("windowFlags"));
    ASSERT_TRUE(!data.contains("font"));
    ASSERT_TRUE(!data.contains("frameGeometry"));
    ASSERT_TRUE(data.contains("enabled"));
    ASSERT_TRUE(data.contains("childrenRect"));
    ASSERT_TRUE(data.contains("rect"));
}

// 测试是否可以强制将所有对象看作外部对象
TEST_F(tst_JsonScripter, refAllObjects)
{
    JsonLoader loader;
    REGISTER_METATYPE(CustomWidget);
    qRegisterMetaType<CustomWidget>("CustomWidget");
    QString errors;
    QWidget* root = load<QWidget*>(loader, "tests/AddDisabledProperty.json", &errors);
    ASSERT_TRUE(root != 0);
    _JsonScripter scripter;
    scripter.setOptions(JsonScripter::RefAllObjects);
    ASSERT_TRUE(scripter.scriptToFile(root, "tests/abc.json"));

    QFile file("tests/abc.json");
    bool isOk = file.open(QIODevice::ReadOnly);
    ASSERT_TRUE(isOk);
    QByteArray data = file.readAll();
    file.close();
    ASSERT_TRUE(data.contains(".ref"));
    ASSERT_TRUE(!data.contains(".type"));
}

TEST_F(tst_JsonScripter, addExternalObject)
{
    JsonScripter scripter;

    scripter.setOptions(
        JsonScripter::DisableQtSystemObjects |
        JsonScripter::DisableDynamicProperties
        );
    scripter.setMaxTreeDepth(50);
    scripter.addDisabledProperty(QRegExp("Q\\w+\\.\\w+"));
    //scripter.addDisabledProperty(QRegExp("\\w*(Test|TestGroup)\\(result|parent)"));
    scripter.addDisabledProperty(QRegExp("\\w*(Test|TestGroup)\\parent"));
    scripter.addDisabledProperty(
        QRegExp("\\w*NodeInputTest\\.(nodes|inputs|outputs|parent|nodeInput|primaryInputPort)")
        );
    scripter.addDisabledProperty("*Plan.status");
    scripter.addDisabledProperty(QRegExp("\\w*Field\\.(alignment|children|choice|notifySlot|offset|customItemTexts)"));

#if 0
    PlanView* view = new PlanView;
    QWidget* widget = new QWidget;
    QAction* action1 = new QAction("1", widget);
    QAction* action2 = new QAction("2", widget);
    widget->addAction(action1);
    widget->addAction(action2);
    view->setWidget(widget);

    QList<QAction*> actions = widget->actions();
    ASSERT_EQ(actions.count(), 2);

    // SHOULD NOT CRASH
    scripter.addExternalObject(view);
#endif

#if 0
    // 此处代码在Qt库版本不匹配时，会在QVariant的析构函数中崩溃，
    // 也就是说编译时使用QtVs2013，运行时必须也使用QtVs2013
    QPluginLoader pluginLoader("logviewd.dll");
    ASSERT_TRUE(pluginLoader.load());
    QObject* plugin = pluginLoader.instance();
    ASSERT_TRUE(plugin != NULL);
    scripter.addExternalObject(plugin);
#endif

    SUCCEED();
}

TEST_F(tst_JsonScripter, DISABLED_addMethodConnection)
{
    // DEPRECATED
    ASSERT_TRUE(false);
}

TEST_F(tst_JsonScripter, DISABLED_addPropertyBinding)
{
    // DEPRECATED
    ASSERT_TRUE(false);
}

TEST_F(tst_JsonScripter, script)
{
    JsonLoader loader;
    QString errors;

    REGISTER_METATYPE(Field);
    REGISTER_GENERIC_LIST_PARSER(Field*, loader);

    Field* externalLinkField = new Field;
    Field* externalField = new Field;
    externalLinkField->setObjectName("externalLinkField");
    externalField->setObjectName("externalField");
    externalField->setLink(externalLinkField);
    loader.addGlobalObject(externalLinkField);
    loader.addGlobalObject(externalField);

    Field* rootField = load<Field*>(loader, "tests/ObjectDependency.json", &errors);
    ASSERT_TRUE(rootField != NULL);
    ASSERT_STREQ(errors.toLocal8Bit().constData(), "");

    _JsonScripter scripter;
    scripter.addDisabledProperty(QRegExp(
        "Field.(max|min|size|valid|digitals|dimension|channelMask|linkPath"
        "|harmonicType|isInterHarmonic|isMoniterType|maxHarmonicOrder)"
        ));
    // 此处加入externalField和externalLinkField
    scripter.addExternalObject(externalField, true);

    // 开启SharedObjects，尝试导出为脚本，并重新加载回来，把两次加载的结果进行比较
    ASSERT_TRUE(scripter.scriptToFile(rootField, "tests/ObjectDependency.script.json"));
    Field* reloadedRootField = load<Field*>(loader, "tests/ObjectDependency.script.json", &errors);
    ASSERT_TRUE(reloadedRootField != NULL);
    ASSERT_STREQ(errors.toLocal8Bit().constData(), "");
    ASSERT_TRUE(compareFieldTree(rootField, reloadedRootField));

    // 禁用SharedObjects的合并，再试一次，此时脚本会更长，但是应该不会出错
    scripter.setOptions(JsonScripter::DisableSharedObjects);
    ASSERT_TRUE(scripter.scriptToFile(rootField, "tests/ObjectDependency.script.noshare.json"));
    Field* reloadedRootFieldNoShare = load<Field*>(loader, "tests/ObjectDependency.script.noshare.json", &errors);
    ASSERT_TRUE(reloadedRootFieldNoShare != NULL);
    ASSERT_STREQ(errors.toLocal8Bit().constData(), "");
    ASSERT_TRUE(compareFieldTree(rootField, reloadedRootFieldNoShare));
}

TEST_F(tst_JsonScripter, findAllObjects)
{
    JsonLoader loader;

    REGISTER_METATYPE(Field);
    REGISTER_GENERIC_LIST_PARSER(Field*, loader);

    Field* externalField = new Field;
    externalField->setObjectName("externalField");
    loader.addGlobalObject(externalField);

    Field* rootField = loader.load<Field*>("tests/ObjectDependency.json");
    ASSERT_TRUE(rootField != NULL);

    _JsonScripter scripter;
    QVariant rootVar = QVariant::fromValue<QObject*>(rootField);
    QObjectList allObjects;
    QObjectList sharedObjects;
    int allObjectCount = scripter.findAllObjects(rootVar, allObjects, sharedObjects);
    ASSERT_EQ(allObjectCount, 12);
    ASSERT_EQ(allObjects.count(), 12);
    ASSERT_EQ(sharedObjects.count(), 6);
}

TEST_F(tst_JsonScripter, findSharedObjects)
{
    // 由于findSharedObjects目前只是findAllObjects的简单封装，这里不再重复测试
    SUCCEED();
}

TEST_F(tst_JsonScripter, variantToList)
{
    _JsonScripter scripter;

    QList<float> floatList = QList<float>() << 1 << 2 << 3;
    QVariant floatListVar = QVariant::fromValue< QList<float> >(floatList);
    QVariantList variantList = scripter.variantToList(floatListVar).toList();
    ASSERT_EQ(variantList.count(), 3);
    ASSERT_EQ(variantList[0].toFloat(), 1);
    ASSERT_EQ(variantList[1].toFloat(), 2);
    ASSERT_EQ(variantList[2].toFloat(), 3);

    // 实际上QVariant无法直接搞定此功能，以下仅做阐述性说明
    bool canConvert = floatListVar.canConvert(QMetaType::QVariantList);
    ASSERT_TRUE(canConvert);
    // 没办法直接convert到QVariantList，只能用value<QSequentialIterable>()的方式来间接遍历
    bool convertOk = floatListVar.convert(QMetaType::QVariantList);
    EXPECT_FALSE(convertOk);
    // 这里也不会成功，也就是说，只有办法从自定义list到QSequentialIterable而已，没办法直接到QVariantList
    variantList = floatListVar.toList();
    EXPECT_NE(variantList.count(), 3);
}

TEST_F(tst_JsonScripter, enumToString)
{
    _JsonScripter scripter;

    GeneralTestClass object;
    object.m_enum = GeneralTestClass::EnumValue02;
    object.m_flag = GeneralTestClass::EnumValue01 | GeneralTestClass::EnumValue02;

    const QMetaObject* metaObject = object.metaObject();

    int enumPropIndex = metaObject->indexOfProperty("_enum");
    QMetaProperty enumProp = metaObject->property(enumPropIndex);
    QVariant enumValue = enumProp.read(&object);
    QString enumString = scripter.enumToString(enumValue, enumProp).toString();
    ASSERT_EQ(enumString, QString("GeneralTestClass.EnumValue02"));

    int flagPropIndex = metaObject->indexOfProperty("_flag");
    QMetaProperty flagProp = metaObject->property(flagPropIndex);
    QVariant flagValue = flagProp.read(&object);
    QString flagString = scripter.enumToString(flagValue, flagProp).toString();
    ASSERT_EQ(flagString, QString("GeneralTestClass.EnumValue01|GeneralTestClass.EnumValue02"));

}

// 指定是否导出子对象的属性
TEST_F(tst_JsonScripter, qchildrenProperty)
{
    JsonLoader loader;
    REGISTER_METATYPE(LastWidget);
    qRegisterMetaType<LastWidget>("LastWidget");
    LastWidget* other = new LastWidget;
    other->setObjectName("other");
    loader.addGlobalObject(other);
    QString errors;
    QWidget* root = load<QWidget*>(loader, "tests/qchildrenProperty.json", &errors);
    ASSERT_TRUE(root != 0);
    _JsonScripter scripter;
    scripter.addExternalObject(other);
    scripter.setOptions(JsonScripter::ScriptChildrenObjects);
    ASSERT_TRUE(scripter.scriptToFile(root, "tests/abc.json"));
    // 判断导出的json文件是否含有".qchildren"
    QFile file("tests/abc.json");
    bool isOk = file.open(QIODevice::ReadOnly);
    ASSERT_TRUE(isOk);
    QByteArray data = file.readAll();
    file.close();
    ASSERT_TRUE(data.contains(".qchildren"));
    // 重新将生成的json加载出来
    JsonLoader loader2;
    REGISTER_METATYPE(LastWidget);
    qRegisterMetaType<LastWidget>("LastWidget");
    LastWidget* other2 = new LastWidget;
    other2->setObjectName("other");
    loader2.addGlobalObject(other2);
    QString errors2;
    QWidget* root2 = load<QWidget*>(loader2, "tests/abc.json", &errors2);
    ASSERT_TRUE(root2 != 0);
}