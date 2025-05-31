#ifndef MEDIAPLAYERS_H
#define MEDIAPLAYERS_H

#include <algorithm>
#include <QGridLayout>
#include <QtMultimedia>
#include <QtMultimediaWidgets>
#include <QUrl>
#include <QWebEnginePage>
#include <QWebEngineSettings>
#include <QWebEngineView>
#include <QWidget>
#include "vlc.hpp" // uses libvlcpp from https://github.com/videolan/libvlcpp

struct MediaPlayerBase {
    QUrl videoUrl;
    explicit MediaPlayerBase(const QUrl& videoUrl) : videoUrl(videoUrl) {}
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
    VLCPlayerStruct(const QUrl& videoUrl) : MediaPlayerBase(videoUrl) {
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

#endif //MEDIAPLAYERS_H
