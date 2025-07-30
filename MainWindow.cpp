//MainWindow.cpp
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

#include "MainWindow.h"
#include "AddVideo.h"
#include "BackEndEnum.h"
#include "DropWidget.h"
#include "MediaPlayers.h"
#include "SettingsDialog.h"

#include <QApplication>
#include <QComboBox>
#include <QDebug>
#include <QFont>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenuBar>
#include <QMessageBox>
#include <QPushButton>
#include <QSlider>
#include <QStandardItemModel>
#include <QStyleFactory>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>


// const QString MainWindow::DEFAULT_SPEED_TEXT = QStringLiteral("Speed: 1.00x");
// const QString MainWindow::VLC_BACKEND_ID_TEXT = QStringLiteral("VLC Backend");
// const QString MainWindow::QMEDIAPLAYER_BACKEND_ID_TEXT = QStringLiteral("QMediaPlayer Backend");
// const QString MainWindow::QWEBENGINE_BACKEND_ID_TEXT = QStringLiteral("QWebEngine Backend");

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    initializeUI();
    initializeConnections();
    setupMenuBar();
    applyStyles();
    // setWindowTitle("mfaomp");

    setCentralWidget(m_dropWidget);
    resize(800, 600);

    auto availableBackends = BackEndManager::getAvailableRuntimeBackEnds();
    if (availableBackends.size() == 0) {
        qCritical() << "No available backends found";
        qApp->quit();
    }
    auto defaultbackend = availableBackends.front();
    setActivePlayerCreator(defaultbackend);
    std::string name = BackEndManager::toString(defaultbackend);
    QString label_text = QString::fromStdString(name);
    qDebug() << label_text << " is the default backend";
    m_backendComboBox->setCurrentText(label_text);

    /*
    // Set initial backend (must be after m_backendComboBox is created)
    if (BackEndManager::isBackendAvailableCompiletime(BackEndManager::BackEnd::VLCPlayer)) {
        setActivePlayerCreator(BackEndManager::BackEnd::VLCPlayer);
        std::string name = BackEndManager::toString(BackEndManager::BackEnd::VLCPlayer);
        // name = name + " Backend";
        QString label_text = QString::fromStdString(name);
        m_backendComboBox->setCurrentText(label_text);
    } else if (BackEndManager::isBackendAvailableCompiletime(BackEndManager::BackEnd::QMediaPlayer)) {
        setActivePlayerCreator(BackEndManager::BackEnd::QMediaPlayer);
        std::string name = BackEndManager::toString(BackEndManager::BackEnd::QMediaPlayer);
        // name = name + " Backend";
        QString label_text = QString::fromStdString(name);
        m_backendComboBox->setCurrentText(label_text);
    } else if (BackEndManager::isBackendAvailableCompiletime(BackEndManager::BackEnd::QWebEngine)) {
        setActivePlayerCreator(BackEndManager::BackEnd::QWebEngine);
        std::string name = BackEndManager::toString(BackEndManager::BackEnd::QWebEngine);
        // name = name + " Backend";
        QString label_text = QString::fromStdString(name);
        m_backendComboBox->setCurrentText(label_text);
    } else {
        qCritical() << "No media backends available!";
        qApp->quit();
    }
    */
}

MainWindow::~MainWindow() = default;

void MainWindow::initializeUI() {
    m_videoContainer = new QWidget(this);
    m_videoLayout = new QGridLayout(m_videoContainer);
    m_videoLayout->setContentsMargins(0, 0, 0, 0);
    m_videoLayout->setSpacing(10);
    m_videoContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_mediaPlayers = QVector<MediaPlayerBase *>();
    m_dropWidget = new DropWidget(m_videoLayout, m_mediaPlayers, this);
    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addWidget(m_videoContainer);

    m_seekSlider = new QSlider(Qt::Horizontal, m_dropWidget);
    m_seekSlider->setRange(0, SEEK_SLIDER_MAX_VALUE);
    m_seekSlider->setSingleStep(1);
    m_seekSlider->setPageStep(10);
    m_seekSlider->setTracking(true); // true means signal emitted while dragging

    QFrame *line = new QFrame(m_dropWidget);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);

    mainLayout->addWidget(line);
    mainLayout->addWidget(m_seekSlider);

    QWidget *controlsPanel = setupControlsPanel();

    m_backendComboBox = new QComboBox(m_dropWidget);
    populateBackendComboBox();

    QHBoxLayout *bottomRowLayout = new QHBoxLayout(); // No parent widget for layout itself initially
    bottomRowLayout->addWidget(m_backendComboBox);
    bottomRowLayout->addStretch();
    bottomRowLayout->addWidget(controlsPanel);

    mainLayout->addLayout(bottomRowLayout);

    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(10);
    m_dropWidget->setLayout(mainLayout); // Set the constructed layout on m_dropWidget
}

QWidget* MainWindow::setupControlsPanel() {
    m_addButton = new QPushButton("Add Video", m_dropWidget);
    m_pauseAllButton = new QPushButton("Pause All", m_dropWidget);
    m_unpauseAllButton = new QPushButton("Unpause All", m_dropWidget);
    m_muteAllButton = new QPushButton("Mute All", m_dropWidget);
    m_unmuteAllButton = new QPushButton("Unmute All", m_dropWidget);
    m_clearAllButton = new QPushButton("Clear All", m_dropWidget);
    m_increaseSpeedButton = new QPushButton("Speed+", m_dropWidget);
    m_decreaseSpeedButton = new QPushButton("Speed-", m_dropWidget);
    m_resetSpeedButton = new QPushButton("Speed0", m_dropWidget);

    m_speedDisplayLabel = new QLabel("1x", m_dropWidget);
    m_speedDisplayLabel->setMinimumWidth(100);
    m_speedDisplayLabel->setAlignment(Qt::AlignCenter);

    QWidget *buttonContainer = new QWidget(m_dropWidget);
    QHBoxLayout *buttonLayout = new QHBoxLayout(buttonContainer);
    buttonLayout->addWidget(m_addButton);
    buttonLayout->addWidget(m_pauseAllButton);
    buttonLayout->addWidget(m_unpauseAllButton);
    buttonLayout->addWidget(m_muteAllButton);
    buttonLayout->addWidget(m_unmuteAllButton);
    buttonLayout->addWidget(m_clearAllButton);
    buttonLayout->addWidget(m_increaseSpeedButton);
    buttonLayout->addWidget(m_decreaseSpeedButton);
    buttonLayout->addWidget(m_resetSpeedButton);
    buttonLayout->addWidget(m_speedDisplayLabel);
    buttonLayout->addStretch();
    buttonLayout->setContentsMargins(0, 0, 0, 0);
    buttonLayout->setSpacing(8);
    return buttonContainer;
}

void MainWindow::populateBackendComboBox() {
    QStandardItemModel *model = new QStandardItemModel(m_backendComboBox);
    auto enumvalues = BackEndManager::AllBackEnds;
    for (auto enumvalue: enumvalues) {
        QStandardItem *item = new QStandardItem(QString::fromStdString(BackEndManager::toString(enumvalue) /*+ " Backend"*/));
        if (!BackEndManager::isBackendAvailableRuntime(enumvalue)) {
            item->setEnabled(false);
        }
        model->appendRow(item);
    }
    m_backendComboBox->setModel(model);
}


void MainWindow::initializeConnections() {
    connectMenuItems();
    connectPlayerControlButtons();
    connectOtherControls();

    // Timer for updating seek slider
    QTimer *sliderUpdateTimer = new QTimer(this);
    sliderUpdateTimer->setInterval(SLIDER_UPDATE_INTERVAL_MS);
    connect(sliderUpdateTimer, &QTimer::timeout, this, &MainWindow::updateSeekSliderFromFirstPlayer);
    sliderUpdateTimer->start();
}

void MainWindow::connectMenuItems() {
    // Connections for menu actions are done in setupMenuBar where actions are created
}

void MainWindow::connectPlayerControlButtons() {
    connect(m_addButton, &QPushButton::clicked, this, &MainWindow::openNewVideo);
    connect(m_pauseAllButton, &QPushButton::clicked, this, &MainWindow::pauseAllPlayers);
    connect(m_unpauseAllButton, &QPushButton::clicked, this, &MainWindow::playAllPlayers);
    connect(m_muteAllButton, &QPushButton::clicked, this, &MainWindow::muteAllPlayers);
    connect(m_unmuteAllButton, &QPushButton::clicked, this, &MainWindow::unmuteAllPlayers);
    connect(m_clearAllButton, &QPushButton::clicked, this, &MainWindow::clearAllVideos);

    connect(m_increaseSpeedButton, &QPushButton::clicked, this, [this]() {
        changeAllPlayersSpeed(SPEED_INCREMENT);
    });
    connect(m_decreaseSpeedButton, &QPushButton::clicked, this, [this]() {
        changeAllPlayersSpeed(-SPEED_INCREMENT);
    });
    connect(m_resetSpeedButton, &QPushButton::clicked, this, &MainWindow::resetAllPlayersSpeed);
}

void MainWindow::connectOtherControls() {
    connect(m_seekSlider, &QSlider::sliderReleased, this, &MainWindow::handleSeekSliderReleased);
    connect(m_backendComboBox, &QComboBox::currentTextChanged, this, &MainWindow::handleBackendChanged);
    if (m_dropWidget) { // m_dropWidget is the central widget itself.
        connect(m_dropWidget, &DropWidget::filesDropped, this, &MainWindow::handleFilesDropped);
    }
}


void MainWindow::setupMenuBar() {
    m_menuBar = new QMenuBar(this); // Parent is this MainWindow
    setMenuBar(m_menuBar);

    m_fileMenu = m_menuBar->addMenu(tr("&File"));
    m_openNewVideoAction = new QAction(tr("&Open New Video..."), this);
    m_fileMenu->addAction(m_openNewVideoAction);
    m_fileMenu->addSeparator();
    m_exitAction = new QAction(tr("&Exit"), this);
    m_fileMenu->addAction(m_exitAction);

    m_editMenu = m_menuBar->addMenu(tr("&Edit"));
    m_clearAction = new QAction(tr("&Clear All"), this);
    m_editMenu->addAction(m_clearAction);

    m_optionsMenu = m_menuBar->addMenu(tr("&Options"));
    m_settingsAction = new QAction(tr("&Settings..."), this);
    m_optionsMenu->addAction(m_settingsAction);

    // Connect menu actions
    connect(m_openNewVideoAction, &QAction::triggered, this, &MainWindow::openNewVideo);
    connect(m_clearAction, &QAction::triggered, this, &MainWindow::clearAllVideos);
    connect(m_settingsAction, &QAction::triggered, this, &MainWindow::openSettingsDialog);
    connect(m_exitAction, &QAction::triggered, this, &MainWindow::exitApplication);
}

void MainWindow::applyStyles() {
    QFont font("Segoe UI", 10);
    if(m_addButton) m_addButton->setFont(font);
    if(m_pauseAllButton) m_pauseAllButton->setFont(font);
    if(m_unpauseAllButton) m_unpauseAllButton->setFont(font);
    if(m_muteAllButton) m_muteAllButton->setFont(font);
    if(m_unmuteAllButton) m_unmuteAllButton->setFont(font);
    if(m_clearAllButton) m_clearAllButton->setFont(font);
    if(m_increaseSpeedButton) m_increaseSpeedButton->setFont(font);
    if(m_decreaseSpeedButton) m_decreaseSpeedButton->setFont(font);
    if(m_resetSpeedButton) m_resetSpeedButton->setFont(font);
    if(m_backendComboBox) m_backendComboBox->setFont(font);
    // Note: Other widgets like labels, menu items might also need styling if desired.
}

void MainWindow::setActivePlayerCreator(BackEndManager::BackEnd backendType) {
    m_activePlayerCreator = PlayerFactory::ProduceChosenFactory(backendType);
    m_activeBackendType = backendType;
    // qDebug() << "Active player creator set for backend type: " << BackEndManager::toString(backendType);
    qDebug() << "Active player creator set for backend type: " << QString::fromStdString(BackEndManager::toString(backendType));
}


void MainWindow::pauseAllPlayers() {
    std::ranges::for_each(m_mediaPlayers, [](MediaPlayerBase *player) {player->pause();});
}

void MainWindow::playAllPlayers() {
    std::ranges::for_each(m_mediaPlayers, [](MediaPlayerBase *player) {player->play();});
}

void MainWindow::muteAllPlayers() {
    std::ranges::for_each(m_mediaPlayers, [](MediaPlayerBase *player) {player->mute();});
}

void MainWindow::unmuteAllPlayers() {
    std::ranges::for_each(m_mediaPlayers, [](MediaPlayerBase *player) {player->unmute();});
}

void MainWindow::changeAllPlayersSpeed(float delta) {
    if (m_mediaPlayers.isEmpty()) return;

    float newRateForDisplay = 0.0f; // Default if somehow no player gives a rate
    bool firstPlayerProcessed = false;

    for (MediaPlayerBase* player : m_mediaPlayers) {
        if (!player) continue;
        float currentRate = player->speed();
        float newRate;
        if (delta > 0) { // Increasing speed
            newRate = std::min(currentRate + delta, MAX_PLAYBACK_SPEED);
        } else { // Decreasing speed
            newRate = std::max(currentRate + delta, MIN_PLAYBACK_SPEED);
        }
        player->speed(newRate);

        if (!firstPlayerProcessed) {
            newRateForDisplay = newRate;
            firstPlayerProcessed = true;
        }
    }

    if (firstPlayerProcessed) {
        updateSpeedDisplay(newRateForDisplay);
    } else if (m_mediaPlayers.isEmpty()){ // If all players were null and removed, or list became empty
        resetPlaybackControls();
    }
}

void MainWindow::resetAllPlayersSpeed() {
    std::ranges::for_each(m_mediaPlayers, [](MediaPlayerBase *player) {player->speed(1.0f);});
    updateSpeedDisplay(1.0f);
}


// --- UI Update Slots ---
void MainWindow::updateSpeedDisplay(float rate) {
    if(m_speedDisplayLabel)
        m_speedDisplayLabel->setText(QString("Speed: %1x").arg(rate, 0, 'f', 2));
}

void MainWindow::updateSeekSliderDisplay(int64_t position, int64_t duration) {
    if (!m_seekSlider) return;

    m_seekSlider->blockSignals(true);
    if (duration > 0 && position <= duration) {
        int value = static_cast<int>((static_cast<double>(position) / duration) * SEEK_SLIDER_MAX_VALUE);
        m_seekSlider->setValue(value);
    } else {
        m_seekSlider->setValue(0);
    }
    m_seekSlider->blockSignals(false);
}

void MainWindow::resetPlaybackControls() {
    if(m_speedDisplayLabel) m_speedDisplayLabel->setText("1x");
    if(m_seekSlider) {
        m_seekSlider->blockSignals(true);
        m_seekSlider->setValue(0);
        m_seekSlider->blockSignals(false);
    }
}

void MainWindow::updateSeekSliderFromFirstPlayer() {
    if (!m_mediaPlayers.isEmpty() && m_mediaPlayers.first() != nullptr) {
        MediaPlayerBase* firstPlayer = m_mediaPlayers.first();
        updateSeekSliderDisplay(firstPlayer->time(), firstPlayer->duration());
    } else {
        resetPlaybackControls(); // Reset slider if no players or first is null
    }
}

// --- Action Slots ---
void MainWindow::handleSeekSliderReleased() {
    if (m_mediaPlayers.isEmpty() || !m_seekSlider) return;

    int sliderValue = m_seekSlider->value();
    std::ranges::for_each(m_mediaPlayers, [sliderValue, this](MediaPlayerBase *player) {int64_t duration = player->duration();
        if (duration > 0) {
            int64_t seekTo = static_cast<int64_t>(
                (static_cast<double>(sliderValue) / SEEK_SLIDER_MAX_VALUE) * duration);
            player->set_time(seekTo);
        }});
}

void MainWindow::handleBackendChanged(const QString& text) {
    qCritical() << "Selected backend: " << text;
    QVector<QPair<QUrl, int64_t>> currentlyPlayingInfo;
    for (MediaPlayerBase* player : m_mediaPlayers) {
        if (player) {
            currentlyPlayingInfo.append({player->videoUrl, player->time()});

        }
    }
    std::optional<BackEndManager::BackEnd> backendOpt = BackEndManager::fromString(text.toStdString());
    if (!backendOpt.has_value()) {
        qCritical() << "Invalid backend string";
        return;
    }
    clearAllVideos();
    BackEndManager::BackEnd newBackendType = backendOpt.value();
    if (!PlayerFactory::ProduceChosenFactory(newBackendType)) {
         qCritical() << "Failed to create player factory for backend:" << text << ". Aborting backend change.";
         m_activePlayerCreator = nullptr;
         m_activeBackendType = newBackendType; // Still update type for record.
         QMessageBox::critical(this, "Backend Error", "Could not initialize selected backend: " + text);
         return;
    }
    setActivePlayerCreator(newBackendType);

    // Re-add players
    for (const auto& videoInfo : currentlyPlayingInfo) {
        // addVideoPlayer is expected to add the new player to m_mediaPlayers
        addVideoPlayer(*m_videoLayout, videoInfo.first, m_mediaPlayers, m_activePlayerCreator);
    }

    // Pause all, then play all and set time, preserving original logic flow
    // This assumes addVideoPlayer might start them, or just for robustness.
    for (int i = 0; i < m_mediaPlayers.size(); ++i) {
        if(m_mediaPlayers.at(i)) m_mediaPlayers.at(i)->pause();
    }

    for (int i = 0; i < m_mediaPlayers.size(); ++i) {
        if (m_mediaPlayers.at(i) && i < currentlyPlayingInfo.size()) {
            m_mediaPlayers.at(i)->play();
            qDebug() << "Restoring time for" << currentlyPlayingInfo[i].first << "to:" << currentlyPlayingInfo[i].second;
            m_mediaPlayers.at(i)->set_time(currentlyPlayingInfo[i].second);
        }
    }
     if (m_mediaPlayers.isEmpty()) { // If no videos were playing, ensure UI is still reset
        resetPlaybackControls();
    }
}

void MainWindow::openNewVideo() {
    if (!m_activePlayerCreator) {
        QMessageBox::warning(this, "No Backend", "No active media backend selected or available.");
        return;
    }
    // Assumes openAndAddVideo from AddVideo.h correctly uses m_videoLayout, m_mediaPlayers, etc.
    openAndAddVideo(*this, *m_videoLayout, m_mediaPlayers, m_activePlayerCreator);
}

void MainWindow::clearAllVideos() {
    std::ranges::for_each(m_mediaPlayers, [](MediaPlayerBase *player) {player->clear();});
    m_mediaPlayers.clear();
    resetPlaybackControls();
}

void MainWindow::openSettingsDialog() {
    // Pass current settings to the dialog
    SettingsDialog settingsDialog(SPEED_INCREMENT, MIN_PLAYBACK_SPEED, MAX_PLAYBACK_SPEED, m_currentStyle, this);

    connect(&settingsDialog, &SettingsDialog::settingsAccepted,
            this, &MainWindow::updatePlaybackSettings);
    connect(&settingsDialog, &SettingsDialog::styleAccepted,
            this, &MainWindow::updateApplicationStyle);

    settingsDialog.exec();
    // Disconnect to avoid multiple connections if dialog is opened again (QDialog does this on close often)
    // Or ensure SettingsDialog uses unique connections or QScopedConnection.
    // For exec(), typically connections are fine as the dialog instance is temporary.
}

void MainWindow::updatePlaybackSettings(float newSpeedIncrement, float newMinSpeed, float newMaxSpeed) {
    SPEED_INCREMENT = newSpeedIncrement;
    MIN_PLAYBACK_SPEED = newMinSpeed;
    MAX_PLAYBACK_SPEED = newMaxSpeed;
    qDebug() << "Playback settings updated: Increment=" << SPEED_INCREMENT
             << ", Min=" << MIN_PLAYBACK_SPEED
             << ", Max=" << MAX_PLAYBACK_SPEED;
}

void MainWindow::updateApplicationStyle(const QString& newStyle) {
    if (m_currentStyle != newStyle) {
        m_currentStyle = newStyle;
        QApplication::setStyle(QStyleFactory::create(newStyle));
        qDebug() << "Application style updated to: " << newStyle;
    }
}

void MainWindow::handleFilesDropped(const QList<QUrl>& urls) {
    if (!m_activePlayerCreator) {
        QMessageBox::warning(this, "No Backend", "Cannot add videos: No active media backend.");
        return;
    }
    for (const QUrl& url : urls) {
        if (url.isLocalFile()) {
            // Assumes addVideoPlayer from AddVideo.h
            addVideoPlayer(*m_videoLayout, url, m_mediaPlayers, m_activePlayerCreator);
        }
    }
}

void MainWindow::exitApplication() {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, tr("Exit"), tr("Are you sure you want to exit?"),
                                  QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        qApp->quit();
    }
}