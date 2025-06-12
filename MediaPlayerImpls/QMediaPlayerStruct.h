#ifndef QMEDIAPLAYERSTRUCT_H
#define QMEDIAPLAYERSTRUCT_H

#include "../MediaPlayers.h"

struct QMediaPlayerStruct : public MediaPlayerBase {
    QMediaPlayer *mediaPlayer = nullptr;
    QVideoWidget *videoWidget = nullptr; //TODO: maybe can be QWidget* and moved to base class?
    QAudioOutput* audioOutput;

    explicit QMediaPlayerStruct (const QUrl& videoUrl) : MediaPlayerBase(videoUrl) {
        videoWidget = new QVideoWidget();
        videoWidget->setStyleSheet("background-color: black;");
        videoWidget->setMinimumSize(64, 64);
        mediaPlayer = new QMediaPlayer();
        audioOutput = new QAudioOutput();
        mediaPlayer->setAudioOutput(audioOutput);
        mediaPlayer->setVideoOutput(videoWidget);
        mediaPlayer->setSource(videoUrl);
    }

    void play() override {
        mediaPlayer->play();
    }
    void pause() override {
        mediaPlayer->pause();
    }
    void mute() override {
        mediaPlayer->audioOutput()->setMuted(true);
    }
    void unmute() override {
        mediaPlayer->audioOutput()->setMuted(false);
    }
    void clear() override {
        mediaPlayer->stop();
        audioOutput->deleteLater();
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

#endif //QMEDIAPLAYERSTRUCT_H
