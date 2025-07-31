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

#ifndef ADDVIDEO_H
#define ADDVIDEO_H
#include "MediaPlayers.h"
#include <QGridLayout>
#include <functional>
#include <QFileDialog>

using CreatePlayerFuncPtrType = MediaPlayerBase* (*)(const QUrl&);

// Signature for the file dialog function
using GetOpenFileUrlsFuncType = std::function<QList<QUrl>(
    QWidget* parent,
    const QString& caption,
    const QUrl& dir,
    const QString& filter,
    QString* selectedFilter,
    QFileDialog::Options options,
    const QStringList& supportedSchemes
)>;

void openAndAddVideo(QWidget& parentWidget,
                     QGridLayout& layout,
                     QVector<MediaPlayerBase*>& mediaPlayers,
                     CreatePlayerFuncPtrType createPlayerFunc,
                     const GetOpenFileUrlsFuncType& getUrlsFunc = QFileDialog::getOpenFileUrls);

void rearrangeVideoPlayers(QGridLayout& layout, QVector<MediaPlayerBase *>& mediaPlayers);

void addVideoPlayer(QGridLayout& layout, const QUrl& videoUrl, QVector<MediaPlayerBase *>& mediaPlayers, CreatePlayerFuncPtrType createPlayerFunc);

#endif //ADDVIDEO_H
