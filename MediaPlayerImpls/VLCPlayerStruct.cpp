#include "VLCPlayerStruct.h"
#include "vlc.hpp"
//forward declaration
void rearrangeVideoPlayers(QGridLayout& layout, QVector<MediaPlayerBase*>& mediaPlayers);

VLCPlayerStruct::VLCPlayerStruct(const QUrl& videoUrl): MediaPlayerBase(videoUrl) {
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

QWidget * VLCPlayerStruct::getVideoWidget() {
    return videoWidget;
}

void addVLCVideoPlayer(QGridLayout& layout, const QUrl& videoUrl, QVector<MediaPlayerBase*>& vlcPlayers) {
    VLCPlayerStruct* player = new VLCPlayerStruct(videoUrl);
    player->mediaPlayer->play();
    if (!player->mediaPlayer->isPlaying()) {
        qWarning("Failed to start VLC playback.");
    }
    vlcPlayers.append(player);
    rearrangeVideoPlayers(layout, vlcPlayers);
}
