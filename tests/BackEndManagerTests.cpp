// BackEndManagerTests.cpp
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>
#include "MediaPlayerImpls/BackEndEnum.h"
#include "config.h"
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

template<typename Container>
std::vector<std::string> backendsToStrings(const Container& backends) {
    std::vector<std::string> names;
    for (const auto& be : backends) {
        names.push_back(BackEndManager::toString(be));
    }
    std::sort(names.begin(), names.end());
    return names;
}


TEST_CASE("BackEndManager - toString", "[BackEndManager]") {
    REQUIRE(BackEndManager::toString(BackEndManager::BackEnd::QMediaPlayer) == "QMediaPlayer");
    REQUIRE(BackEndManager::toString(BackEndManager::BackEnd::VLCPlayer) == "VLCPlayer");
    REQUIRE(BackEndManager::toString(BackEndManager::BackEnd::QWebEngine) == "QWebEngine");
}

TEST_CASE("BackEndManager - fromString", "[BackEndManager]") {
    SECTION("Valid strings") {
        auto qmp = BackEndManager::fromString("QMediaPlayer");
        REQUIRE(qmp.has_value());
        REQUIRE(qmp.value() == BackEndManager::BackEnd::QMediaPlayer);

        auto vlc = BackEndManager::fromString("VLCPlayer");
        REQUIRE(vlc.has_value());
        REQUIRE(vlc.value() == BackEndManager::BackEnd::VLCPlayer);

        auto qwe = BackEndManager::fromString("QWebEngine");
        REQUIRE(qwe.has_value());
        REQUIRE(qwe.value() == BackEndManager::BackEnd::QWebEngine);
    }
    SECTION("Invalid string") {
        auto invalid = BackEndManager::fromString("InvalidBackendString");
        REQUIRE_FALSE(invalid.has_value());
    }
    SECTION("Empty string") {
        auto empty = BackEndManager::fromString("");
        REQUIRE_FALSE(empty.has_value());
    }
}

TEST_CASE("BackEndManager - isBackendAvailableCompiletime", "[BackEndManager]") {
#ifdef HAVE_QTMULTIMEDIA
    REQUIRE(BackEndManager::isBackendAvailableCompiletime(BackEndManager::BackEnd::QMediaPlayer) == true);
#else
    REQUIRE(BackEndManager::isBackendAvailableCompiletime(BackEndManager::BackEnd::QMediaPlayer) == false);
#endif

#ifdef HAVE_LIBVLC
    REQUIRE(BackEndManager::isBackendAvailableCompiletime(BackEndManager::BackEnd::VLCPlayer) == true);
#else
    REQUIRE(BackEndManager::isBackendAvailableCompiletime(BackEndManager::BackEnd::VLCPlayer) == false);
#endif

#ifdef HAVE_QTWEBENGINE
    REQUIRE(BackEndManager::isBackendAvailableCompiletime(BackEndManager::BackEnd::QWebEngine) == true);
#else
    REQUIRE(BackEndManager::isBackendAvailableCompiletime(BackEndManager::BackEnd::QWebEngine) == false);
#endif
}

TEST_CASE("BackEndManager - getAvailableCompiletimeBackEnds", "[BackEndManager]") {
    auto compileTimeAvailable = BackEndManager::getAvailableCompiletimeBackEnds();
    std::vector<std::string> expectedNames;

#ifdef HAVE_QTMULTIMEDIA
    expectedNames.push_back("QMediaPlayer");
#endif
#ifdef HAVE_LIBVLC
    expectedNames.push_back("VLCPlayer");
#endif
#ifdef HAVE_QTWEBENGINE
    expectedNames.push_back("QWebEngine");
#endif
    std::sort(expectedNames.begin(), expectedNames.end());

    REQUIRE_THAT(backendsToStrings(compileTimeAvailable), Catch::Matchers::Equals(expectedNames));
}

TEST_CASE("BackEndManager - isBackendAvailableRuntime", "[BackEndManager][Runtime]") {

    bool qmpRuntime = BackEndManager::isBackendAvailableRuntime(BackEndManager::BackEnd::QMediaPlayer);
#ifdef HAVE_QTMULTIMEDIA
    INFO("QMediaPlayer runtime availability check. Expected based on compile flag.");
    REQUIRE(qmpRuntime == BackEndTypes::QMediaPlayer::isAvailableRuntime()); // Check against direct call
#else
    INFO("QMediaPlayer runtime availability check. Expected false based on compile flag.");
    REQUIRE(qmpRuntime == false);
#endif

    // VLCPlayer
    bool vlcRuntime = BackEndManager::isBackendAvailableRuntime(BackEndManager::BackEnd::VLCPlayer);
#ifdef HAVE_LIBVLC
    INFO("VLCPlayer runtime availability check. Expected based on compile flag.");
    REQUIRE(vlcRuntime == BackEndTypes::VLCPlayer::isAvailableRuntime());
#else
    REQUIRE(vlcRuntime == false);
#endif

    // QWebEngine
    bool qweRuntime = BackEndManager::isBackendAvailableRuntime(BackEndManager::BackEnd::QWebEngine);
#ifdef HAVE_QTWEBENGINE
    INFO("QWebEngine runtime availability check. Expected based on compile flag.");
    REQUIRE(qweRuntime == BackEndTypes::QWebEngine::isAvailableRuntime());
#else
    REQUIRE(qweRuntime == false);
#endif
}

TEST_CASE("BackEndManager - getAvailableRuntimeBackEnds", "[BackEndManager][Runtime]") {
    std::vector<BackEndManager::BackEnd> runtimeAvailable = BackEndManager::getAvailableRuntimeBackEnds();
    std::vector<std::string> expectedRuntimeNames;

    if (BackEndTypes::QMediaPlayer::isAvailableRuntime()) {
        expectedRuntimeNames.push_back("QMediaPlayer");
    }
    if (BackEndTypes::VLCPlayer::isAvailableRuntime()) {
        expectedRuntimeNames.push_back("VLCPlayer");
    }
    if (BackEndTypes::QWebEngine::isAvailableRuntime()) {
        expectedRuntimeNames.push_back("QWebEngine");
    }
    std::sort(expectedRuntimeNames.begin(), expectedRuntimeNames.end());

    REQUIRE_THAT(backendsToStrings(runtimeAvailable), Catch::Matchers::Equals(expectedRuntimeNames));
}