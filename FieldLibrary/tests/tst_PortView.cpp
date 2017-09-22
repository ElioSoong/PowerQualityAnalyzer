#include "FieldLibrary/RealField.h"
#include "FieldLibrary/IntegerField.h"
#include "FieldLibrary/StringField.h"
#include "FieldLibrary/FieldView.h"
#include "FieldLibrary/FieldWidgetFactory.h"
#include "TestbenchLibrary/PortView.h"
#include "TestbenchLibrary/ContainerPortView.h"
#include "TestbenchLibrary/Device.h"
#include "TestbenchLibrary/Port.h"
#include "JsonLoader/JsonLoader.h"
#include "MockWidgets.h"

#include <gtest/gtest.h>
#include <QtTest>
#include <QLabel>
#include <QBuffer>

#define SHOW_WIDGETS    0

namespace tst
{
    class tst_PortView : public ::testing::Test
    {
    protected:
        virtual void SetUp() override
        {
            m_root = new ::ContainerPortView;
            m_device = new ::Device;
            m_protocol = new ::Protocol;
            m_ioDev = new ::QBuffer;
            m_port1 = new ::Port;
            m_port2 = new ::Port;
            m_field1 = new RealField;
            m_field2 = new RealField;
            m_factory = new FieldWidgetFactory;

            m_device->setName("device");
            m_port1->setName("Port1");
            m_port2->setName("Port2");
            m_field1->setChannelMask("Element1|Element2|Element3");
            m_field1->setEditorType("MyWidget");
            m_field2->setChannelMask("Element2");
            m_field2->setEditorType("MyWidget");

            m_port1->setField(m_field1);
            m_port2->setField(m_field2);

            QList<Port*> ports = QList<Port*>() << m_port1 << m_port2;
            m_device->setPorts(ports);

            static ProtocolAccess writeAccess;
            static ProtocolAccessInfo writeAccessCmd;
            writeAccessCmd.setFormat("%f");
            writeAccessCmd.setArgs(QStringList() << "field.data.FieldDataRole");
            writeAccess.setCmd(&writeAccessCmd);
            m_port1->setWriteAccess(&writeAccess);
            m_protocol->setIoDevice(m_ioDev);
            m_device->setProtocol(m_protocol);

            QStringList containerWidgetTypes = QStringList() << "MyContainer" << "MyContainer";
            m_factory->setProperty("containerWidgetTypes", containerWidgetTypes);
            FieldView::setDefaultFieldWidgetFactory(m_factory);

            REGISTER_METATYPE(FieldView);
            REGISTER_METATYPE(PortView);
            REGISTER_METATYPE(ContainerPortView);
            REGISTER_METATYPE(MyWidget);
            REGISTER_METATYPE(MyContainer);
            REGISTER_METATYPE_X(QLabel, "Label");

            CreateDefaultWidgets();
        }

        virtual void TearDown() override
        {
            delete m_device;
            delete m_port1;
        }

        void CreateDefaultWidgets()
        {
            m_child1 = new PortView;
            m_child2 = new PortView;
            m_child3 = new PortView;
            m_child1->setPortName("DOES NOT EXIST");
            m_child2->setPortName("Port1");
            m_child3->setPortName("Port2");
            QList<PortView*> children = QList<PortView*>() << m_child1 << m_child2 << m_child3;
            m_root->setChildren(children);

            PortView::setCurrentDevice(m_device);
            QWidget* rootWidget = m_root->createWidget();
            ASSERT_TRUE(rootWidget != NULL);
            QList<QWidget*> childrenWidgets = rootWidget->findChildren<QWidget*>(
                QString(), Qt::FindDirectChildrenOnly
                );
            ASSERT_EQ(childrenWidgets.count(), 2);
            QObject* childWidget1 = childrenWidgets[0];
            QList<MyWidget*> myWidgets = childWidget1->findChildren<MyWidget*>(
                QString()//, Qt::FindDirectChildrenOnly
                );
            // 3个通道(ChannelField)，每个通道对应一个独立的IFieldWidget
            ASSERT_EQ(myWidgets.count(), 3);

#if SHOW_WIDGETS
            rootWidget->show();
            QApplication::exec();
#endif
        }

        ::ContainerPortView* m_root;
        ::PortView* m_child1;
        ::PortView* m_child2;
        ::PortView* m_child3;

        ::Device* m_device;
        ::Protocol* m_protocol;
        ::QBuffer* m_ioDev;
        ::Port* m_port1;
        ::Port* m_port2;
        ::IField* m_field1;
        ::IField* m_field2;
        ::FieldWidgetFactory* m_factory;
    };

    TEST_F(tst_PortView, fieldWidgetFactory)
    {
        PortView view;

        FieldWidgetFactory* factory = view.fieldWidgetFactory();
        ASSERT_TRUE(factory != NULL);
    }

    TEST_F(tst_PortView, onCurrentDeviceChanged)
    {
        QList<Port*> ports = QList<Port*>() << m_port1;
        m_device->setPorts(ports);
        QWidget* rootWidgetAfterChanged = m_root->createWidget();
        ASSERT_TRUE(rootWidgetAfterChanged != NULL);
        QList<QWidget*> childrenWidgetsAfterChanged = rootWidgetAfterChanged->findChildren<QWidget*>(
            QString(), Qt::FindDirectChildrenOnly
            );
        ASSERT_EQ(childrenWidgetsAfterChanged.count(), 1);
#if SHOW_WIDGETS
        rootWidgetAfterChanged->show();
        QApplication::exec();
#endif

        // TODO: 注意检查内存泄漏问题：不再显示的对象是否已经被销毁？
    }

    TEST_F(tst_PortView, onPortChanged)
    {
        // 此处必须指定Channel的序号（可以不指定名字？待测试），因为commitToWidget将直接使用此序号
        //Channel channel0("Element1");
        Channel channel0(0, "Element1");
        QVariant value(123.4f);

        m_port1->setData(channel0, value, IField::FieldDataRole);
        //QVariant portValue = m_port1->data(channel0, IField::FieldDataRole);
        //ASSERT_EQ(value, portValue);
        QApplication::processEvents();

        // m_port1 <-> m_child2，因为m_child1对应了一个不存在的端口
        QWidget* widget = m_child2->fieldWidget(channel0.index());
        IFieldWidget* fieldWidget = qobject_cast<IFieldWidget*>(widget);
        ASSERT_TRUE(fieldWidget != NULL);
        QVariant widgetValue = fieldWidget->value();
        ASSERT_EQ(value.toString(), widgetValue.toString());
    }

    TEST_F(tst_PortView, onWidgetChanged)
    {
        Channel channel0(0, "Element1");
        QVariant value(456.7f);

        m_port1->setData(channel0, 123.4f, IField::FieldDataRole);
        tst_PortView::m_ioDev->reset();
        tst_PortView::m_ioDev->open(QIODevice::ReadWrite);

        // m_port1 <-> m_child2，因为m_child1对应了一个不存在的端口
        QWidget* widget = m_child2->fieldWidget(channel0.index());
        IFieldWidget* fieldWidget = qobject_cast<IFieldWidget*>(widget);
        ASSERT_TRUE(fieldWidget != NULL);
        fieldWidget->setValue(value);

        QApplication::processEvents();

        QVariant portValue = m_port1->data(channel0, IField::FieldDataRole);
        ASSERT_EQ(value.toString().toStdString(), portValue.toString().toStdString());

        QByteArray writtenData = tst_PortView::m_ioDev->data();
        tst_PortView::m_ioDev->close();
        ASSERT_EQ(value.toFloat(), QString(writtenData).toFloat());
    }

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

    TEST_F(tst_PortView, loadPortViewTreeFromJson)
    {
        // 给出完整的脚本载入测试，当然这依赖于至少一套FieldWidgets控件库
        JsonLoader loader;
        REGISTER_GENERIC_LIST_PARSER(PortView*, loader);
        REGISTER_GENERIC_LIST_PARSER(FieldView*, loader);

        PortView::setCurrentDevice(m_device);

        QString errorMsg;
        FieldView* rootView = load<FieldView*>(loader, "UnitTest/PortViewTree.json", &errorMsg);
        ASSERT_EQ(errorMsg.toStdString(), "");
        ASSERT_TRUE(rootView != NULL);

        QWidget* rootWidget = rootView->createWidget();
        ASSERT_TRUE(rootWidget != NULL);
#if SHOW_WIDGETS || 1
        rootWidget->show();
        QApplication::exec();
#endif
    }
}