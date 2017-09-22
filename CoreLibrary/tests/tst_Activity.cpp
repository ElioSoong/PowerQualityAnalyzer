#include <gtest/gtest.h>
#include <QtTest>
#include "common/Activity.h"
#include "common/ActivityModel.h"
#include "AnalyzerLibrary/PacketField.h"
#include "JsonLoader/JsonLoader.h"
#include "FieldLibrary/IField.h"
#include "FieldLibrary/RealField.h"
#include "FieldLibrary/IntegerField.h"
#include "FieldLibrary/VirtualField.h"
#include "JsonLoader/JsonLoader.h"

namespace tst
{
    class tst_Activity : public ::testing::Test
    {
    public:
        virtual void SetUp() Q_DECL_OVERRIDE
        {
            JsonLoader jsonLoader;
            REGISTER_METATYPE(IField);
            REGISTER_METATYPE(RealField);
            REGISTER_METATYPE(IntegerField);
            REGISTER_METATYPE(VirtualField);
            REGISTER_METATYPE(PacketField);
            REGISTER_METATYPE(PacketItemIndexField);
            REGISTER_METATYPE(LongIntegerField);
            REGISTER_METATYPE(Activity);
            REGISTER_METATYPE(ConfigItem);
            REGISTER_GENERIC_LIST_PARSER(ConfigItem*, jsonLoader);
            REGISTER_GENERIC_LIST_PARSER(IField*, jsonLoader);
            jsonLoader.load("CoreLibrary/tests/Activity.json");
            jsonLoader.load("AnalyzerLibrary/packet.json");
            // ÖØÖÃ×Ö¶ÎÊ÷
            IField::resetAll();
            m_activity = qobject_cast<Activity*>(jsonLoader.findObject("activity"));
            IField* obj = qobject_cast<IField*>(jsonLoader.findObject("packet"));
            obj->setMountPath("/pa/packet");
            IField *field = IField::at("/pa/packet/Basic/U/RMS");
            m_preConfigDatas << QVariant(32);
            field->setData(-1, -1, QVariant(32), 0);
            QString data = field->data().toString();
        }
    public:
        Activity* m_activity;
        QList<QVariant> m_preConfigDatas;
    };

    TEST_F(tst_Activity, start)
    {
        m_activity->start();
        QList<QVariant> priConfigItems;
        QList<QVariant> configItems;
        QList<ConfigItem *> items = m_activity->configItems();
        int itemsCount = items.size();;
        for (int item = 0; item < itemsCount; item++)
        {
            QString path = items[item]->path();
            QVariant data = items[item]->data();
            priConfigItems << data;
            int role = items[item]->role();

            IField *itemField = IField::at(path);
            if (itemField)
            {
                QVariant data = itemField->data();
                configItems << data;
            }
        }
        bool ok = (priConfigItems == configItems);
        ASSERT_TRUE(ok);
    }

    TEST_F(tst_Activity, stop)
    {
        m_activity->start();
        m_activity->stop();
        QList<QVariant> priConfigItems;
        QList<QVariant> configItems;
        QList<ConfigItem *> items = m_activity->configItems();
        int itemsCount = items.size();;
        for (int item = 0; item < itemsCount; item++)
        {
            QString path = items[item]->path();
            QVariant data = items[item]->data();
            priConfigItems << data;
            int role = items[item]->role();

            IField *itemField = IField::at(path);
            if (itemField)
            {
                QVariant data = itemField->data();
                configItems << data;
            }
        }
        bool ok = (m_preConfigDatas == configItems);
        ASSERT_TRUE(ok);
    }
}