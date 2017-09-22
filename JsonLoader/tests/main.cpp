
#include <QApplication>
#include <QtTest>

#include "gtest/gtest.h"

#ifdef _MSC_VER
#include <Windows.h>
void windowsMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QString messageWithLineEnding = msg + "\r\n";
    LPCWSTR localMsg = (LPCWSTR)messageWithLineEnding.unicode();
    OutputDebugString(localMsg);
    _putws(localMsg);
}
#endif

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

#ifdef _MSC_VER
    qInstallMessageHandler(windowsMessageHandler);
#endif

    testing::GTEST_FLAG(catch_exceptions) = false;
    testing::InitGoogleTest(&argc, argv);

    // Runs all tests using Google Test.  
    return RUN_ALL_TESTS();
}
