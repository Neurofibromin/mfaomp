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

#include "AddVideo.h"
#include <QDebug>
#include <QFileDialog>
#include <QGridLayout>
#include <QStandardPaths>
#include <QUrl>
#include <QWidget>
#include <QApplication>
#include <QMainWindow>
#include <QWebEngineView>
#include <QUrl>
#include <QFile>
#include <QDebug>
#include <QtGlobal> // For Q_OS_WIN, Q_OS_LINUX, Q_OS_MACOS
#include "vlc.hpp" // uses libvlcpp from https://github.com/videolan/libvlcpp
#include <iostream>
#include <QtMultimedia>
#include <QtMultimediaWidgets>
#include <cmath>

void openAndAddVideo(QWidget& parent, QGridLayout& layout, QVector<MediaPlayerBase*>& mediaPlayers) {
    QString videoFilter = "Video Files (*.mp4 *.avi *.mkv *.mov *.wmv);;All Files (*)";
    QString defaultDir = QStandardPaths::writableLocation(QStandardPaths::MoviesLocation);
    if (defaultDir.isEmpty()) defaultDir = QDir::homePath();
    QList<QUrl> videoUrls = QFileDialog::getOpenFileUrls(
        &parent, "Open Video Files", QUrl::fromLocalFile(defaultDir), videoFilter
    );
    for (const QUrl &url : videoUrls) {
        if (url.isValid())
            addVideoPlayer(layout, url, mediaPlayers);
    }
}

void rearrangeVideoPlayers(QGridLayout& layout, QVector<MediaPlayerBase*>& mediaPlayers) {
    for (int i = layout.count() - 1; i >= 0; --i) {
        QLayoutItem *item = layout.takeAt(i);
        if (QWidget *widget = item->widget()) {
            // not deleting the widget here, as it's owned by mediaPlayers.
            layout.removeWidget(widget);
        }
        delete item;
    }
    // Calculate (new) grid size
    int totalPlayers = mediaPlayers.length();
    int numCols = static_cast<int>(std::ceil(std::sqrt(totalPlayers)));
    numCols = std::max(1, numCols); // Ensure at least 1 column

    for (int i = 0; i < totalPlayers; ++i) {
        int row = i / numCols;
        int col = i % numCols;
        QMediaPlayerStruct* player;
        VLCPlayerStruct * player2;
        QWebEngineStruct * player3;
        switch (CurrentBackEndStatusSingleton::getInstance().getCurrentBackEnd()) {
            // maybe like this:
            // layout.addWidget(mediaPlayers[i]->videoWidget, row, col);
            case QMediaPlayerBackEnd:
                player = dynamic_cast<QMediaPlayerStruct *>( mediaPlayers[i]);
                layout.addWidget(player->videoWidget, row, col);
                break;
            case VLCPlayerBackEnd:
                player2 = dynamic_cast<VLCPlayerStruct *> (mediaPlayers[i]);
                layout.addWidget(player2->videoWidget, row, col);
                break;
            case QWebEngineBackEnd:
                player3 = dynamic_cast<QWebEngineStruct *> (mediaPlayers[i]);
                layout.addWidget(player3->webView, row, col);
                break;
        }
    }
}

void addVideoPlayer(QGridLayout& layout, const QUrl& videoUrl, QVector<MediaPlayerBase*>& mediaPlayers) {
    switch (CurrentBackEndStatusSingleton::getInstance().getCurrentBackEnd()) {
        case VLCPlayerBackEnd:
            addVLCVideoPlayer(layout, videoUrl, mediaPlayers);
            break;
        case QMediaPlayerBackEnd:
            addQMediaPlayer(layout, videoUrl, mediaPlayers);
            break;
        case QWebEngineBackEnd:
            addQWebEnginePlayer(layout, videoUrl, mediaPlayers);
            break;
    }
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

void addQMediaPlayer(QGridLayout& layout, const QUrl& videoUrl, QVector<MediaPlayerBase*>& players) {
    QMediaPlayerStruct* player = new QMediaPlayerStruct(videoUrl);
    player->mediaPlayer->play();

    // Connect error signals for debugging
    QObject::connect(player->mediaPlayer, &QMediaPlayer::errorOccurred,
                     [](QMediaPlayer::Error error, const QString &errorString){
        qWarning() << "QMediaPlayer Error:" << error << "-" << errorString;
    });

    players.append(player);
    rearrangeVideoPlayers(layout, players);
}

void addQWebEnginePlayer(QGridLayout &layout, const QUrl &videoUrl, QVector<MediaPlayerBase *> &players) {
    // The Mediaplayers can be embedded in a single web page or multiple webrenderer instances.
    // implementation will be more similar of multiple instances are created because other MediaPlayerDerived implementations
    // are independent between videos - potentially multiple videos could be playing with different backends at once potentially

    QWebEngineStruct* player = new QWebEngineStruct(videoUrl);
    player->loadVideo(videoUrl);
    player->play();
    players.append(player);
    rearrangeVideoPlayers(layout, players);
}
