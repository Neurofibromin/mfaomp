#ifndef BACKENDAVAILABILITY_H
#define BACKENDAVAILABILITY_H

#include "config.h"

namespace mfaomp::BackendAvailability {
        constexpr bool isVLCAvailableAtCompileTime() {
#ifdef HAVE_LIBVLC
                return true;
#else
                return false;
#endif
        }

        constexpr bool isQtMultimediaAvailableAtCompileTime() {
#ifdef HAVE_QTMULTIMEDIA
                return true;
#else
                return false;
#endif
        }

        constexpr bool isQtWebEngineAvailableAtCompileTime() {
#ifdef HAVE_QTWEBENGINE
                return true;
#else
                return false;
#endif
        }

        // These are runtime checks, declared in header, defined in .cpp
        bool isVLCAvailableAtRuntime();
        bool isQtMultimediaAvailableAtRuntime();
        bool isQtWebEngineAvailableAtRuntime();
}

#endif //BACKENDAVAILABILITY_H