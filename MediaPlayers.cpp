#include "MediaPlayers.h"
#include "PlayerFactory.h"
#include <QDebug>
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

MediaPlayerBase* Conversion::convertCurrentPlayerTo(MediaPlayerBase* currentPlayer,
                                                    BackEndManager::BackEnd desiredBackEnd, QWidget* parent) {
    if (currentPlayer == nullptr) {
        qDebug("Conversion::convertCurrentPlayerTo was called with null player as source");
        return nullptr;
    }
    std::pair<QUrl, int64_t> currentState {currentPlayer->videoUrl, currentPlayer->time()};
    auto swapperCreator = PlayerFactory::ProduceChosenFactory(desiredBackEnd, parent);
    if (!swapperCreator) {
        qCritical() << "Failed to create player factory for backend:" <<
            QString::fromStdString(BackEndManager::toString(desiredBackEnd)) << ". Aborting backend change.";
        QMessageBox::critical(nullptr, "Could not convert backend", "Could not initialize selected backend at conversion: " + QString::fromStdString(BackEndManager::toString(desiredBackEnd)));
        return currentPlayer;
    }
    MediaPlayerBase* converted_player = swapperCreator(currentState.first, parent);
    converted_player->set_time(currentState.second);
    return converted_player;
}

