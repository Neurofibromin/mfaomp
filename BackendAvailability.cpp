/*
    mfaomp - Multiple Files At Once Media Player
    Copyright (C) 2025  Neurofibromin

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

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
