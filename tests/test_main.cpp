#include <QtTest/QTest>
#include <QObject>
#include <catch2/catch_test_macros.hpp>
#include <QApplication>
#include "MainWindow.h"
#include "AddVideo.h"
#include <catch2/catch_test_macros.hpp>
#include <QApplication>
#include <QTest> // Core testing library
#include <QSignalSpy> // For checking signal emissions

#include "SettingsDialog.h"

// class TestMfaomp : public QObject
// {
//     Q_OBJECT
//
// private slots:
//     void initTestCase()
//     {
//         qDebug() << "Initializing test case...";
//     }
//
//     void cleanupTestCase()
//     {
//         qDebug() << "Cleaning up test case...";
//     }
//
//     void init()
//     {
//         qDebug() << "Initializing test...";
//     }
//
//     void cleanup()
//     {
//         qDebug() << "Cleaning up test...";
//     }
//
//     void testMainWindow()
//     {
//         MainWindow mainWindow;
//         QCOMPARE(mainWindow.windowTitle(), QString("mfaomp"));
//     }
//
//     void testMath()
//     {
//         int a = 1;
//         int b = 1;
//         QCOMPARE(a + b, 2);
//     }
// };


TEST_CASE("Example Test", "[example]") {
    REQUIRE(1 == 1);
}

TEST_CASE("Example Test 2", "[example]") {
    int x = 2;
    int y = 2;
    REQUIRE(x == y);
}

TEST_CASE("Gui Test example 1", "[gui]") {
    int argc = 0;
    char* argv[] = { nullptr };
    QApplication app(argc, argv);
}

// QTEST_MAIN(TestMfaomp)

#include "test_main.moc"