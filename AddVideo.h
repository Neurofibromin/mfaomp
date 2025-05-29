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
#include <algorithm>
#include <cmath>
#include <QGridLayout>
#include <QtMultimedia>
#include <QtMultimediaWidgets>
#include <QUrl>
#include <QWidget>
#include <thread>
#include "AddVideo.h"
#include "vlc.hpp" // uses libvlcpp from https://github.com/videolan/libvlcpp

enum currentBackEnd {
    QMediaPlayerBackEnd,
    VLCPlayerBackEnd
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
    CurrentBackEndStatusSingleton() : currentBackend_(VLCPlayerBackEnd) {}
    CurrentBackEndStatusSingleton(const CurrentBackEndStatusSingleton&) = delete;
    CurrentBackEndStatusSingleton& operator=(const CurrentBackEndStatusSingleton&) = delete;
    currentBackEnd currentBackend_;
};

struct MediaPlayerBase {
    QUrl videoUrl;
    virtual ~MediaPlayerBase() {} // for polymorphism
    virtual void play() = 0;
    virtual void pause() = 0;
    virtual void clear() = 0;
    virtual float speed(float sp = 0.0) = 0;
    virtual int64_t time() = 0;
    virtual void set_time(int64_t time) = 0;
    virtual float duration() = 0;
};

struct VLCPlayerStruct : public MediaPlayerBase {
    VLC::Instance *vlcInstance = nullptr;
    VLC::MediaPlayer *mediaPlayer = nullptr;
    QWidget *videoWidget = nullptr;
    void play() override {
        mediaPlayer->play();
    }
    void pause() override {
        mediaPlayer->pause();
    }
    void clear() override {
        mediaPlayer->stop();
        delete mediaPlayer;
        delete vlcInstance;
        videoWidget->deleteLater();
    }
    float speed(float sp = 0.0) override {
        if (sp != 0.0) {
            mediaPlayer->setRate(sp);
        }
        return mediaPlayer->rate();
    }
    int64_t time() override {
        return mediaPlayer->time();
    }
    void set_time(int64_t time) override {
        mediaPlayer->setTime(time);
    }
    float duration() override {
        return mediaPlayer->media()->duration();
    }
    ~VLCPlayerStruct() override {
        VLCPlayerStruct::clear();
    }
};

struct QMediaPlayerStruct : public MediaPlayerBase {
    QMediaPlayer *mediaPlayer = nullptr;
    QVideoWidget *videoWidget = nullptr;
    QAudioOutput* audioOutput;
    void play() override {
        mediaPlayer->play();
    }
    void pause() override {
        mediaPlayer->pause();
    }
    void clear() override {
        mediaPlayer->stop();
        mediaPlayer->deleteLater();
        videoWidget->deleteLater();
    }
    float speed(float sp = 0.0) override {
        if (sp != 0.0) {
            mediaPlayer->setPlaybackRate(sp);
        }
        return mediaPlayer->playbackRate();
    }
    int64_t time() override {
        return mediaPlayer->position();
    }
    void set_time(int64_t time) override {
        if (mediaPlayer->mediaStatus() == QMediaPlayer::BufferedMedia ||
            mediaPlayer->mediaStatus() == QMediaPlayer::LoadedMedia) {
            mediaPlayer->setPosition(time);
            } else {
                QObject::connect(mediaPlayer, &QMediaPlayer::mediaStatusChanged, [this, time](QMediaPlayer::MediaStatus status) {
                    if (status == QMediaPlayer::BufferedMedia || status == QMediaPlayer::LoadedMedia) {
                        mediaPlayer->setPosition(time);
                    }
                });
            }
    }
    float duration() override {
        return mediaPlayer->duration();
    }
    ~QMediaPlayerStruct() override {
        QMediaPlayerStruct::clear();
    }
};

void openAndAddVideo(QWidget& parent, QGridLayout& layout, QVector<MediaPlayerBase*>& mediaPlayers);
void rearrangeVideoPlayers(QGridLayout& layout, QVector<MediaPlayerBase*>& mediaPlayers);
void addVideoPlayer(QGridLayout& layout, const QUrl& videoUrl, QVector<MediaPlayerBase*>& mediaPlayers);
void addVLCVideoPlayer(QGridLayout& layout, const QUrl& videoUrl, QVector<MediaPlayerBase*>& vlcPlayers);
void addQMediaPlayer(QGridLayout& layout, const QUrl& videoUrl, QVector<MediaPlayerBase*>& players);

#endif //ADDVIDEO_H