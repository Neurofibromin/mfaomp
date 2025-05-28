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

#ifndef ADDVIDEO_H
#define ADDVIDEO_H
#include <QGridLayout>
#include <QWidget>
#include <QWindow>

#include "vlc.hpp" // uses libvlcpp from https://github.com/videolan/libvlcpp

struct VLCPlayer {
    VLC::Instance *vlcInstance = nullptr;
    VLC::MediaPlayer *mediaPlayer = nullptr;
    QWidget *videoWidget = nullptr;
};

void openAndAddVideoVLC(QWidget& parent, QGridLayout& layout, QVector<VLCPlayer>& vlcPlayers);

void rearrangeVideoPlayers(QGridLayout& layout, QVector<VLCPlayer>& vlcPlayers);

void addVLCVideoPlayer(QGridLayout& layout, const QUrl& videoUrl, QVector<VLCPlayer>& vlcPlayers);

#endif //ADDVIDEO_H