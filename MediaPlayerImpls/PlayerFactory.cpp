#include "PlayerFactory.h"
#include "VLCPlayerStruct.h"
#include "QMediaPlayerStruct.h"
#include "QWebEngineStruct.h"
#include "CurrentBackEndStatusSingleton.h"
#include "config.h"

MediaPlayerBase * PlayerFactory::createPlayer(const QUrl& videoUrl) {
    switch (CurrentBackEndStatusSingleton::getInstance().getCurrentBackEnd()) {
#ifdef HAVE_LIBVLC
        case VLCPlayerBackEnd:
            qDebug() << "Creating VLCPlayerStruct for:" << videoUrl;
            return new VLCPlayerStruct(videoUrl);
#endif
#ifdef HAVE_QTMULTIMEDIA
        case QMediaPlayerBackEnd:
            qDebug() << "Creating QMediaPlayerStruct for:" << videoUrl;
            return new QMediaPlayerStruct(videoUrl);
#endif
#ifdef HAVE_QTWEBENGINE
        case QWebEngineBackEnd:
            qDebug() << "Creating QWebEngineStruct for:" << videoUrl;
            return new QWebEngineStruct(videoUrl);
#endif
        default:
            qWarning() << "No valid media player backend selected for URL:" << videoUrl;
            return nullptr;
    }
}
