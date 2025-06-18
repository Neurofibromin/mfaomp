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


#include "VLCPlayerStruct.h"
#include "vlc.hpp"

VLCPlayerStruct::VLCPlayerStruct(const QUrl& videoUrl): MediaPlayerBase(videoUrl) {
    vlcInstance = new VLC::Instance(0, nullptr);
    // 2. Create Media
    QString path = videoUrl.toLocalFile();
    VLC::Media* media = new VLC::Media(*vlcInstance, path.toUtf8().constData(), VLC::Media::FromPath);
    media->addOption(":audio-time-stretch");
    // 3. Create MediaPlayer
    mediaPlayer = new VLC::MediaPlayer(*vlcInstance);
    mediaPlayer->setMedia(*media);
    //TODO: Media object maybe released after setting it to the media player
    delete media;
    // 4. Create a QWidget to render into
    videoWidget = new QWidget();
    videoWidget->setAttribute(Qt::WA_NativeWindow);
    videoWidget->setStyleSheet("background-color: black;");
    videoWidget->setMinimumSize(64, 64);
    videoWidget->winId();
#ifdef Q_OS_WIN
        mediaPlayer->setHwnd(reinterpret_cast<void*>(videoWidget->winId()));
#elif defined(Q_OS_MACOS)
        mediaPlayer->setNsobject(reinterpret_cast<void*>(videoWidget->winId()));
#else
    mediaPlayer->setXwindow(videoWidget->winId());
#endif
}

void VLCPlayerStruct::play() {
    mediaPlayer->play();
}

void VLCPlayerStruct::pause() {
    mediaPlayer->pause();
}

void VLCPlayerStruct::mute() {
    mediaPlayer->setMute(true);
}

void VLCPlayerStruct::unmute() {
    mediaPlayer->setMute(false);
}

void VLCPlayerStruct::clear() {
    mediaPlayer->stop();
    delete mediaPlayer;
    delete vlcInstance;
    videoWidget->deleteLater();
}

float VLCPlayerStruct::speed(float sp) {
    if (sp != 0.0) {
        mediaPlayer->setRate(sp);
    }
    return mediaPlayer->rate();
}

int64_t VLCPlayerStruct::time() {
    return mediaPlayer->time();
}

void VLCPlayerStruct::set_time(int64_t time) {
    mediaPlayer->setTime(time);
}

float VLCPlayerStruct::duration() {
    return mediaPlayer->media()->duration();
}

VLCPlayerStruct::~VLCPlayerStruct() {
    VLCPlayerStruct::clear();
}

QWidget* VLCPlayerStruct::getVideoWidget() {
    return videoWidget;
}
