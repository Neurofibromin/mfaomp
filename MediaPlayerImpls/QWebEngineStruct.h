#ifndef QWEBENGINESTRUCT_H
#define QWEBENGINESTRUCT_H
#include <QWebEngineView>
#include <QWebEngineSettings>
#include <QWebEngineView>
#include <QWidget>
#include "../MediaPlayers.h"
#include <QEventLoop>
#include <QGridLayout>
#include <QTimer>

struct QWebEngineStruct : public MediaPlayerBase {
    QWebEngineView *webView = nullptr;
    QWidget *videoWidget = nullptr;
    float lastKnownDuration = 0.0;
    int64_t lastKnownTime = 0;
    float lastKnownSpeed = 1.0;

    explicit QWebEngineStruct(const QUrl& videoUrl);

    QString generateHtmlContent(const QUrl& videoUrl);

    void play() override;

    void pause() override;

    void mute() override;

    void unmute() override;

    void clear() override;

    float speed(float sp = 0.0) override;

    int64_t time() override;


    void set_time(int64_t time_ms) override;

    void waitForVideoMetadataThenFetchDuration();

    float duration() override;

    ~QWebEngineStruct() override;
};

void addQWebEnginePlayer(QGridLayout &layout, const QUrl &videoUrl, QVector<MediaPlayerBase *> &players);

#endif //QWEBENGINESTRUCT_H
