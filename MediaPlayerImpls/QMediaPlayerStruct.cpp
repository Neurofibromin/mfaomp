#include "QMediaPlayerStruct.h"

#include <QtMultimedia>
#include <QtMultimediaWidgets>

QMediaPlayerStruct::QMediaPlayerStruct(const QUrl& videoUrl): MediaPlayerBase(videoUrl) {
    videoWidget = new QVideoWidget();
    videoWidget->setStyleSheet("background-color: black;");
    videoWidget->setMinimumSize(64, 64);
    mediaPlayer = new QMediaPlayer();
    audioOutput = new QAudioOutput();
    mediaPlayer->setAudioOutput(audioOutput);
    mediaPlayer->setVideoOutput(videoWidget);
    mediaPlayer->setSource(videoUrl);
}

void QMediaPlayerStruct::play() {
    mediaPlayer->play();
}

void QMediaPlayerStruct::pause() {
    mediaPlayer->pause();
}

void QMediaPlayerStruct::mute() {
    mediaPlayer->audioOutput()->setMuted(true);
}

void QMediaPlayerStruct::unmute() {
    mediaPlayer->audioOutput()->setMuted(false);
}

void QMediaPlayerStruct::clear() {
    mediaPlayer->stop();
    audioOutput->deleteLater();
    mediaPlayer->deleteLater();
    videoWidget->deleteLater();
}

float QMediaPlayerStruct::speed(float sp) {
    if (sp != 0.0) {
        mediaPlayer->setPlaybackRate(sp);
    }
    return mediaPlayer->playbackRate();
}

int64_t QMediaPlayerStruct::time() {
    return mediaPlayer->position();
}

void QMediaPlayerStruct::set_time(int64_t time) {
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

float QMediaPlayerStruct::duration() {
    return mediaPlayer->duration();
}

QMediaPlayerStruct::~QMediaPlayerStruct() {
    QMediaPlayerStruct::clear();
}

QWidget * QMediaPlayerStruct::getVideoWidget() {
    return videoWidget;
}
