//BackEndEnum.h

#ifndef BACKENDENUM_H
#define BACKENDENUM_H
#include <map>
#include <optional>
#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <type_traits>
#include <algorithm>
#include <optional>
#include <array>
#include <algorithm>
#include <iostream>
#include <type_traits>
#include "config.h"
#include  "BackEndEnum.h"
#include <QDebug>
#include "config.h"
#include <type_traits>

#ifdef HAVE_LIBVLC
#include <vlc/vlc.h>
#include "vlc.hpp"
#endif

#ifdef HAVE_QTMULTIMEDIA
#include <QtMultimedia>
#include <QtMultimediaWidgets>
#include <QMediaPlayer>
#include <QMediaDevices>
#endif

#ifdef HAVE_QTWEBENGINE
#include <QWebEngineView>
#include <QWebEngineProfile>
#endif

#ifdef HAVE_SDL2
#include <SDL2/SDL.h>
#endif


namespace { //anon namespace to hide structs
    namespace BackEndTypes {
        struct QMediaPlayer {
            static bool isAvailableRuntime() {
#ifdef HAVE_QTMULTIMEDIA
                try {
                    QMediaPlayer player;
                    if (QMediaDevices::audioOutputs().isEmpty()) {
                        // return false; // if no audio outputs means 'unavailable' this could be false, but it can stay true for now
                    }
                    return true;
                } catch (...) {
                    return false;
                }
#endif
                return false;
            }
            static consteval bool isAvailableCompiletime() {
#ifdef HAVE_QTMULTIMEDIA
                return true;
#else
                return false;
#endif
            }
            static constexpr const char* name = "QMediaPlayer";
        };

        struct VLCPlayer {
            static bool isAvailableRuntime() {
#ifdef HAVE_LIBVLC
                libvlc_instance_t* inst = libvlc_new(0, nullptr);
                if (inst) {
                    libvlc_release(inst);
                    return true;
                }
#endif
                return false;
            }
            static consteval bool isAvailableCompiletime() {
#ifdef HAVE_LIBVLC
                return true;
#else
                return false;
#endif
            }
            static constexpr const char* name = "VLCPlayer";
        };

        struct QWebEngine {
            static bool isAvailableRuntime() {
#ifdef HAVE_QTWEBENGINE
                try {
                    QWebEngineView view;
                    return true;
                } catch (...) {
                    return false;
                }
#endif
                return false;
            }
            static consteval bool isAvailableCompiletime() {
#ifdef HAVE_QTWEBENGINE
                return true;
#else
                return false;
#endif
            }
            static constexpr const char* name = "QWebEngine";
        };

        struct SDL2Player {
            static bool isAvailableRuntime() {
#ifdef HAVE_SDL2
                return true; //TODO: add real check here
#endif
                return false;
            }
            static consteval bool isAvailableCompiletime() {
#ifdef HAVE_SDL2
                return true;
#else
                return false;
#endif
            }
            static constexpr const char* name = "SDL2";
        };
    }
}

class BackEndManager {
    BackEndManager() = delete; //make class pure static
public:
    enum class BackEnd {
        QMediaPlayer,
        VLCPlayer,
        QWebEngine,
        SDL2
    };

    static constexpr std::array<BackEnd, 4> AllBackEnds = {
        BackEnd::VLCPlayer,
        BackEnd::QMediaPlayer,
        BackEnd::QWebEngine,
        BackEnd::SDL2
    };

private:
    struct BackEndMetaData {
        using AvailabilityChecker = bool (*)();
        using NameGetter = const char* (*)();

        BackEndMetaData(AvailabilityChecker runt, /*AvailabilityChecker compt,*/ NameGetter nameGetter) :
            isRunTimeAvailable(runt),
            // isCompileTimeAvailable(compt),
            getName(nameGetter) {}

        AvailabilityChecker isRunTimeAvailable;
        // AvailabilityChecker isCompileTimeAvailable;
        NameGetter getName;
    };

    static const std::map<BackEnd, BackEndMetaData> &  getBackEndMetaDataMap() {
        static const auto qmpName = []() { return BackEndTypes::QMediaPlayer::name; };
        static const auto vlcName = []() { return BackEndTypes::VLCPlayer::name; };
        static const auto qweName = []() { return BackEndTypes::QWebEngine::name; };
        static const auto sdlName = []() { return BackEndTypes::SDL2Player::name; };
        static const std::map<BackEnd, BackEndMetaData> metaDataMap = {
            {BackEnd::QMediaPlayer, {&BackEndTypes::QMediaPlayer::isAvailableRuntime, qmpName}},
            {BackEnd::VLCPlayer,   {&BackEndTypes::VLCPlayer::isAvailableRuntime, vlcName}},
            {BackEnd::QWebEngine,  {&BackEndTypes::QWebEngine::isAvailableRuntime, qweName}},
            {BackEnd::SDL2,  {&BackEndTypes::SDL2Player::isAvailableRuntime, sdlName}}
        };
        return metaDataMap;
    }

    template<typename Func>
    static constexpr decltype(auto) dispatchToType(BackEnd backend, Func&& func) {
        switch (backend) {
            case BackEnd::QMediaPlayer:
                return std::forward<Func>(func).template operator()<BackEndTypes::QMediaPlayer>();
            case BackEnd::VLCPlayer:
                return std::forward<Func>(func).template operator()<BackEndTypes::VLCPlayer>();
            case BackEnd::QWebEngine:
                return std::forward<Func>(func).template operator()<BackEndTypes::QWebEngine>();
            case BackEnd::SDL2:
                return std::forward<Func>(func).template operator()<BackEndTypes::SDL2Player>();
        }
    }

public:
    static std::string constexpr toString(BackEnd backend) {
        return dispatchToType(backend, []<typename T>() { // C++20 lambda template
            return std::string(T::name);
        });
    }

    static bool constexpr isBackendAvailableCompiletime(BackEnd backend) {
        return dispatchToType(backend, []<typename T>() {
            return T::isAvailableCompiletime();
        });
    }
    static bool isBackendAvailableRuntime(BackEnd backend) {
        return dispatchToType(backend, []<typename T>() {
            return T::isAvailableRuntime();
        });
    }

    static std::optional<BackEnd> consteval fromStringComptime(const std::string& str) {
        auto arr = AllBackEnds;
        for (const auto& bc : arr) {
            if (toString(bc) == str) {
                return bc;
            }
        }
        return std::nullopt;
    }

    static std::optional<BackEnd> fromString(const std::string& str) {
        const auto& map = getBackEndMetaDataMap();
        for (const auto& pair : map) {
            if (pair.second.getName() == str) {
                return pair.first;
            }
        }
        return std::nullopt;
    }


    static consteval auto getAvailableCompiletimeBackEnds() {
        constexpr  int count = std::count_if(AllBackEnds.begin(), AllBackEnds.end(),
            [](BackEnd be){ return isBackendAvailableCompiletime(be); });
        // consteval int count = []() {
        //     int available_count = 0;
        //     for (BackEnd backend : AllBackEnds) {
        //         if (isBackendAvailableCompiletime(backend)) {
        //             available_count++;
        //         }
        //     }
        //     return available_count;
        // }();
        std::array<BackEnd, count> available{};

        if constexpr (count > 0) {
            int current_idx = 0;
            for (BackEnd backend : AllBackEnds) {
                if (isBackendAvailableCompiletime(backend)) {
                    available[current_idx++] = backend;
                }
            }
        }
        return available;
    }

    static std::vector<BackEnd> getAvailableRuntimeBackEnds() {
        std::vector<BackEnd> available;
        std::ranges::for_each(AllBackEnds, [&available](BackEnd backend) -> void {
            if (isBackendAvailableRuntime(backend)) {
                available.push_back(backend);
            }
        } );
        return available;
    }
};

#endif //BACKENDENUM_H