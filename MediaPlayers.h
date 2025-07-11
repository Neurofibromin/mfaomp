//MediaPlayers.h
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

#ifndef MEDIAPLAYERS_H
#define MEDIAPLAYERS_H

#include <QUrl>
#include <QWidget>

struct MediaPlayerBase {
    QUrl videoUrl;

    explicit MediaPlayerBase(const QUrl& videoUrl) : videoUrl(videoUrl) {
    }

    virtual ~MediaPlayerBase() {} // for polymorphism
    virtual void play() = 0;

    virtual void pause() = 0;

    virtual void mute() = 0;

    virtual void unmute() = 0;

    virtual void clear() = 0;

    virtual float speed(float sp = 0.0) = 0;

    virtual int64_t time() = 0;

    virtual void set_time(int64_t time) = 0;

    virtual float duration() = 0;

    virtual QWidget* getVideoWidget() = 0;
};

#endif //MEDIAPLAYERS_H
