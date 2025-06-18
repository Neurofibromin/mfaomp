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

#ifndef CURRENTBACKENDSTATUSSINGLETON_H
#define CURRENTBACKENDSTATUSSINGLETON_H

#include "BackendAvailability.h"

enum currentBackEnd {
    QMediaPlayerBackEnd,
    VLCPlayerBackEnd,
    QWebEngineBackEnd
};

// Probably not best practice
// Singleton class to store the current backend status
class CurrentBackEndStatusSingleton {
public:
    static CurrentBackEndStatusSingleton& getInstance() {
        static CurrentBackEndStatusSingleton instance;
        return instance;
    }
    void setCurrentBackEnd(currentBackEnd backend) {
        currentBackend_ = backend;
    }
    currentBackEnd getCurrentBackEnd() const {
        return currentBackend_;
    }
private:
    CurrentBackEndStatusSingleton() : currentBackend_(QMediaPlayerBackEnd) {
        if constexpr (mfaomp::BackendAvailability::isVLCAvailableAtCompileTime()) {
            currentBackend_ = VLCPlayerBackEnd;
            return;
        }
        else if constexpr (mfaomp::BackendAvailability::isQtMultimediaAvailableAtCompileTime()) {
            currentBackend_ = QMediaPlayerBackEnd;
            return;
        }
        else
            currentBackend_ = QWebEngineBackEnd;
    }
    CurrentBackEndStatusSingleton(const CurrentBackEndStatusSingleton&) = delete;
    CurrentBackEndStatusSingleton& operator=(const CurrentBackEndStatusSingleton&) = delete;
    currentBackEnd currentBackend_;
};

#endif //CURRENTBACKENDSTATUSSINGLETON_H
