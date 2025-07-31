#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp> // For templated tests
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

// function overloading for "dispatch" for the independence of internal members
#ifdef HAVE_LIBVLC
void checkInternalIndependence(const VLCPlayerStruct* p1, const VLCPlayerStruct* p2) {
    INFO("Verifying that the internal VLC media player objects are independent.");
    REQUIRE(p1->mediaPlayer != p2->mediaPlayer);
}
#endif

#ifdef HAVE_QTMULTIMEDIA
void checkInternalIndependence(const QMediaPlayerStruct* p1, const QMediaPlayerStruct* p2) {
    INFO("Verifying that the internal QMediaPlayer objects are independent.");
    REQUIRE(p1->mediaPlayer != p2->mediaPlayer);
}
#endif

#ifdef HAVE_QTWEBENGINE
void checkInternalIndependence(const QWebEngineStruct* p1, const QWebEngineStruct* p2) {
    INFO("Verifying that the internal QWebEngineView objects are independent.");
    REQUIRE(p1->webView != p2->webView);
}
#endif

#ifdef HAVE_SDL2
void checkInternalIndependence(const SDL2Struct* p1, const SDL2Struct* p2) {
    INFO("Verifying that the internal video widgets are independent.");
    REQUIRE(p1->videoWidget != p2->videoWidget);
}
#endif

// --- Templated Test Case ---
// This struct is used to map a BackEnd enum value to its corresponding concrete struct type.
template<BackEndManager::BackEnd E, typename T>
struct BackendMapping {
    static constexpr BackEndManager::BackEnd backend_enum = E;
    using PlayerStructType = T;
};

// This single TEMPLATE_TEST_CASE replaces both of the previous test cases for available backends.
// It will generate a separate test run for each type in this list.
// The #ifdef guards ensure that we only try to test backends that are actually compiled.
TEMPLATE_TEST_CASE(
    "PlayerFactory - Verify factory function for all available backends",
    "[PlayerFactory][template]",
#ifdef HAVE_LIBVLC
    (BackendMapping<BackEndManager::BackEnd::VLCPlayer, VLCPlayerStruct>),
#endif
#ifdef HAVE_QTMULTIMEDIA
    (BackendMapping<BackEndManager::BackEnd::QMediaPlayer, QMediaPlayerStruct>),
#endif
#ifdef HAVE_QTWEBENGINE
    (BackendMapping<BackEndManager::BackEnd::QWebEngine, QWebEngineStruct>),
#endif
#ifdef HAVE_SDL2
    (BackendMapping<BackEndManager::BackEnd::SDL2, SDL2Struct>)
#endif
) {
    const QUrl testUrl("file:///some/dummy/video.mp4");

    // TestType will be one of the BackendMapping structs from the list above.
    const BackEndManager::BackEnd backendToTest = TestType::backend_enum;
    using PlayerStruct = typename TestType::PlayerStructType;

    INFO("Testing backend: " << BackEndManager::toString(backendToTest));

    // 1. Get the factory function once.
    auto factoryFunc = PlayerFactory::ProduceChosenFactory(backendToTest);
    REQUIRE(factoryFunc != nullptr);

    // 2. Test basic creation.
    SECTION("Basic player creation") {
        MediaPlayerBase* player = factoryFunc(testUrl);
        REQUIRE(player != nullptr);

        // Verify it's the correct concrete type.
        auto castedPlayer = dynamic_cast<PlayerStruct*>(player);
        REQUIRE(castedPlayer != nullptr);

        delete player;
    }

    // 3. Test reusability and instance independence.
    SECTION("Factory reusability and instance independence") {
        // Create two instances from the same factory function.
        MediaPlayerBase* player1 = factoryFunc(testUrl);
        MediaPlayerBase* player2 = factoryFunc(testUrl);
        REQUIRE(player1 != nullptr);
        REQUIRE(player2 != nullptr);

        // Verify that the two instances are different objects.
        REQUIRE(player1 != player2);

        // Cast to concrete types to check internal members.
        auto castedPlayer1 = dynamic_cast<PlayerStruct*>(player1);
        auto castedPlayer2 = dynamic_cast<PlayerStruct*>(player2);
        REQUIRE(castedPlayer1 != nullptr);
        REQUIRE(castedPlayer2 != nullptr);

        // Use our overloaded helper to check for deep independence.
        checkInternalIndependence(castedPlayer1, castedPlayer2);

        // 4. Test the create-destroy-create lifecycle.
        INFO("Destroying the first player to test lifecycle robustness.");
        delete player1;

        INFO("Creating a third player to ensure the factory is still valid.");
        MediaPlayerBase* player3 = factoryFunc(testUrl);
        REQUIRE(player3 != nullptr);

        // Final cleanup
        delete player2;
        delete player3;
    }
}

// Keep the tests for unavailable and invalid backends, as they are not parameterized.
TEST_CASE("PlayerFactory - Factory for unavailable or invalid backends", "[PlayerFactory]") {
    SECTION("Factory for unavailable backends") {
        if constexpr (! BackEndManager::isBackendAvailableCompiletime(BackEndManager::BackEnd::VLCPlayer)) {
            REQUIRE(PlayerFactory::ProduceChosenFactory(BackEndManager::BackEnd::VLCPlayer) == nullptr);
        }
        if constexpr (! BackEndManager::isBackendAvailableCompiletime(BackEndManager::BackEnd::QMediaPlayer)) {
            REQUIRE(PlayerFactory::ProduceChosenFactory(BackEndManager::BackEnd::QMediaPlayer) == nullptr);
        }
        if constexpr (! BackEndManager::isBackendAvailableCompiletime(BackEndManager::BackEnd::QWebEngine)) {
            REQUIRE(PlayerFactory::ProduceChosenFactory(BackEndManager::BackEnd::QWebEngine) == nullptr);
        }
        if constexpr (! BackEndManager::isBackendAvailableCompiletime(BackEndManager::BackEnd::SDL2)) {
            REQUIRE(PlayerFactory::ProduceChosenFactory(BackEndManager::BackEnd::SDL2) == nullptr);
        }
        REQUIRE(true);
    }

    SECTION("Default case for invalid backend enum") {
        auto invalidBackend = static_cast<BackEndManager::BackEnd>(999);
        REQUIRE(PlayerFactory::ProduceChosenFactory(invalidBackend) == nullptr);
    }
}