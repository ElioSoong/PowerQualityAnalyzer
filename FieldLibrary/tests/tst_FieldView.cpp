#include "FieldLibrary/RealField.h"
#include "FieldLibrary/IntegerField.h"
#include "FieldLibrary/StringField.h"
#include "FieldLibrary/FieldView.h"

#include <gtest/gtest.h>
#include <QtTest>

namespace tst
{
    class tst_FieldView : public ::testing::Test
    {
    };

    TEST_F(tst_FieldView, test1)
    {
        FieldView view;

        ASSERT_TRUE(true);
    }

    TEST_F(tst_FieldView, fieldWidgetFactory)
    {
        FieldView view;

        FieldWidgetFactory* factory = view.fieldWidgetFactory();
        ASSERT_TRUE(factory != NULL);
    }
}