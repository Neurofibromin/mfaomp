//SettingTests.cpp
#define CATCH_CONFIG_EXTERNAL_INTERFACES
#include "AddVideo.h"
#include "MainWindow.h"
#include "SettingsDialog.h"
#include <QApplication>
#include <QMessageBox>
#include <QObject>
#include <QPushButton>
#include <QSignalSpy>
#include <QTest>
#include <catch2/catch_session.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <catch2/reporters/catch_reporter_event_listener.hpp>
#include <catch2/reporters/catch_reporter_registrars.hpp>

static int g_test_argc = 1;
static char app_name_str[] = "mfaomp_tests_executable";
static char* g_test_argv[] = {app_name_str, nullptr};

namespace {
    class QtAppManagerListener : public Catch::EventListenerBase {
    public:
        using Catch::EventListenerBase::EventListenerBase;

        QApplication* currentApp = nullptr;

        void testRunStarting(Catch::TestRunInfo const& /*testRunInfo*/) override {
            if (!QApplication::instance()) {
                currentApp = new QApplication(g_test_argc, g_test_argv);
            }
        }

        void testRunEnded(Catch::TestRunStats const& /*testRunStats*/) override {
            if (currentApp) {
                delete currentApp;
                currentApp = nullptr;
            }
        }
    };
}

CATCH_REGISTER_LISTENER(QtAppManagerListener)

TEST_CASE("SettingsDialog - Initialization emits correct initial values on accept", "[gui][SettingsDialog]") {
    float initialSpeedIncrement = 0.25f;
    float initialMinSpeed = 0.5f;
    float initialMaxSpeed = 4.0f;
    QString initialStyle = "Fusion";

    SettingsDialog dialog(initialSpeedIncrement, initialMinSpeed, initialMaxSpeed, initialStyle);
    QSignalSpy settingsSpy(&dialog, &SettingsDialog::settingsAccepted);
    QSignalSpy styleSpy(&dialog, &SettingsDialog::styleAccepted);

    auto* okButton = dialog.findChild<QPushButton *>("okButton");
    REQUIRE(okButton);
    QTest::mouseClick(okButton, Qt::LeftButton);

    REQUIRE(settingsSpy.count() == 1);
    QList<QVariant> settingsArgs = settingsSpy.takeFirst();
    REQUIRE(settingsArgs.at(0).toFloat() == initialSpeedIncrement);
    REQUIRE(settingsArgs.at(1).toFloat() == initialMinSpeed);
    REQUIRE(settingsArgs.at(2).toFloat() == initialMaxSpeed);

    REQUIRE(styleSpy.count() == 1);
    QList<QVariant> styleArgs = styleSpy.takeFirst();
    REQUIRE(styleArgs.at(0).toString() == initialStyle);
}

TEST_CASE("SettingsDialog - Accept with valid input (private members)", "[gui][SettingsDialog]") {
    SettingsDialog dialog(0.1f, 0.5f, 2.0f, "Fusion");
    QSignalSpy settingsSpy(&dialog, &SettingsDialog::settingsAccepted);


    auto* speedSpinBox = dialog.findChild<QDoubleSpinBox *>("speedIncrementSpinBox");
    auto* minSpeedSpinBox = dialog.findChild<QDoubleSpinBox *>("minPlaybackSpeedSpinBox");
    auto* maxSpeedSpinBox = dialog.findChild<QDoubleSpinBox *>("maxPlaybackSpeedSpinBox");
    auto* okButton = dialog.findChild<QPushButton *>("okButton");

    REQUIRE(speedSpinBox);
    REQUIRE(minSpeedSpinBox);
    REQUIRE(maxSpeedSpinBox);
    REQUIRE(okButton);

    double newSpeedIncrement = 0.3;
    double newMinSpeed = 0.6;
    double newMaxSpeed = 2.5;

    speedSpinBox->setValue(newSpeedIncrement);
    minSpeedSpinBox->setValue(newMinSpeed);
    maxSpeedSpinBox->setValue(newMaxSpeed);


    QTest::mouseClick(okButton, Qt::LeftButton);

    REQUIRE(dialog.result() == QDialog::Accepted);
    REQUIRE(settingsSpy.count() == 1);
    QList<QVariant> arguments = settingsSpy.takeFirst();
    REQUIRE_THAT(arguments.at(0).toDouble(), Catch::Matchers::WithinRel(newSpeedIncrement,0.001));
    REQUIRE_THAT(arguments.at(1).toDouble(), Catch::Matchers::WithinRel(newMinSpeed,0.001));
    REQUIRE_THAT(arguments.at(2).toDouble(), Catch::Matchers::WithinRel(newMaxSpeed,0.001));
}

TEST_CASE("SettingsDialog - Reject with invalid speed range", "[gui][SettingsDialog]") {
    SettingsDialog dialog(0.1f, 0.5f, 2.0f, "Fusion");
    dialog.show();
    QTest::qWait(50);
    QSignalSpy settingsSpy(&dialog, &SettingsDialog::settingsAccepted);

    auto* speedSpinBox = dialog.findChild<QDoubleSpinBox *>("speedIncrementSpinBox");
    auto* minSpeedSpinBox = dialog.findChild<QDoubleSpinBox *>("minPlaybackSpeedSpinBox");
    auto* maxSpeedSpinBox = dialog.findChild<QDoubleSpinBox *>("maxPlaybackSpeedSpinBox");
    auto* okButton = dialog.findChild<QPushButton *>("okButton");

    REQUIRE(speedSpinBox);
    REQUIRE(minSpeedSpinBox);
    REQUIRE(maxSpeedSpinBox);
    REQUIRE(okButton);

    minSpeedSpinBox->setValue(3.0);
    maxSpeedSpinBox->setValue(2.0);

    QTimer::singleShot(100, []() {
        QApplication::processEvents();
        QWidget* activeModal = QApplication::activeModalWidget();
        QMessageBox* messageBox = qobject_cast<QMessageBox *>(activeModal);
        if (messageBox) {
            messageBox->accept();
        } else {
            FAIL("No QMessageBox appeared after clicking OK on SettingsDialog.");
        }
    });

    QTest::mouseClick(okButton, Qt::LeftButton);
    REQUIRE(dialog.result() == QDialog::Rejected);
    REQUIRE(dialog.isVisible());
    REQUIRE(settingsSpy.count() == 0);
}

TEST_CASE("SettingsDialog - Cancel button", "[gui][SettingsDialog]") {
    SettingsDialog dialog(0.1f, 0.5f, 2.0f, "Fusion");
    QSignalSpy settingsSpy(&dialog, &SettingsDialog::settingsAccepted);
    QSignalSpy styleSpy(&dialog, &SettingsDialog::styleAccepted);
    auto* cancelButton = dialog.findChild<QPushButton *>("cancelButton");
    REQUIRE(cancelButton);
    REQUIRE(cancelButton->text() == "Cancel");
    QTest::mouseClick(cancelButton, Qt::LeftButton);
    REQUIRE(dialog.result() == QDialog::Rejected);
    REQUIRE(settingsSpy.count() == 0);
    REQUIRE(styleSpy.count() == 0);
}
