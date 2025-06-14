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
#include <QFile>
#include <QtGlobal>
#include <iostream>
#include <cmath>
#include "config.h"


#ifdef HAVE_LIBVLC
#include "MediaPlayerImpls/VLCPlayerStruct.h"
#include "vlc.hpp" // uses libvlcpp from https://github.com/videolan/libvlcpp
#endif
#ifdef HAVE_QTMULTIMEDIA
#include "MediaPlayerImpls/QMediaPlayerStruct.h"
#endif
#ifdef HAVE_QTWEBENGINE
#include "MediaPlayerImpls/QWebEngineStruct.h"
#endif

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
        delete item;
    }
    // Calculate (new) grid size
    int totalPlayers = mediaPlayers.length();
    int numCols = static_cast<int>(std::ceil(std::sqrt(totalPlayers))); // try to make the layout approx square
    numCols = std::max(1, numCols); // Ensure at least 1 column

    for (int i = 0; i < totalPlayers; ++i) {
        int row = i / numCols;
        int col = i % numCols;
        QWidget* videoWidget = mediaPlayers[i]->getVideoWidget();
        if (videoWidget) {
            layout.addWidget(videoWidget, row, col);
        } else {
            qWarning() << "MediaPlayerBase at index " << i << " returned a null video widget.";
        }
    }
}

void addVideoPlayer(QGridLayout& layout, const QUrl& videoUrl, QVector<MediaPlayerBase*>& mediaPlayers) {
    qDebug() << "adding video: " << videoUrl;
    switch (CurrentBackEndStatusSingleton::getInstance().getCurrentBackEnd()) {
#ifdef HAVE_LIBVLC
        case VLCPlayerBackEnd:
            addVLCVideoPlayer(layout, videoUrl, mediaPlayers);
            break;
#endif
#ifdef HAVE_QTMULTIMEDIA
        case QMediaPlayerBackEnd:
            addQMediaPlayer(layout, videoUrl, mediaPlayers);
            break;
#endif
#ifdef HAVE_QTWEBENGINE
        case QWebEngineBackEnd:
            addQWebEnginePlayer(layout, videoUrl, mediaPlayers);
            break;
#endif
    }
}