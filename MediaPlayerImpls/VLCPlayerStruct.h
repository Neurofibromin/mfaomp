#ifndef VLCPLAYERSTRUCT_H
#define VLCPLAYERSTRUCT_H

#include "../MediaPlayers.h"
#include "vlc.hpp" // uses libvlcpp from https://github.com/videolan/libvlcpp

struct VLCPlayerStruct : public MediaPlayerBase {
    VLC::Instance *vlcInstance = nullptr;
    VLC::MediaPlayer *mediaPlayer = nullptr;
    QWidget *videoWidget = nullptr;

    explicit VLCPlayerStruct(const QUrl& videoUrl);

    void play() override;

    void pause() override;

    void mute() override;

    void unmute() override;

    void clear() override;

    float speed(float sp = 0.0) override;

    int64_t time() override;

    void set_time(int64_t time) override;

    float duration() override;

    ~VLCPlayerStruct() override;
};

void addVLCVideoPlayer(QGridLayout& layout, const QUrl& videoUrl, QVector<MediaPlayerBase*>& vlcPlayers);

#endif //VLCPLAYERSTRUCT_H
