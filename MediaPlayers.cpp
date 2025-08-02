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
    QWidget* videoWidget = getVideoWidget();
    if (!videoWidget) {
        return;
    }
    QMenu* contextMenu = createContextMenu(videoWidget);
    if (contextMenu) {
        contextMenu->setAttribute(Qt::WA_DeleteOnClose);
        contextMenu->popup(videoWidget->mapToGlobal(pos));
    }
}