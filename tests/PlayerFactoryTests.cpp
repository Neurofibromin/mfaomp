#include <catch2/catch_test_macros.hpp>
#include "MediaPlayerImpls/PlayerFactory.h"
#include "MediaPlayerImpls/BackEndEnum.h"
#include "config.h"
#include <QUrl>

class MediaPlayerBase;
#ifdef HAVE_LIBVLC
#include "MediaPlayerImpls/VLCPlayerStruct.h"
#endif
#ifdef HAVE_QTMULTIMEDIA
#include "MediaPlayerImpls/QMediaPlayerStruct.h"
#endif
#ifdef HAVE_QTWEBENGINE
#include "MediaPlayerImpls/QWebEngineStruct.h"
#endif
#ifdef HAVE_SDL2
#include "MediaPlayerImpls/SDL2Struct.h"
#endif

TEST_CASE("PlayerFactory - ProduceChosenFactory", "[PlayerFactory]") {
    const QUrl testUrl("file:///dev/null");

    SECTION("Factory for available backends") {
#ifdef HAVE_LIBVLC
        SECTION("VLCPlayer Backend") {
            auto factoryFunc = PlayerFactory::ProduceChosenFactory(BackEndManager::BackEnd::VLCPlayer);
            REQUIRE(factoryFunc != nullptr);
            MediaPlayerBase* player = factoryFunc(testUrl);
            REQUIRE(player != nullptr);
            VLCPlayerStruct* vlcPlayer = dynamic_cast<VLCPlayerStruct*>(player);
            REQUIRE(vlcPlayer != nullptr);
            delete player;
        }
#endif

#ifdef HAVE_QTMULTIMEDIA
        SECTION("QMediaPlayer Backend") {
            auto factoryFunc = PlayerFactory::ProduceChosenFactory(BackEndManager::BackEnd::QMediaPlayer);
            REQUIRE(factoryFunc != nullptr);
            MediaPlayerBase* player = factoryFunc(testUrl);
            REQUIRE(player != nullptr);
            QMediaPlayerStruct* qmpPlayer = dynamic_cast<QMediaPlayerStruct*>(player);
            REQUIRE(qmpPlayer != nullptr);
            delete player;
        }
#endif

#ifdef HAVE_QTWEBENGINE
        SECTION("QWebEngine Backend") {
            auto factoryFunc = PlayerFactory::ProduceChosenFactory(BackEndManager::BackEnd::QWebEngine);
            REQUIRE(factoryFunc != nullptr);
            MediaPlayerBase* player = factoryFunc(testUrl);
            REQUIRE(player != nullptr);
            QWebEngineStruct* qwePlayer = dynamic_cast<QWebEngineStruct*>(player);
            REQUIRE(qwePlayer != nullptr);
            delete player;
        }
#endif

#ifdef HAVE_SDL2
        SECTION("SDL2 Backend") {
            auto factoryFunc = PlayerFactory::ProduceChosenFactory(BackEndManager::BackEnd::SDL2);
            REQUIRE(factoryFunc != nullptr);
            MediaPlayerBase* player = factoryFunc(testUrl);
            REQUIRE(player != nullptr);
            SDL2Struct* sdlPlayer = dynamic_cast<SDL2Struct*>(player);
            REQUIRE(sdlPlayer != nullptr);
            delete player;
        }
#endif
    }

    SECTION("Factory for unavailable backends") {
#ifndef HAVE_LIBVLC
        SECTION("VLCPlayer Backend (Unavailable)") {
            auto factoryFunc = PlayerFactory::ProduceChosenFactory(BackEndManager::BackEnd::VLCPlayer);
            REQUIRE(factoryFunc == nullptr);
        }
#endif

#ifndef HAVE_QTMULTIMEDIA
        SECTION("QMediaPlayer Backend (Unavailable)") {
            auto factoryFunc = PlayerFactory::ProduceChosenFactory(BackEndManager::BackEnd::QMediaPlayer);
            REQUIRE(factoryFunc == nullptr);
        }
#endif

#ifndef HAVE_QTWEBENGINE
        SECTION("QWebEngine Backend (Unavailable)") {
            auto factoryFunc = PlayerFactory::ProduceChosenFactory(BackEndManager::BackEnd::QWebEngine);
            REQUIRE(factoryFunc == nullptr);
        }
#endif

#ifndef HAVE_SDL2
        SECTION("SDL2 Backend (Unavailable)") {
            auto factoryFunc = PlayerFactory::ProduceChosenFactory(BackEndManager::BackEnd::SDL2);
            REQUIRE(factoryFunc == nullptr);
        }
#endif
    }

    SECTION("Default case for invalid backend") {
        // With invalid backend enum value cast from an integer that is not valid.
        std::cerr << "invalid test section" << std::endl;
        auto invalidBackend = static_cast<BackEndManager::BackEnd>(999);
        auto factoryFunc = PlayerFactory::ProduceChosenFactory(invalidBackend);
        REQUIRE(factoryFunc == nullptr);
    }
}

TEST_CASE("PlayerFactory - Factory Function Behavior and Instance Independence", "[PlayerFactory]") {
    const QUrl testUrl("file:///some/dummy/video.mp4");

#ifdef HAVE_LIBVLC
    SECTION("VLCPlayer: Factory function is reusable and creates independent instances") {
        auto factoryFunc = PlayerFactory::ProduceChosenFactory(BackEndManager::BackEnd::VLCPlayer);
        REQUIRE(factoryFunc != nullptr);

        INFO("Creating first player instance.");
        MediaPlayerBase* player1 = factoryFunc(testUrl);
        REQUIRE(player1 != nullptr);

        INFO("Creating second player instance from the same factory function.");
        MediaPlayerBase* player2 = factoryFunc(testUrl);
        REQUIRE(player2 != nullptr);

        INFO("Verifying that the two player instances are not the same object.");
        REQUIRE(player1 != player2);

        auto vlcPlayer1 = dynamic_cast<VLCPlayerStruct*>(player1);
        auto vlcPlayer2 = dynamic_cast<VLCPlayerStruct*>(player2);
        REQUIRE(vlcPlayer1 != nullptr);
        REQUIRE(vlcPlayer2 != nullptr);
        INFO("Verifying that the internal VLC media player objects are independent.");
        REQUIRE(vlcPlayer1->mediaPlayer != vlcPlayer2->mediaPlayer);

        INFO("Destroying the first player.");
        delete player1;

        INFO("Creating a third player to ensure the factory is still valid after deletion.");
        MediaPlayerBase* player3 = factoryFunc(testUrl);
        REQUIRE(player3 != nullptr);

        delete player2;
        delete player3;
    }
#endif

#ifdef HAVE_QTMULTIMEDIA
    SECTION("QMediaPlayer: Factory function is reusable and creates independent instances") {
        auto factoryFunc = PlayerFactory::ProduceChosenFactory(BackEndManager::BackEnd::QMediaPlayer);
        REQUIRE(factoryFunc != nullptr);

        INFO("Creating two player instances from the same factory function.");
        MediaPlayerBase* player1 = factoryFunc(testUrl);
        MediaPlayerBase* player2 = factoryFunc(testUrl);
        REQUIRE(player1 != nullptr);
        REQUIRE(player2 != nullptr);
        REQUIRE(player1 != player2);

        auto qmpPlayer1 = dynamic_cast<QMediaPlayerStruct*>(player1);
        auto qmpPlayer2 = dynamic_cast<QMediaPlayerStruct*>(player2);
        REQUIRE(qmpPlayer1 != nullptr);
        REQUIRE(qmpPlayer2 != nullptr);
        INFO("Verifying that the internal QMediaPlayer objects are independent.");
        REQUIRE(qmpPlayer1->mediaPlayer != qmpPlayer2->mediaPlayer);

        INFO("Destroying the first player.");
        delete player1;
        INFO("Creating a third player to ensure the factory is still valid after deletion.");
        MediaPlayerBase* player3 = factoryFunc(testUrl);
        REQUIRE(player3 != nullptr);

        delete player2;
        delete player3;
    }
#endif

#ifdef HAVE_QTWEBENGINE
    SECTION("QWebEngine: Factory function is reusable and creates independent instances") {
        auto factoryFunc = PlayerFactory::ProduceChosenFactory(BackEndManager::BackEnd::QWebEngine);
        REQUIRE(factoryFunc != nullptr);

        INFO("Creating two player instances from the same factory function.");
        MediaPlayerBase* player1 = factoryFunc(testUrl);
        MediaPlayerBase* player2 = factoryFunc(testUrl);
        REQUIRE(player1 != nullptr);
        REQUIRE(player2 != nullptr);
        REQUIRE(player1 != player2);

        auto qwePlayer1 = dynamic_cast<QWebEngineStruct*>(player1);
        auto qwePlayer2 = dynamic_cast<QWebEngineStruct*>(player2);
        REQUIRE(qwePlayer1 != nullptr);
        REQUIRE(qwePlayer2 != nullptr);
        INFO("Verifying that the internal QWebEngineView objects are independent.");
        REQUIRE(qwePlayer1->webView != qwePlayer2->webView);

        INFO("Destroying the first player.");
        delete player1;
        INFO("Creating a third player to ensure the factory is still valid after deletion.");
        MediaPlayerBase* player3 = factoryFunc(testUrl);
        REQUIRE(player3 != nullptr);

        delete player2;
        delete player3;
    }
#endif

#ifdef HAVE_SDL2
    SECTION("SDL2: Factory function is reusable and creates independent instances") {
        auto factoryFunc = PlayerFactory::ProduceChosenFactory(BackEndManager::BackEnd::SDL2);
        REQUIRE(factoryFunc != nullptr);

        INFO("Creating two player instances from the same factory function.");
        MediaPlayerBase* player1 = factoryFunc(testUrl);
        MediaPlayerBase* player2 = factoryFunc(testUrl);
        REQUIRE(player1 != nullptr);
        REQUIRE(player2 != nullptr);
        REQUIRE(player1 != player2);

        auto sdlPlayer1 = dynamic_cast<SDL2Struct*>(player1);
        auto sdlPlayer2 = dynamic_cast<SDL2Struct*>(player2);
        REQUIRE(sdlPlayer1 != nullptr);
        REQUIRE(sdlPlayer2 != nullptr);
        INFO("Verifying that the internal video widgets are independent.");
        REQUIRE(sdlPlayer1->videoWidget != sdlPlayer2->videoWidget);

        INFO("Destroying the first player.");
        delete player1;
        INFO("Creating a third player to ensure the factory is still valid after deletion.");
        MediaPlayerBase* player3 = factoryFunc(testUrl);
        REQUIRE(player3 != nullptr);

        delete player2;
        delete player3;
    }
#endif
}