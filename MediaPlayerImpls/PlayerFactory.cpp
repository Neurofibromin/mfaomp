//PlayerFactory.cpp
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

#include "PlayerFactory.h"
#include "VLCPlayerStruct.h"
#include "QMediaPlayerStruct.h"
#include "QWebEngineStruct.h"
#include "BackEndEnum.h"
#include "config.h"

CreatePlayerFuncPtrType PlayerFactory::ProduceChosenFactory(currentBackEnd backend) {
    CreatePlayerFuncPtrType createdFactoryFunction;
    switch (backend) {
#ifdef HAVE_LIBVLC
        case VLCPlayerBackEnd:
            createdFactoryFunction = [](const QUrl& url) -> MediaPlayerBase* { return new VLCPlayerStruct(url); };
            break;
#endif
#ifdef HAVE_QTMULTIMEDIA
        case QMediaPlayerBackEnd:
            createdFactoryFunction = [](const QUrl& url) -> MediaPlayerBase* { return new QMediaPlayerStruct(url); };
            break;
#endif
#ifdef HAVE_QTWEBENGINE
        case QWebEngineBackEnd:
            createdFactoryFunction = [](const QUrl& url) -> MediaPlayerBase* { return new QWebEngineStruct(url); };
            break;
#endif
        default:
            qWarning() << "No valid factory function for backend type: " << backend;
            return nullptr;
    }
    return createdFactoryFunction;
}
