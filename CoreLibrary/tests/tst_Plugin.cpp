#include <gtest/gtest.h>
#include <QtTest>

#include "common/Plugin.h"
#include "common/Activity.h"
#include "common/ActivityModel.h"
#include "JsonLoader/JsonLoader.h"

namespace tst
{
    class tst_Plugin : public ::testing::Test
    {
    public:
        virtual void SetUp() Q_DECL_OVERRIDE
        {
            REGISTER_METATYPE(Activity);
            REGISTER_METATYPE(ActivityModel);
            REGISTER_GENERIC_LIST_PARSER(Activity*, loader);
            list.setIid("com.inno.common.Module");
            list.setName("modules");

#if 1
            // Plugin是使用相对路径来遍历插件文件夹的，在此之前需要先正确设置相对路径 [5/3/2017 CHENHONGHAO]
            QString currentPath = QDir::currentPath();
            QDir::setCurrent(currentPath + "/CoreLibrary/tests");
            currentPath = QDir::currentPath();
            qDebug() << currentPath;
#endif
        }

        JsonLoader loader;
        PluginList list;
    };

    TEST_F(tst_Plugin, loadJsonPlugin)
    {
        int count = list.load(loader);
        ASSERT_EQ(count, 1);
    }

    TEST_F(tst_Plugin, loadDllPlugin)
    {
        ASSERT_TRUE(true);
    }

    TEST_F(tst_Plugin, listPluginInstances)
    {
        int count = list.load(loader);
        ASSERT_EQ(count, 1);
        QObjectList instances = list.instances();
        ASSERT_EQ(instances.count(), 1);
        ASSERT_EQ(instances[0]->objectName(), "activityModel");
    }
}