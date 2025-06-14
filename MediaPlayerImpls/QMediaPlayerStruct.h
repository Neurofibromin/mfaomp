#ifndef QMEDIAPLAYERSTRUCT_H
#define QMEDIAPLAYERSTRUCT_H

#include "../MediaPlayers.h"
#include <QtMultimedia>
#include <QtMultimediaWidgets>

struct QMediaPlayerStruct : public MediaPlayerBase {
    QMediaPlayer *mediaPlayer = nullptr;
    QVideoWidget *videoWidget = nullptr; //TODO: maybe can be QWidget* and moved to base class?
    QAudioOutput* audioOutput;

    explicit QMediaPlayerStruct (const QUrl& videoUrl);

    void play() override;

    void pause() override;

    void mute() override;

    void unmute() override;

    void clear() override;

    float speed(float sp = 0.0) override;

    int64_t time() override;

    void set_time(int64_t time) override;

    float duration() override;

    ~QMediaPlayerStruct() override;

    QWidget* getVideoWidget() override;
};


void addQMediaPlayer(QGridLayout& layout, const QUrl& videoUrl, QVector<MediaPlayerBase*>& players);

#endif //QMEDIAPLAYERSTRUCT_H
