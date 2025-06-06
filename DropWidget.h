#ifndef DROPWIDGET_H
#define DROPWIDGET_H

#include <QWidget>
#include <QDropEvent>
#include <QMimeData>
#include <QUrl>
#include <QGridLayout>
#include <QVector>

#include "AddVideo.h"
#include "MediaPlayers.h"

class DropWidget : public QWidget {
    Q_OBJECT
public:
    DropWidget(QGridLayout* p_layout, QVector<MediaPlayerBase*>& p_players, QWidget* parent = nullptr)
        : QWidget(parent), layout(p_layout), mediaPlayers(p_players) {
        setAcceptDrops(true);
    }

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
        for (const QUrl& url : urls) {
            if (url.isLocalFile()) {
                addVideoPlayer(*layout, url, mediaPlayers);
            }
        }
    }

private:
    QGridLayout* layout;
    QVector<MediaPlayerBase*>& mediaPlayers;
};

#endif //DROPWIDGET_H
