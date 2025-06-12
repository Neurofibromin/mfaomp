#ifndef QWEBENGINESTRUCT_H
#define QWEBENGINESTRUCT_H
#include <QWebEngineView>
#include <QWebEngineSettings>
#include <QWebEngineView>
#include <QWidget>
#include "../MediaPlayers.h"
#include <QEventLoop>
#include <QTimer>

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
                waitForVideoMetadataThenFetchDuration();
                play();
            } else {
                qWarning() << "QWebEngineView failed to load page.";
            }
        });

    }

    QString generateHtmlContent(const QUrl& videoUrl);

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

    float speed(float sp = 0.0) override;

    int64_t time() override {
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
                    qWarning() << "QWebEngineStruct: Failed to get time or invalid result:" << result;
                }
                loop.quit();
            });

        QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
        timer.start();
        loop.exec();
        lastKnownTime = currentTimeMs;
        return lastKnownTime;
    }



    void set_time(int64_t time_ms) override {
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
        QObject::connect(webView, &QWebEngineView::loadFinished,  [this, script, time_ms](bool ok) {
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

    void waitForVideoMetadataThenFetchDuration() {
        QTimer* pollTimer = new QTimer(webView);
        pollTimer->setInterval(100);
        QObject::connect(pollTimer, &QTimer::timeout, webView, [this, pollTimer]() {
            webView->page()->runJavaScript("document.getElementById('mediaPlayerVideo').duration;",
                                           [this, pollTimer](const QVariant& result) {
                if (result.isValid() && result.canConvert<double>()) {
                    double durationVal = result.toDouble();
                    if (durationVal > 0 && std::isfinite(durationVal)) {
                        qDebug() << "QWebEngineStruct: Video duration is now available: " << durationVal << " seconds";
                        lastKnownDuration = static_cast<float>(durationVal * 1000);
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
                    currentDuration = result.toFloat() * 1000;
                    qDebug() << "QWebEngineStruct: Fetched duration:" << currentDuration;
                } else {
                    qWarning() << "QWebEngineStruct: Failed to get duration or invalid result:" << result;
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

    ~QWebEngineStruct() override {
        QWebEngineStruct::clear();
    }
};

#endif //QWEBENGINESTRUCT_H
