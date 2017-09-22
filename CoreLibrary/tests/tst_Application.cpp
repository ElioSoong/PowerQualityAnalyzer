#include <gtest/gtest.h>
#include <QtTest>

#include "common/Plugin.h"
#include "common/Application.h"
#include "common/Activity.h"
#include "common/ActivityModel.h"
#include "JsonLoader/JsonLoader.h"

namespace tst
{
    class MockApplication : public ::Application
    {

    public:
        MockApplication(int& argc, char** argv) : Application(argc, argv)
        {

        }

        virtual MainWindow* createMainWindow() override
        {
            return NULL;
        }

    };

    class tst_Application : public ::testing::Test
    {
    };

    TEST_F(tst_Application, setPlugins)
    {
        // FIXME:Application不好测试，一个应用只能创建一个Application对象 [5/3/2017 CHENHONGHAO]
#if 0
        int argc = 1;
        char* argv[] = { "mockapp.exe" };
        MockApplication app(argc, argv);
        JsonLoader loader;
        PluginList list;


        REGISTER_METATYPE(Activity);
        REGISTER_METATYPE(ActivityModel);
        REGISTER_GENERIC_LIST_PARSER(Activity*, loader);
        list.setIid("com.inno.common.Module");
        list.setName("modules");
#endif
    }

}