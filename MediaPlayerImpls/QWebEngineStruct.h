//QWebEngineStruct.h
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

#ifndef QWEBENGINESTRUCT_H
#define QWEBENGINESTRUCT_H
#include "../MediaPlayers.h"

class QWebEngineView;

struct QWebEngineStruct : public MediaPlayerBase {
    QWebEngineView* webView = nullptr;
    QWidget* videoWidget = nullptr;
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

    QWidget* getVideoWidget() override;

    QMenu* createContextMenu(QWidget* parent) override;
};

#endif //QWEBENGINESTRUCT_H
