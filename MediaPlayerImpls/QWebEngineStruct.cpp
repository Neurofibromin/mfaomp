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

#include "QWebEngineStruct.h"
#include <QWebEngineSettings>
#include <QWebEngineView>
#include <QWidget>
#include <QGridLayout>
#include <QTimer>
#include <QEventLoop>


QWebEngineStruct::QWebEngineStruct(const QUrl& videoUrl, QWidget* parent): MediaPlayerBase(videoUrl, parent) {
    webView = new QWebEngineView();
    webView->settings()->setAttribute(QWebEngineSettings::PluginsEnabled, true);
    webView->settings()->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
    videoWidget = webView;
    videoWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    QString html = generateHtmlContent(videoUrl);
    webView->setHtml(html, QUrl::fromLocalFile(videoUrl.toLocalFile()));
    QObject::connect(webView, &QWebEngineView::loadFinished, [this](bool ok) {
        if (ok) {
            qDebug() << "QWebEngineView load finished.";
            waitForVideoMetadataThenFetchDuration();
            play();
        } else {
            qWarning() << "QWebEngineView failed to load page.";
        }
    });

    QObject::connect(videoWidget, &QWidget::customContextMenuRequested, this, &MediaPlayerBase::ShowContextMenu);
}

void QWebEngineStruct::play() {
    qDebug() << "QWebEngineStruct: play() called";
    webView->page()->runJavaScript("document.getElementById('mediaPlayerVideo').play();");
}

void QWebEngineStruct::pause() {
    qDebug() << "QWebEngineStruct: pause() called";
    webView->page()->runJavaScript("document.getElementById('mediaPlayerVideo').pause();");
}

void QWebEngineStruct::mute() {
    webView->page()->runJavaScript("document.getElementById('mediaPlayerVideo').muted = true;");
}

void QWebEngineStruct::unmute() {
    webView->page()->runJavaScript("document.getElementById('mediaPlayerVideo').muted = false;");
}

void QWebEngineStruct::clear() {
    qDebug() << "QWebEngineStruct: clear() called";
    if (!webView)
        return;
    webView->page()->runJavaScript(
        "document.getElementById('mediaPlayerVideo').pause(); document.getElementById('mediaPlayerVideo').src = '';");
    webView->setHtml("");
    webView->deleteLater();
    webView = nullptr;
    videoWidget = nullptr;
}

float QWebEngineStruct::speed(float sp) {
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
                                               qWarning() << "QWebEngineStruct: Failed to get speed or invalid result:"
                                                       << result;
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

int64_t QWebEngineStruct::time() {
    QEventLoop loop;
    QTimer timer;
    timer.setSingleShot(true);
    timer.setInterval(100);
    int64_t currentTimeMs = lastKnownTime;
    webView->page()->runJavaScript("document.getElementById('mediaPlayerVideo').currentTime;",
                                   [&currentTimeMs, &loop](const QVariant& result) {
                                       if (result.isValid() && result.typeId() == QMetaType::Double) {
                                           currentTimeMs = static_cast<int64_t>(result.toDouble() * 1000);
                                           qDebug() << "QWebEngineStruct: Fetched time (ms):" << currentTimeMs;
                                       } else {
                                           qWarning() << "QWebEngineStruct: Failed to get time or invalid result:" <<
                                                   result;
                                       }
                                       loop.quit();
                                   });

    QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    timer.start();
    loop.exec();
    lastKnownTime = currentTimeMs;
    return lastKnownTime;
}

void QWebEngineStruct::set_time(int64_t time_ms) {
    qDebug() << "QWebEngineStruct: set_time() called to (ms):" << time_ms;
    double time_s = static_cast<double>(time_ms) / 1000.0;
    QString script = QString(R"(
            (function(){
                const video = document.getElementById('mediaPlayerVideo');
                function setVideoTime(seconds) {
                    if (!video) return;
                    if (video.readyState >= 1) {
                        video.currentTime = seconds;
                        console.log('Set video.currentTime to', seconds);
                    } else {
                        video.addEventListener('loadedmetadata', function handler() {
                            video.currentTime = seconds;
                            console.log('Set video.currentTime after metadata:', seconds);
                            video.removeEventListener('loadedmetadata', handler);
                        });
                        console.log('Deferring set time to', seconds);
                    }
                }
                setVideoTime(%1);
            })();
        )").arg(time_s);
    // maybe not loaded, in which case must wait for the loadFinished signal, but if loaded already, then just execute it as is.
    QObject::connect(webView, &QWebEngineView::loadFinished, [this, script, time_ms](bool ok) {
        if (ok) {
            qDebug() << "QWebEngineView load finished.";
            webView->page()->runJavaScript(script);
            lastKnownTime = time_ms; // Optimistically update, but actual time may be different
            play();
        } else {
            qWarning() << "QWebEngineView failed to load page.";
        }
    });
    webView->page()->runJavaScript(script);
    lastKnownTime = time_ms; // Optimistically update, but actual time may be different
}

void QWebEngineStruct::waitForVideoMetadataThenFetchDuration() {
    QTimer* pollTimer = new QTimer(webView);
    pollTimer->setInterval(100);
    QObject::connect(pollTimer, &QTimer::timeout, webView, [this, pollTimer]() {
        webView->page()->runJavaScript("document.getElementById('mediaPlayerVideo').duration;",
                                       [this, pollTimer](const QVariant& result) {
                                           if (result.isValid() && result.canConvert<double>()) {
                                               double durationVal = result.toDouble();
                                               if (durationVal > 0 && std::isfinite(durationVal)) {
                                                   qDebug() << "QWebEngineStruct: Video duration is now available: " <<
                                                           durationVal << " seconds";
                                                   lastKnownDuration = static_cast<float>(durationVal * 1000);
                                                   pollTimer->stop();
                                                   pollTimer->deleteLater();
                                               } else {
                                                   qDebug() <<
                                                           "QWebEngineStruct: Duration still invalid, polling again.";
                                               }
                                           }
                                       });
    });
    pollTimer->start();
}

float QWebEngineStruct::duration() {
    QEventLoop loop;
    QTimer timer;
    timer.setSingleShot(true);
    timer.setInterval(100);

    float currentDuration = lastKnownDuration;
    webView->page()->runJavaScript("document.getElementById('mediaPlayerVideo').duration;",
                                   [&currentDuration, &loop](const QVariant& result) {
                                       if (result.isValid() && result.typeId() == QMetaType::Double) {
                                           currentDuration = result.toFloat() * 1000;
                                           qDebug() << "QWebEngineStruct: Fetched duration:" << currentDuration;
                                       } else {
                                           qWarning() << "QWebEngineStruct: Failed to get duration or invalid result:"
                                                   << result;
                                       }
                                       loop.quit();
                                   });
    QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    timer.start();
    loop.exec();
    if (currentDuration > 0 && std::isfinite(currentDuration))
        lastKnownDuration = currentDuration;
    return currentDuration;
}

QWebEngineStruct::~QWebEngineStruct() {
    QWebEngineStruct::clear();
}

QWidget* QWebEngineStruct::getVideoWidget() {
    return webView;
}

QString QWebEngineStruct::generateHtmlContent(const QUrl& videoUrl) {
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
        )").arg(videoUrl.toString());
}

QMenu* QWebEngineStruct::createContextMenu(QWidget* parent) {
    auto* menu = new QMenu(parent);
    menu->addAction("QWebEngine: Play", this, &QWebEngineStruct::play);
    menu->addAction("QWebEngine: Pause", this, &QWebEngineStruct::pause);
    return menu;
}