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
