//VLCPlayerStruct.h
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

#ifndef VLCPLAYERSTRUCT_H
#define VLCPLAYERSTRUCT_H

#include "../MediaPlayers.h"

namespace VLC {
    //forward decl
    class Instance;
    class MediaPlayer;
}

struct VLCPlayerStruct : public MediaPlayerBase {
    VLC::Instance* vlcInstance = nullptr;
    VLC::MediaPlayer* mediaPlayer = nullptr;
    QWidget* videoWidget = nullptr;

    explicit VLCPlayerStruct(const QUrl& videoUrl, QWidget* parent);

    void play() override;

    void pause() override;

    void mute() override;

    void unmute() override;

    void clear() override;

    float speed(float sp = 0.0) override;

    int64_t time() override;

    void set_time(int64_t time) override;

    float duration() override;

    bool loop(std::optional<bool> set_val) override;

    ~VLCPlayerStruct() override;

    QWidget* getVideoWidget() override;

    QMenu* createContextMenu(QWidget* parent) override;

};

#endif //VLCPLAYERSTRUCT_H
