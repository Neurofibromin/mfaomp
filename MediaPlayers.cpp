#include "MediaPlayers.h"
#include "PlayerFactory.h"
#include <QDebug>
#include <QUrl>


void MediaPlayerBase::ShowContextMenu(const QPoint& pos) {
    QMenu* contextMenu = createCustomContextMenu();
    if (contextMenu) {
        contextMenu->setAttribute(Qt::WA_DeleteOnClose);
        contextMenu->popup(getVideoWidget()->mapToGlobal(pos));
    }
}

QMenu* MediaPlayerBase::availableConversions(const std::string& excluded_string, QWidget* parentMenu) {
    QMenu* conversionMenu = new QMenu("Convert To", parentMenu);

    auto backends = BackEndManager::getAvailableRuntimeBackEnds();
    for (const auto backend : backends) {
        std::string backendString = BackEndManager::toString(backend);
        if (backendString == excluded_string) { // Don't show option to convert to self
            continue;
        }
        QAction* action = conversionMenu->addAction(QString::fromStdString("Convert to " + backendString));
        connect(action, &QAction::triggered, this, [this, backend]() {
            emit conversionRequested(this, backend);
        });
    }
    if (conversionMenu->isEmpty()) {
        conversionMenu->setEnabled(false);
    }
    return conversionMenu;
}

QMenu* MediaPlayerBase::menuBuilderGeneric(const std::string& excluded_string) {
    auto* menu = new QMenu(getVideoWidget());
    menu->addAction("Play", [this] { this->play(); });
    menu->addAction("Pause", [this] { this->pause(); });
    menu->addAction("Speed up", [this] { this->speed(); }); //get SPEED_INCREMENT from mainwindow
    menu->addAction("Slow down", [this] { this->speed(); }); //get SPEED_INCREMENT from mainwindow
    menu->addAction("Properties", [this] { this->pause(); }); //TODO: Later
    QAction* loopAction = new QAction("Loop", menu);
    loopAction->setCheckable(true);
    loopAction->setChecked(this->loop(std::nullopt));
    connect(loopAction, &QAction::toggled, this, [this](bool checked) {
        this->loop(checked);
    });
    menu->addAction(loopAction);
    menu->addSeparator();
    QMenu* conversionMenu = availableConversions(excluded_string, menu);
    menu->addMenu(conversionMenu);
    return menu;
}

MediaPlayerBase* Conversion::convertCurrentPlayerTo(MediaPlayerBase* currentPlayer,
                                                    BackEndManager::BackEnd desiredBackEnd, QWidget* parent) {
    if (currentPlayer == nullptr) {
        qDebug("Conversion::convertCurrentPlayerTo was called with null player as source");
        return nullptr;
    }
    std::pair<QUrl, int64_t> currentState {currentPlayer->videoUrl, currentPlayer->time()};
    auto swapperCreator = PlayerFactory::ProduceChosenFactory(desiredBackEnd);
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

