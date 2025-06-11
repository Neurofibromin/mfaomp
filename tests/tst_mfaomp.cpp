#include <QtTest/QTest>
#include <QObject>

#include "MainWindow.h"
#include "AddVideo.h"

class TestMfaomp : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase()
    {
        qDebug() << "Initializing test case...";
    }

    void cleanupTestCase()
    {
        qDebug() << "Cleaning up test case...";
    }

    void init()
    {
        qDebug() << "Initializing test...";
    }

    void cleanup()
    {
        qDebug() << "Cleaning up test...";
    }

    void testMainWindow()
    {
        MainWindow mainWindow;
        QCOMPARE(mainWindow.windowTitle(), QString("mfaomp"));
    }

    void testMath()
    {
        int a = 1;
        int b = 1;
        QCOMPARE(a + b, 2);
    }
};

QTEST_MAIN(TestMfaomp)

#include "tst_mfaomp.moc"