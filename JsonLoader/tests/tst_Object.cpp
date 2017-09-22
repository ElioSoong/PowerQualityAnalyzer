#include "Object.h"
#include "Test.h"
#include "JsonLoader.h"

#include <gtest/gtest.h>
#include <QtTest>

#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>

#if !ENABLE_CUSTOM_OBJECT_FACTORY
#if 0
// 这段代码用在模板参数时，gcc不支持 [5/6/2016 CHH513]
DECLARE_CLASS_WRAPPER(QObject)
DECLARE_CLASS_WRAPPER(QWidget)
DECLARE_CLASS_WRAPPER(QLabel)
DECLARE_CLASS_WRAPPER(QPushButton)
#else
// 在Test.h中直接声明这些类 [12/30/2016 CHENHONGHAO]
#endif
Q_DECLARE_METATYPE(QObjectWrapper);
Q_DECLARE_METATYPE(QWidgetWrapper);
Q_DECLARE_METATYPE(QPushButtonWrapper);
Q_DECLARE_METATYPE(QLabelWrapper);
//Q_DECLARE_METATYPE(QList<QPushButtonWrapper*>);
//Q_DECLARE_METATYPE(QPushButtonWrapper*);
#endif


struct _ObjectContext : ObjectContext
{

};

struct _JsonLoader : JsonLoader
{
    using JsonLoader::parseArrayValue;
    using JsonLoader::parseArrayElementType;
};

class tst_Object : public ::testing::Test
{
protected:
    template <typename T>
    QVariantList createVariantArray(int count)
    {
        QVariantList elementVarList;
        while (count--)
        {
            T element;
            elementVarList.push_back(QVariant::fromValue<T>(element));
        }
        return elementVarList;
    }
    template <typename T>
    QVariantList createPointerVariantArray(int count)
    {
        QVariantList elementVarList;
        while (count--)
        {
            T* element = new T;
            elementVarList.push_back(QVariant::fromValue<T*>(element));
        }
        return elementVarList;
    }

    template <typename T>
    void parseArray(
        _JsonLoader& loader, 
        const QVariantList& elementVarList, 
        const char* elementTypeName, 
        const char* arrayTypeName,
        QVariant* resultVariant = NULL
        )
    {
        // 首先使用JsonLoader解析出元素对象类型
        int resultElementType = loader.parseArrayElementType(0, arrayTypeName);
        const char* resultElementTypeName = QMetaType::typeName(resultElementType);
        // 此处容易因REGISTER_METATYPE_X中的类型名与注册名不相等而失败，例如QLabelWrapper注册为"QLabel"
        //ASSERT_STREQ(resultElementTypeName, elementTypeName);
        // 然后使用JsonLoader根据元素对象类型将QVariantList转换为一个对应类型的列表（QVariantList->QList<T>）
        QVariant resultArray = loader.parseArrayValue(elementVarList, resultElementType);
        const char* resultArrayTypeName = resultArray.typeName();
        // 此处容易因QList<Variant> != QVariantList而失败，但实际上在Qt内部两者是相等的
        //ASSERT_STREQ(resultArrayTypeName, arrayTypeName);
        ASSERT_TRUE(resultArray.canConvert<QVariantList>());
        QSequentialIterable iterable = resultArray.value<QSequentialIterable>();
        int iterableSize = iterable.size();
        ASSERT_EQ(iterableSize, elementVarList.count());
        for (int i = 0; i < iterableSize; ++i)
            ASSERT_EQ(iterable.at(i), elementVarList.at(i));
        if (resultVariant)
            *resultVariant = resultArray;
    }
};

#define TEST_ARRAY(_loader, _type) {\
    QVariantList array = createVariantArray<_type>(2);  \
    QVariant result;    \
    parseArray<_type>((_loader), array, #_type, "QList<" #_type ">", &result);   \
    ASSERT_TRUE(result.canConvert< QList<_type> >());    \
}

#define TEST_POINTER_ARRAY(_loader, _type) {\
    QVariantList array = createPointerVariantArray<_type>(2);  \
    QVariant result;    \
    parseArray<_type>((_loader), array, #_type, "QList<" #_type ">", &result);   \
    ASSERT_TRUE(result.canConvert< QList<_type*> >());    \
}

TEST_F(tst_Object, registerMetatype)
{
#if !ENABLE_CUSTOM_OBJECT_FACTORY
    REGISTER_METATYPE_X(QWidgetWrapper, "QWidget");
    REGISTER_METATYPE_X(QLabelWrapper, "QLabel");
    REGISTER_METATYPE_X(QPushButtonWrapper, "QPushButton");
#else
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

    ASSERT_TRUE(id1 != 0);
    ASSERT_TRUE(id2 != 0);
    ASSERT_TRUE(id3 != 0);
    ASSERT_TRUE(id4 != 0);
    ASSERT_TRUE(obj1 != NULL);
    ASSERT_TRUE(obj2 != NULL);
}


TEST_F(tst_Object, registerListParser)
{
    _JsonLoader loader;

#if !ENABLE_CUSTOM_OBJECT_FACTORY
    REGISTER_METATYPE_X(QPushButtonWrapper, "QPushButton");
    REGISTER_METATYPE_X(QLabelWrapper, "QLabel");
    REGISTER_METATYPE_X(QWidgetWrapper, "QWidget");
#else
    REGISTER_METATYPE(QPushButton);
    REGISTER_METATYPE(QLabel);
    REGISTER_METATYPE(QWidget);
#endif

    // 首先使用Helper宏注册多个通用的ArrayValueParser
    REGISTER_GENERIC_LIST_PARSER(QVariant, loader);
    REGISTER_GENERIC_LIST_PARSER(bool, loader);
    REGISTER_GENERIC_LIST_PARSER(int, loader);
    REGISTER_GENERIC_LIST_PARSER(double, loader);
    // 然后注册两个自定义的ArrayValueParser
    // 注册了也用不到，因为这里是QVector，只是演示一下用法
//     loader.registerArrayValueParser(
//         new GenericVectorValueParser<QLabelWrapper*>(
//         &loader,
//         GET_METATYPE_ID_METHOD("QLabel"))
//         );
#if !ENABLE_CUSTOM_OBJECT_FACTORY
    loader.registerArrayValueParser(
        new GenericListValueParser<QLabelWrapper*>(
        &loader,
        GET_METATYPE_ID_METHOD("QLabel"))
        );
    loader.registerArrayValueParser(
        new GenericListValueParser<QPushButtonWrapper*>(
        &loader,
        GET_METATYPE_ID_METHOD("QPushButton"))
        );
    loader.registerArrayValueParser(
        new GenericListValueParser<QWidgetWrapper*>(
        &loader,
        GET_METATYPE_ID_METHOD("QWidget"))
        );
#else
    REGISTER_GENERIC_LIST_PARSER(QPushButton*, loader);
    REGISTER_GENERIC_LIST_PARSER(QLabel*, loader);
    REGISTER_GENERIC_LIST_PARSER(QWidget*, loader);
#endif

    TEST_ARRAY(loader, bool);
    TEST_ARRAY(loader, int);
    TEST_ARRAY(loader, double);
    TEST_ARRAY(loader, QVariant);
#if !ENABLE_CUSTOM_OBJECT_FACTORY
    TEST_POINTER_ARRAY(loader, QPushButtonWrapper);
    TEST_POINTER_ARRAY(loader, QLabelWrapper);
    TEST_POINTER_ARRAY(loader, QWidgetWrapper);
#else
    TEST_POINTER_ARRAY(loader, QPushButton);
    TEST_POINTER_ARRAY(loader, QLabel);
    TEST_POINTER_ARRAY(loader, QWidget);
#endif

    // 最后注册一个完全自定义的：保证QList<QObject> = QObjectList
    loader.registerArrayValueParser(
        new GenericContainerValueParser<QObject*, QObjectList>(
        &loader,
        GET_METATYPE_ID_METHOD("QObject"),
        QRegExp("QObjectList"))
        );
    QVariantList array = createPointerVariantArray<QObject>(2);
    QVariant result;
    parseArray<QObject>(loader, array, "QObject", "QList<QObject*>", &result);
    // 此处毫无难度，实际上Qt内部保证了QList<QObject*>和QObjectList的通用性
    ASSERT_TRUE(result.canConvert< QObjectList >());
    parseArray<QObject>(loader, array, "QObject", "QObjectList", &result);
    // 此处毫无难度，实际上Qt内部保证了QList<QObject*>和QObjectList的通用性
    ASSERT_TRUE(result.canConvert< QList<QObject*> >());
}
