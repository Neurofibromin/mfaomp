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


#include "VLCPlayerStruct.h"
#include "vlc.hpp"
#include <QMetaObject>

VLCPlayerStruct::VLCPlayerStruct(const QUrl& videoUrl, QWidget* parent): MediaPlayerBase(videoUrl, parent) {
    // this way looping is handled by vlc but cannot be turned off:
    // const char* cmdarg = "--input-repeat=999999";
    // vlcInstance = new VLC::Instance(1, &cmdarg);
    vlcInstance = new VLC::Instance(0, nullptr);

    m_mediaListPlayer = new VLC::MediaListPlayer(*vlcInstance);
    m_mediaPlayer = new VLC::MediaPlayer(*vlcInstance);
    m_mediaList = new VLC::MediaList(*vlcInstance);
    m_mediaListPlayer->setMediaPlayer(*m_mediaPlayer);

    // 2. Create Media
    QString path = videoUrl.toLocalFile();
    VLC::Media media (*vlcInstance, path.toUtf8().constData(), VLC::Media::FromPath);
    media.addOption(":audio-time-stretch");
    // media->addOption("--repeat"); //doesn't work
    {
        /*// 3. Create MediaPlayer
        mediaPlayer = new VLC::MediaPlayer(*vlcInstance);
        mediaPlayer->setMedia(*media);
        delete media;*/
    }
    m_mediaList->addMedia(media);
    m_mediaListPlayer->setMediaList(*m_mediaList);


    // 4. Create a QWidget to render into
    videoWidget = new QWidget();
    videoWidget->setAttribute(Qt::WA_NativeWindow);
    videoWidget->setStyleSheet("background-color: black;");
    videoWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(videoWidget, &QWidget::customContextMenuRequested, this, &MediaPlayerBase::ShowContextMenu);
    videoWidget->setMinimumSize(64, 64);
    videoWidget->winId();

#ifdef Q_OS_WIN
    m_mediaPlayer->setHwnd(reinterpret_cast<void*>(videoWidget->winId()));
#elif defined(Q_OS_MACOS)
    m_mediaPlayer->setNsobject(reinterpret_cast<void*>(videoWidget->winId()));
#else
    m_mediaPlayer->setXwindow(videoWidget->winId());
#endif

    // VLC::MediaPlayerEventManager& eventManager = mediaPlayer->eventManager();
    // m_endReachedEvent = eventManager.onEndReached([this]()-> void {
    //     if (m_isLooping) {
    //         //Problem: this doesn't work, VLC gets stuck in some ENDED state:
    //         // QMetaObject::invokeMethod(this, "play", Qt::QueuedConnection);
    //         //Problem: the below code also doesn't work
    //         std::cout << "loophandling" << std::endl;
    //         auto current = mediaPlayer->media()->mrl();
    //         VLC::Media* medianew = new VLC::Media(*vlcInstance, current, VLC::Media::FromPath);
    //         medianew->addOption(":audio-time-stretch");
    //         mediaPlayer->setMedia(*medianew);
    //         delete medianew;
    //         mediaPlayer->play();
    //         //perhaps Solution: define "--input-repeat=999999" in the ctor and if looping should be disabled stop playing the media
    //         //a short while before it ends - cannot use onEndReached because it will not trigger before restarting the loop
    //     }
    //     else
    //         std::cout << "not looping handling" << std::endl;
    // });
}

void VLCPlayerStruct::play() {
    if (m_mediaListPlayer) m_mediaListPlayer->play();
}

void VLCPlayerStruct::pause() {
    if (m_mediaListPlayer) m_mediaListPlayer->pause();
}

void VLCPlayerStruct::mute() {
    if (m_mediaPlayer) m_mediaPlayer->setMute(true);
}

void VLCPlayerStruct::unmute() {
    if (m_mediaPlayer) m_mediaPlayer->setMute(false);
}

void VLCPlayerStruct::clear() {
    if (m_mediaListPlayer) {
        m_mediaListPlayer->stop();
    }
    delete m_mediaListPlayer;
    m_mediaListPlayer = nullptr;
    delete m_mediaList;
    m_mediaList = nullptr;
    delete m_mediaPlayer;
    m_mediaPlayer = nullptr;
    delete vlcInstance;
    vlcInstance = nullptr;
    if (videoWidget) {
        videoWidget->deleteLater();
        videoWidget = nullptr;
    }
    // if (m_endReachedEvent) {
    //     m_endReachedEvent->unregister();
    //     m_endReachedEvent = nullptr;
    // }
}

float VLCPlayerStruct::speed(float sp) {
    if (m_mediaPlayer) {
        if (sp != 0.0) {
            m_mediaPlayer->setRate(sp);
        }
        return m_mediaPlayer->rate();
    }
    return 1.0f;
}

int64_t VLCPlayerStruct::time() {
    if (m_mediaPlayer) {
        return m_mediaPlayer->time();
    }
    return 0;
}

void VLCPlayerStruct::set_time(int64_t time) {
    if (m_mediaPlayer) {
        m_mediaPlayer->setTime(time);
        std::cout << "set time to: " << time << " iscorrect: " << static_cast<int64_t>(m_mediaPlayer->time()) << std::endl;
    }
}

float VLCPlayerStruct::duration() {
    if (m_mediaPlayer) {
        return m_mediaPlayer->media()->duration();
    }
    return 0;
}

bool VLCPlayerStruct::loop(std::optional<bool> set_val) {
    if (set_val.has_value()) {
        //set
        if (m_mediaListPlayer) {
            m_isLooping = set_val.value();
            if (m_isLooping) {
                m_mediaListPlayer->setPlaybackMode(libvlc_playback_mode_loop);
            } else {
                m_mediaListPlayer->setPlaybackMode(libvlc_playback_mode_default);
            }
        }
        else
            qWarning("Cannot set loop if medialistplayer null");
    }
    // get
    return m_isLooping;
}

VLCPlayerStruct::~VLCPlayerStruct() {
    VLCPlayerStruct::clear();
}

QWidget* VLCPlayerStruct::getVideoWidget() {
    return videoWidget;
}

QMenu* VLCPlayerStruct::createContextMenu() {
    return menuBuilderGeneric("VLCPlayer");
}
