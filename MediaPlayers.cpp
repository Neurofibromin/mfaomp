#include "MediaPlayers.h"
#include <cmath>
#include <iostream>
#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QGridLayout>
#include <QStandardPaths>
#include <QtGlobal>
#include <QUrl>

void MediaPlayerBase::ShowContextMenu(const QPoint& pos) {
    QWidget* ww = MediaPlayerBase::getVideoWidget();
    QMenu* menu = MediaPlayerBase::createContextMenu(ww);
    menu->popup(ww->mapToGlobal(pos));
}