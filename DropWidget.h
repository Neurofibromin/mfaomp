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

#ifndef DROPWIDGET_H
#define DROPWIDGET_H

#include "AddVideo.h"
#include <QWidget>
#include <QDragEnterEvent>
#include <QMimeData>

class DropWidget : public QWidget {
    Q_OBJECT

public:
    DropWidget(QGridLayout* p_layout, QVector<MediaPlayerBase *>& p_players, QWidget* parent = nullptr)
        : QWidget(parent), video_layout(p_layout), mediaPlayers(p_players) {
        setAcceptDrops(true);
    }

signals:
    void filesDropped(const QList<QUrl>& urls);


protected:
    void dragEnterEvent(QDragEnterEvent* event) override {
        //TODO: be more granular here
        /*
        *
        if (event->mimeData()->hasFormat("text/plain"))
        // https://doc.qt.io/qt-6/qt.html#DropAction-enum
         */
        if (event->mimeData()->hasUrls()) {
            event->acceptProposedAction();
        }
    }

    void dropEvent(QDropEvent* event) override {
        QList<QUrl> urls = event->mimeData()->urls();
        // QList<QUrl> urls = event->mimeData()->urls();
        if (!urls.isEmpty()) emit filesDropped(urls);
        // for (const QUrl& url: urls) {
        //     if (url.isLocalFile()) {
        //         addVideoPlayer(*video_layout, url, mediaPlayers);
        //     }
        // }
    }

private:
    QGridLayout* video_layout;
    QVector<MediaPlayerBase *>& mediaPlayers;
};

#endif //DROPWIDGET_H
