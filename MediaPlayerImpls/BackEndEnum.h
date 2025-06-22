//BackEndEnum.h

#ifndef BACKENDENUM_H
#define BACKENDENUM_H
#include <map>
#include <optional>
#include <string>
#include <vector>
#include <map>
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
            static constexpr bool isAvailableCompiletime() {
#ifdef HAVE_QTMULTIMEDIA
                return true;
#else
                return false;
#endif
            }
            static constexpr const char* name = "QMediaPlayerBackEnd";
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
            static constexpr bool isAvailableCompiletime() {
#ifdef HAVE_LIBVLC
                return true;
#else
                return false;
#endif
            }
            static constexpr const char* name = "VLCPlayerBackEnd";
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
            static constexpr bool isAvailableCompiletime() {
#ifdef HAVE_QTWEBENGINE
                return true;
#else
                return false;
#endif
            }
            static constexpr const char* name = "QWebEngineBackEnd";
        };
    }
}

class BackEndManager {
    BackEndManager() = delete; //make class pure static
public:
    enum class BackEnd {
        QMediaPlayer,
        VLCPlayer,
        QWebEngine
    };

    static constexpr std::array<BackEnd, 3> AllBackEnds = {
        BackEnd::QMediaPlayer,
        BackEnd::VLCPlayer,
        BackEnd::QWebEngine
    };

private:
    struct BackEndMetaData {
        using AvailabilityChecker = bool (*)();
        using NameGetter = const char* (*)();

        BackEndMetaData(AvailabilityChecker runt, AvailabilityChecker compt, NameGetter nameGetter) :
            isRunTimeAvailable(runt),
            isCompileTimeAvailable(compt),
            getName(nameGetter) {}

        AvailabilityChecker isRunTimeAvailable;
        AvailabilityChecker isCompileTimeAvailable;
        NameGetter getName;
    };

    static const constexpr std::map<BackEnd, BackEndMetaData> &  getBackEndMetaDataMap() {
        static const auto qmpName = []() { return BackEndTypes::QMediaPlayer::name; };
        static const auto vlcName = []() { return BackEndTypes::VLCPlayer::name; };
        static const auto qweName = []() { return BackEndTypes::QWebEngine::name; };
        static const std::map<BackEnd, BackEndMetaData> metaDataMap = {
            {BackEnd::QMediaPlayer, {&BackEndTypes::QMediaPlayer::isAvailableRuntime, &BackEndTypes::QMediaPlayer::isAvailableCompiletime, qmpName}},
            {BackEnd::VLCPlayer,   {&BackEndTypes::VLCPlayer::isAvailableRuntime, &BackEndTypes::VLCPlayer::isAvailableCompiletime,   vlcName}},
            {BackEnd::QWebEngine,  {&BackEndTypes::QWebEngine::isAvailableRuntime, &BackEndTypes::QWebEngine::isAvailableCompiletime,  qweName}}
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
        }
    }

public:
    static std::string toString(BackEnd backend) {
        return dispatchToType(backend, []<typename T>() { // C++20 lambda template
            return std::string(T::name);
        });
    }

    static bool constexpr isBackendAvailable(BackEnd backend) {
        if (std::is_constant_evaluated()) {
            return dispatchToType(backend, []<typename T>() {
                return T::isAvailableCompiletime();
            });
        }
        else {
            return dispatchToType(backend, []<typename T>() {
                return T::isAvailableRuntime();
            });
        }
    }

    static std::optional<BackEnd> constexpr fromString(const std::string& str) {
        const auto& map = getBackEndMetaDataMap();
        for (const auto& pair : map) {
            if (pair.second.getName() == str) {
                return pair.first;
            }
        }
        return std::nullopt;
    }

    static std::vector<BackEnd> constexpr getAvailableBackEnds() {
        std::vector<BackEnd> available;
        std::ranges::for_each(AllBackEnds, [&available](BackEnd backend) -> void {
            if (isBackendAvailable(backend)) {
                available.push_back(backend);
            }
        } );
        return available;
    }
};

#endif //BACKENDENUM_H