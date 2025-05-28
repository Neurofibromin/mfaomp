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
#include <QWindow>
#include <QLabel>
#include <QtGlobal> // For Q_OS_WIN, Q_OS_LINUX, Q_OS_MACOS
#include "vlc.hpp" // uses libvlcpp from https://github.com/videolan/libvlcpp
#include <iostream>
#include <thread>

void openAndAddVideoVLC(QWidget& parent, QGridLayout& layout, QVector<VLCPlayer>& vlcPlayers) {
    QString videoFilter = "Video Files (*.mp4 *.avi *.mkv *.mov *.wmv);;All Files (*)";
    QString defaultDir = QStandardPaths::writableLocation(QStandardPaths::MoviesLocation);
    if (defaultDir.isEmpty()) defaultDir = QDir::homePath();

    QList<QUrl> videoUrls = QFileDialog::getOpenFileUrls(
        &parent, "Open Video Files", QUrl::fromLocalFile(defaultDir), videoFilter
    );

    for (const QUrl &url : videoUrls) {
        if (url.isValid())
            addVLCVideoPlayer(layout, url, vlcPlayers);
    }
}

void rearrangeVideoPlayers(QGridLayout& layout, QVector<VLCPlayer>& vlcPlayers) {
    // Clear existing widgets from the layout
    for (int i = layout.count() - 1; i >= 0; --i) {
        QLayoutItem *item = layout.takeAt(i);
        if (QWidget *widget = item->widget()) {
            // not deleting the widget here, as it's owned by VLCPlayer.
            layout.removeWidget(widget);
        }
        delete item;
    }
    // Calculate (new) grid size
    int totalPlayers = vlcPlayers.length();
    int numCols = static_cast<int>(std::ceil(std::sqrt(totalPlayers)));
    numCols = std::max(1, numCols);
    for (int i = 0; i < totalPlayers; ++i) {
        VLCPlayer& player = vlcPlayers[i];
        int row = i / numCols;
        int col = i % numCols;
        layout.addWidget(player.videoWidget, row, col);
    }
}

void addVLCVideoPlayer(QGridLayout& layout, const QUrl& videoUrl, QVector<VLCPlayer>& vlcPlayers) {
    VLCPlayer player;
    // 1. Create VLC instance
    player.vlcInstance = new VLC::Instance(0, nullptr);
    // 2. Create Media
    QString path = videoUrl.toLocalFile();
    VLC::Media *media = new VLC::Media(*player.vlcInstance, path.toUtf8().constData(), VLC::Media::FromPath);
    media->addOption(":audio-time-stretch");
    // 3. Create MediaPlayer
    player.mediaPlayer = new VLC::MediaPlayer(*player.vlcInstance);
    player.mediaPlayer->setMedia(*media);
    //TODO: Media object maybe released after setting it to the media player
    delete media;
    // 4. Create a QWidget to render into
    player.videoWidget = new QWidget;
    player.videoWidget->setAttribute(Qt::WA_NativeWindow);
    player.videoWidget->setStyleSheet("background-color: black;");
    player.videoWidget->setMinimumSize(64, 64);
    player.videoWidget->winId();
    // Set the video widget for rendering
#ifdef Q_OS_WIN
    player.mediaPlayer->setHwnd(reinterpret_cast<void*>(player.videoWidget->winId()));
#elif defined(Q_OS_MACOS)
    player.mediaPlayer->setNsobject(reinterpret_cast<void*>(player.videoWidget->winId()));
#else // only works on x11
    player.mediaPlayer->setXwindow(player.videoWidget->winId());
#endif

    player.mediaPlayer->play();
    if (!player.mediaPlayer->isPlaying()) {
        qWarning("Failed to start VLC playback.");
    }
    vlcPlayers.append(player);
    // 5. Re-layout all players
    rearrangeVideoPlayers(layout, vlcPlayers);
}
