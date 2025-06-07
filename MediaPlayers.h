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

#ifndef MEDIAPLAYERS_H
#define MEDIAPLAYERS_H

#include <QDebug>
#include <QtMultimedia>
#include <QtMultimediaWidgets>
#include <QUrl>
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
    virtual void mute() = 0;
    virtual void unmute() = 0;
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

struct QWebEngineStruct : public MediaPlayerBase {
    QWebEngineView *webView = nullptr;
    QWidget *videoWidget = nullptr;
    float lastKnownDuration = 0.0;
    int64_t lastKnownTime = 0;
    float lastKnownSpeed = 1.0;

    explicit QWebEngineStruct(const QUrl& videoUrl) : MediaPlayerBase(videoUrl) {
        webView = new QWebEngineView();
        webView->settings()->setAttribute(QWebEngineSettings::PluginsEnabled, true);
        webView->settings()->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
        videoWidget = webView;
        QString html = generateHtmlContent(videoUrl);
        webView->setHtml(html, QUrl::fromLocalFile(videoUrl.toLocalFile()));
        QObject::connect(webView, &QWebEngineView::loadFinished,  [this](bool ok) {
            if (ok) {
                qDebug() << "QWebEngineView load finished.";
                // QWebEngineStruct::duration();
                // QWebEngineStruct::time();
                // QWebEngineStruct::speed();
                waitForVideoMetadataThenFetch();
            } else {
                qWarning() << "QWebEngineView failed to load page.";
            }
        });

    }

    QString generateHtmlContent(const QUrl& videoUrl) {
        return QString(R"(
            <!DOCTYPE html>
            <html>
            <head>
                <meta charset="utf-8">
                <title>QWebEngine Video Player</title>
                <style>
                    body { margin: 0; overflow: hidden; background-color: #222; }
                    video {
                        display: block;
                        width: 100vw;
                        height: 100vh;
                        object-fit: contain;
                        background-color: black;
                    }
                </style>
            </head>
            <body>
                <video id="mediaPlayerVideo" controls loop>
                    <source src="%1" type="video/mp4">
                    Your browser does not support the video tag or the video format.
                </video>
                <script>
                    // Basic error handling for video element
                    const video = document.getElementById('mediaPlayerVideo');
                    if (video) {
                        video.onerror = function() {
                            console.error('Video loading error:', video.error.code, video.error.message);
                            // You could send a message back to C++ here if needed
                            // new QWebChannel().send('videoError', video.error.message);
                        };
                        video.onloadedmetadata = function() {
                            console.log('Video metadata loaded. Duration:', video.duration);
                        };
                    } else {
                        console.error('Video element not found!');
                    }
                </script>
            </body>
            </html>
        )").arg(videoUrl.toString()); // Use url.toString() to embed it in HTML

        /*
        QString(R"(
            <html>
            <body style="margin:0; background-color:black;">
                <video id="mediaPlayerVideo" width="100%" height="100%" controls>
                    <source src="%1" type="video/mp4">
                </video>
                <script>
                    function playVideo() {
                        document.getElementById('mediaPlayerVideo').play();
                    }
                    function pauseVideo() {
                        document.getElementById('mediaPlayerVideo').pause();
                    }
                    function setSpeed(rate) {
                        document.getElementById('mediaPlayerVideo').playbackRate = rate;
                    }
                    function setTime(t) {
                        document.getElementById('mediaPlayerVideo').currentTime = t / 1000.0;
                    }
                </script>
            </body>
            </html>
        )").arg(videoUrl.toString());
         */
    }

    void play() override {
        qDebug() << "QWebEngineStruct: play() called";
        webView->page()->runJavaScript("document.getElementById('mediaPlayerVideo').play();");
    }

    void pause() override {
        qDebug() << "QWebEngineStruct: pause() called";
        webView->page()->runJavaScript("document.getElementById('mediaPlayerVideo').pause();");
    }

    void mute() override {
        webView->page()->runJavaScript("document.getElementById('mediaPlayerVideo').muted = true;");
    }
    void unmute() override {
        webView->page()->runJavaScript("document.getElementById('mediaPlayerVideo').muted = false;");
    }

    void clear() override {
        qDebug() << "QWebEngineStruct: clear() called";
        if (!webView)
            return;
        webView->page()->runJavaScript("document.getElementById('mediaPlayerVideo').pause(); document.getElementById('mediaPlayerVideo').src = '';");
        webView->setHtml("");
        webView->deleteLater();
        webView = nullptr;
        videoWidget = nullptr;
    }

    float speed(float sp = 0.0) override {
        if (sp != 0.0) {
            QString script = QString("document.getElementById('mediaPlayerVideo').playbackRate = %1;").arg(sp);
            webView->page()->runJavaScript(script);
            lastKnownSpeed = sp;
            return sp;
        } else {
            QEventLoop loop;
            QTimer timer;
            timer.setSingleShot(true);
            timer.setInterval(100);
            float currentSpeed = lastKnownSpeed;
            webView->page()->runJavaScript("document.getElementById('mediaPlayerVideo').playbackRate;",
                                           [&currentSpeed, &loop](const QVariant& result) {
                if (result.isValid() && result.typeId() == QMetaType::Double) {
                    currentSpeed = result.toFloat();
                    qDebug() << "QWebEngineStruct: Fetched speed:" << currentSpeed;
                } else {
                    qWarning() << "QWebEngineStruct: Failed to get speed or invalid result:" << result;
                }
                loop.quit();
            });
            QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
            timer.start();
            loop.exec();
            lastKnownSpeed = currentSpeed;
            return currentSpeed;
        }
    }

    int64_t time() override {
        QEventLoop loop;
        QTimer timer;
        timer.setSingleShot(true);
        timer.setInterval(100);
        int64_t currentTimeMs = lastKnownTime;
        webView->page()->runJavaScript("document.getElementById('mediaPlayerVideo').currentTime;",
                                    [&currentTimeMs, &loop](const QVariant& result) {
                if (result.isValid() && result.typeId() == QMetaType::Double) {
                    currentTimeMs = static_cast<int64_t>(result.toDouble() /** 1000*/);
                    qDebug() << "QWebEngineStruct: Fetched time (ms):" << currentTimeMs;
                } else {
                    qWarning() << "QWebEngineStruct: Failed to get time or invalid result:" << result;
                }
                loop.quit();
            });

        QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
        timer.start();
        loop.exec();
        lastKnownTime = currentTimeMs;
        return currentTimeMs;
    }

    void set_time(int64_t time_ms) override {
        qDebug() << "QWebEngineStruct: set_time() called to (ms):" << time_ms;
        QString script = QString("document.getElementById('mediaPlayerVideo').currentTime = %1;").arg(static_cast<double>(time_ms) /*/ 1000.0*/);
        webView->page()->runJavaScript(script);
        lastKnownTime = time_ms;
    }

    void waitForVideoMetadataThenFetch() {
        QTimer* pollTimer = new QTimer(webView);
        pollTimer->setInterval(200);  // 200ms polling interval

        QObject::connect(pollTimer, &QTimer::timeout, webView, [this, pollTimer]() {
            webView->page()->runJavaScript("document.getElementById('mediaPlayerVideo').duration;",
                                           [this, pollTimer](const QVariant& result) {
                if (result.isValid() && result.canConvert<double>()) {
                    double durationVal = result.toDouble();
                    if (durationVal > 0 && std::isfinite(durationVal)) {
                        qDebug() << "QWebEngineStruct: Video duration is now available:" << durationVal;
                        lastKnownDuration = static_cast<float>(durationVal);
                        pollTimer->stop();
                        pollTimer->deleteLater();
                    } else {
                        qDebug() << "QWebEngineStruct: Duration still invalid, polling again.";
                    }
                }
            });
        });

        pollTimer->start();
    }


    float duration() override {
        QEventLoop loop;
        QTimer timer;
        timer.setSingleShot(true);
        timer.setInterval(100);

        float currentDuration = lastKnownDuration;
        webView->page()->runJavaScript("document.getElementById('mediaPlayerVideo').duration;",
                                    [&currentDuration, &loop](const QVariant& result) {
                if (result.isValid() && result.typeId() == QMetaType::Double) {
                    currentDuration = result.toFloat();
                    qDebug() << "QWebEngineStruct: Fetched duration:" << currentDuration;
                } else {
                    qWarning() << "QWebEngineStruct: Failed to get duration or invalid result:" << result;
                }
                loop.quit();
            });
        QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
        timer.start();
        loop.exec();
        lastKnownDuration = currentDuration;
        return currentDuration;
    }

    ~QWebEngineStruct() override {
        QWebEngineStruct::clear();
    }
};

#endif //MEDIAPLAYERS_H
