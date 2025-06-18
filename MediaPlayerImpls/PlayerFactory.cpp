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
#include "CurrentBackEndStatusSingleton.h"
#include "config.h"

MediaPlayerBase* PlayerFactory::createPlayer(const QUrl& videoUrl) {
    switch (CurrentBackEndStatusSingleton::getInstance().getCurrentBackEnd()) {
#ifdef HAVE_LIBVLC
        case VLCPlayerBackEnd:
            qDebug() << "Creating VLCPlayerStruct for:" << videoUrl;
            return new VLCPlayerStruct(videoUrl);
#endif
#ifdef HAVE_QTMULTIMEDIA
        case QMediaPlayerBackEnd:
            qDebug() << "Creating QMediaPlayerStruct for:" << videoUrl;
            return new QMediaPlayerStruct(videoUrl);
#endif
#ifdef HAVE_QTWEBENGINE
        case QWebEngineBackEnd:
            qDebug() << "Creating QWebEngineStruct for:" << videoUrl;
            return new QWebEngineStruct(videoUrl);
#endif
        default:
            qWarning() << "No valid media player backend selected for URL:" << videoUrl;
            return nullptr;
    }
}
