#ifndef VLCPLAYERSTRUCT_H
#define VLCPLAYERSTRUCT_H

#include "../MediaPlayers.h"
#include "vlc.hpp" // uses libvlcpp from https://github.com/videolan/libvlcpp

struct VLCPlayerStruct : public MediaPlayerBase {
    VLC::Instance *vlcInstance = nullptr;
    VLC::MediaPlayer *mediaPlayer = nullptr;
    QWidget *videoWidget = nullptr;

    explicit VLCPlayerStruct(const QUrl& videoUrl) : MediaPlayerBase(videoUrl) {
        vlcInstance = new VLC::Instance(0, nullptr);
        // 2. Create Media
        QString path = videoUrl.toLocalFile();
        VLC::Media *media = new VLC::Media(*vlcInstance, path.toUtf8().constData(), VLC::Media::FromPath);
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
    void play() override {
        mediaPlayer->play();
    }
    void pause() override {
        mediaPlayer->pause();
    }
    void mute() override {
        mediaPlayer->setMute(true);
    }
    void unmute() override {
        mediaPlayer->setMute(false);
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

#endif //VLCPLAYERSTRUCT_H
