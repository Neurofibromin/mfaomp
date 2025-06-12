// BackendAvailability.cpp
#include "BackendAvailability.h"
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
#include <QMediaDevices> // For checking if default audio output exists, etc.
#endif

#ifdef HAVE_QTWEBENGINE
#include <QWebEngineView>
#include <QWebEngineProfile>
#endif

namespace mfaomp::BackendAvailability {

    bool isVLCAvailableAtRuntime() {

#ifdef HAVE_LIBVLC
            libvlc_instance_t* inst = libvlc_new(0, nullptr);
            if (inst) {
                libvlc_release(inst);
                return true;
            }
#endif
            return false;

    }

    bool isQtMultimediaAvailableAtRuntime() {

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

    bool isQtWebEngineAvailableAtRuntime() {

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

}

