#include "QMediaPlayerStruct.h"

#include <QtMultimedia>
#include <QtMultimediaWidgets>

//forward declaration
void rearrangeVideoPlayers(QGridLayout& layout, QVector<MediaPlayerBase*>& mediaPlayers);

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

void addQMediaPlayer(QGridLayout& layout, const QUrl& videoUrl, QVector<MediaPlayerBase*>& players) {
    QMediaPlayerStruct* player = new QMediaPlayerStruct(videoUrl);
    player->mediaPlayer->play();

    // Connect error signals for debugging
    QObject::connect(player->mediaPlayer, &QMediaPlayer::errorOccurred,
                     [](QMediaPlayer::Error error, const QString &errorString){
        qWarning() << "QMediaPlayer Error:" << error << "-" << errorString;
    });
    players.append(player);
    rearrangeVideoPlayers(layout, players);
}
