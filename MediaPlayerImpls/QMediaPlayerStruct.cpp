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

#include "QMediaPlayerStruct.h"

#include <QtMultimedia>
#include <QtMultimediaWidgets>
#include "BackEndEnum.h"
#include "MainWindow.h"

QMediaPlayerStruct::QMediaPlayerStruct(const QUrl& videoUrl, QWidget* parent): MediaPlayerBase(videoUrl, parent) {
    videoWidget = new QVideoWidget();
    videoWidget->setStyleSheet("background-color: black;");
    videoWidget->setMinimumSize(64, 64);
    videoWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    mediaPlayer = new QMediaPlayer();
    audioOutput = new QAudioOutput();
    mediaPlayer->setAudioOutput(audioOutput);
    mediaPlayer->setVideoOutput(videoWidget);
    mediaPlayer->setSource(videoUrl);

    QObject::connect(videoWidget, &QWidget::customContextMenuRequested, this, &MediaPlayerBase::ShowContextMenu);
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
        QObject::connect(mediaPlayer, &QMediaPlayer::mediaStatusChanged,
                         [this, time](QMediaPlayer::MediaStatus status) {
                             if (status == QMediaPlayer::BufferedMedia || status == QMediaPlayer::LoadedMedia) {
                                 mediaPlayer->setPosition(time);
                             }
                         });
    }
}

float QMediaPlayerStruct::duration() {
    return mediaPlayer->duration();
}

bool QMediaPlayerStruct::loop(std::optional<bool> set_val) {
    if (set_val.has_value()) {
        //set
        if (set_val.value()) {
            mediaPlayer->setLoops(QMediaPlayer::Infinite);
        } else {
            mediaPlayer->setLoops(1);
        }
    }
    //get
    return mediaPlayer->loops() == QMediaPlayer::Infinite;
}

QMediaPlayerStruct::~QMediaPlayerStruct() {
    QMediaPlayerStruct::clear();
}

QWidget* QMediaPlayerStruct::getVideoWidget() {
    return videoWidget;
}

QMenu* QMediaPlayerStruct::createContextMenu(QWidget* parent) {
    auto* menu = new QMenu(parent);
    menu->addAction("Play", [this] { this->play(); });
    menu->addAction("Pause", [this] { this->pause(); });
    menu->addSeparator();

    QAction* loopAction = new QAction("Loop", menu);
    loopAction->setCheckable(true);
    loopAction->setChecked(this->loop(std::nullopt));
    connect(loopAction, &QAction::toggled, this, [this](bool checked) {
        this->loop(checked);
    });
    menu->addAction(loopAction);
    menu->addSeparator();
    QMenu* conversionMenu = availableConversions(std::string("QMediaPlayer"));
    menu->addMenu(conversionMenu);
    return menu;
}
