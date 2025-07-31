#define CATCH_CONFIG_EXTERNAL_INTERFACES
#include <iostream>

#include "VLCPlayerStruct.h"
#include "vlc.hpp"
#include <QApplication>
#include <QDir>
#include <QObject>
#include <QTest>
#include <QUrl>
#include <catch2/catch_session.hpp>
#include <catch2/catch_test_macros.hpp>
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

QUrl getSampleVideoUrl() {
    QString currentSourceFilePath = __FILE__;
    QFileInfo sourceFileInfo(currentSourceFilePath);
    QString currentDirectory = sourceFileInfo.absolutePath();
    QUrl testUrl1 = QUrl::fromLocalFile(currentDirectory + "/test-data-files/SampleVideo_1280x720_1mb.mp4");
    std::cout << "Test url: " << testUrl1.toString().toStdString();
    QFileInfo testFile(testUrl1.toLocalFile());
    REQUIRE(testFile.exists());
    REQUIRE(testFile.isFile());
    REQUIRE(testFile.fileName() == "SampleVideo_1280x720_1mb.mp4");
    return testUrl1;
}


TEST_CASE("VLCPlayerStruct - Constructor and Initial State", "[vlc][media_player]") {
    QUrl videoUrl = getSampleVideoUrl();
    VLCPlayerStruct player(videoUrl);

    SECTION("Initial objects are created") {
        REQUIRE(player.vlcInstance != nullptr);
        REQUIRE(player.mediaPlayer != nullptr);
        REQUIRE(player.getVideoWidget() != nullptr);
    }

    SECTION("Video widget has correct initial properties") {
        QWidget* widget = player.getVideoWidget();
        REQUIRE(widget->testAttribute(Qt::WA_NativeWindow));
    }

    SECTION("Media is loaded into the player") {
        QTest::qWait(200);
        REQUIRE(player.mediaPlayer->media() != nullptr);
        REQUIRE(player.duration() > 0);
    }
}


TEST_CASE("VLCPlayerStruct - Play and Pause", "[vlc][media_player]") {
    VLCPlayerStruct player(getSampleVideoUrl());
    player.getVideoWidget()->show();
    QTest::qWait(200);

    player.play();
    QTest::qWait(500);

    int64_t timeAfterPlay = player.time();
    REQUIRE(timeAfterPlay > 0);

    player.pause();
    QTest::qWait(200);

    int64_t timeAfterPause = player.time();
    REQUIRE_THAT(static_cast<double>(timeAfterPause), Catch::Matchers::WithinAbs(timeAfterPlay, 100.0));

    QTest::qWait(500);
    REQUIRE(player.time() == timeAfterPause);
}


TEST_CASE("VLCPlayerStruct - Mute and Unmute", "[vlc][media_player]") {
    VLCPlayerStruct player(getSampleVideoUrl());

    player.mute();
    REQUIRE(player.mediaPlayer->mute());

    player.unmute();
    REQUIRE_FALSE(player.mediaPlayer->mute());
}


TEST_CASE("VLCPlayerStruct - Speed Control", "[vlc][media_player]") {
    VLCPlayerStruct player(getSampleVideoUrl());

    float newSpeed = 2.0f;
    player.speed(newSpeed);
    REQUIRE_THAT(player.speed(), Catch::Matchers::WithinRel(newSpeed, 0.01f));

    newSpeed = 0.5f;
    player.speed(newSpeed);
    REQUIRE_THAT(player.speed(), Catch::Matchers::WithinRel(newSpeed, 0.01f));
}


TEST_CASE("VLCPlayerStruct - Time and Duration Management", "[vlc][media_player]") {
    VLCPlayerStruct player(getSampleVideoUrl());
    QTest::qWait(200);

    float videoDuration = player.duration();
    REQUIRE(videoDuration > 0);

    int64_t seekTime = static_cast<int64_t>(videoDuration / 2.0);
    player.set_time(seekTime);
    QTest::qWait(300);

    REQUIRE_THAT(static_cast<double>(player.time()), Catch::Matchers::WithinAbs(seekTime, 100.0));
}

TEST_CASE("VLCPlayerStruct - Cleanup on Destruction does not crash", "[vlc][media_player]") {
    VLCPlayerStruct* player = new VLCPlayerStruct(getSampleVideoUrl());
    QWidget* widget = player->getVideoWidget();
    REQUIRE(widget != nullptr);
    delete player;
    player = nullptr;
    SUCCEED("Destructor completed without crashing.");
}