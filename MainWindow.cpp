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

#include "AddVideo.h"
#include "BackEndEnum.h"
#include "BackendAvailability.h"
#include "MainWindow.h"
#include "PlayerFactory.h"
#include <QApplication>
#include <QComboBox>
#include <QDebug>
#include <QFont>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QSlider>
#include <QStandardItemModel>
#include <QStyleFactory>
#include <QWidget>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), mediaPlayers() {
    createWidgets();
    applyStyles();
    makeConnections();
    setCentralWidget(centralWidget);
    resize(800, 600);
    setupMenuBar();
}

MainWindow::~MainWindow() = default;

void MainWindow::createWidgets() {
    videoContainer = new QWidget(this);
    videoLayout = new QGridLayout(videoContainer);
    videoLayout->setContentsMargins(0, 0, 0, 0);
    videoLayout->setSpacing(10);
    videoContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    centralWidget = new DropWidget(videoLayout, mediaPlayers, this);
    addButton = new QPushButton("Add Video", this);
    pauseAllButton = new QPushButton("Pause All", this);
    unpauseAllButton = new QPushButton("Unpause All", this);
    muteAllButton = new QPushButton("Mute All", this);
    unmuteAllButton = new QPushButton("Unmute All", this);
    clearAllButton = new QPushButton("Clear All", this);
    increaseSpeedButton = new QPushButton("Speed+", this);
    decreaseSpeedButton = new QPushButton("Speed-", this);
    resetSpeedButton = new QPushButton("Speed0", this);
    speedDisplayLabel = new QLabel("Speed: 1.00x", this);
    speedDisplayLabel->setMinimumWidth(100);
    speedDisplayLabel->setAlignment(Qt::AlignCenter);

    seekSlider = new QSlider(Qt::Horizontal, this);
    seekSlider->setRange(0, 1000);
    seekSlider->setSingleStep(1);
    seekSlider->setPageStep(10);
    seekSlider->setTracking(true);

    backendComboBox = new QComboBox(centralWidget);
    QStandardItemModel *model = new QStandardItemModel(this);
    QStandardItem *qWebEngineItem = new QStandardItem("QWebEngine Backend");
    if (not mfaomp::BackendAvailability::isQtWebEngineAvailableAtRuntime())
        qWebEngineItem->setEnabled(false);
    QStandardItem *qMediaPlayerItem = new QStandardItem("QMediaPlayer Backend");
    if (not mfaomp::BackendAvailability::isQtMultimediaAvailableAtRuntime())
        qMediaPlayerItem->setEnabled(false);
    QStandardItem *vlcItem = new QStandardItem("VLC Backend");
    if (not mfaomp::BackendAvailability::isVLCAvailableAtRuntime())
        vlcItem->setEnabled(false);
    model->appendRow(vlcItem);
    model->appendRow(qMediaPlayerItem);
    model->appendRow(qWebEngineItem);
    backendComboBox->setModel(model);

    if (mfaomp::BackendAvailability::isVLCAvailableAtRuntime()) {
        setActivePlayerCreator(VLCPlayerBackEnd);
        backendComboBox->setCurrentText("VLC Backend");
    } else if (mfaomp::BackendAvailability::isQtMultimediaAvailableAtRuntime()) {
        setActivePlayerCreator(QMediaPlayerBackEnd);
        backendComboBox->setCurrentText("QMediaPlayer Backend");
    } else if (mfaomp::BackendAvailability::isQtWebEngineAvailableAtRuntime()) {
        setActivePlayerCreator(QWebEngineBackEnd);
        backendComboBox->setCurrentText("QWebEngine Backend");
    } else {
        setActivePlayerCreator(QMediaPlayerBackEnd);
        qCritical() << "No media backends available!";
    }

    QWidget *buttonContainer = new QWidget(centralWidget);
    QHBoxLayout *buttonLayout = new QHBoxLayout(buttonContainer);
    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(pauseAllButton);
    buttonLayout->addWidget(unpauseAllButton);
    buttonLayout->addWidget(muteAllButton);
    buttonLayout->addWidget(unmuteAllButton);
    buttonLayout->addWidget(clearAllButton);
    buttonLayout->addWidget(increaseSpeedButton);
    buttonLayout->addWidget(decreaseSpeedButton);
    buttonLayout->addWidget(resetSpeedButton);
    buttonLayout->addWidget(speedDisplayLabel);
    buttonLayout->addStretch();
    buttonLayout->setContentsMargins(0, 0, 0, 0);
    buttonLayout->setSpacing(8);
    buttonContainer->setLayout(buttonLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->addWidget(videoContainer);
    QFrame *line = new QFrame(centralWidget);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    mainLayout->addWidget(line);
    mainLayout->addWidget(seekSlider);

    QHBoxLayout *bottomRowLayout = new QHBoxLayout(centralWidget);
    bottomRowLayout->addWidget(backendComboBox);
    bottomRowLayout->addStretch();
    bottomRowLayout->addWidget(buttonContainer);

    mainLayout->addLayout(bottomRowLayout);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(10);
    centralWidget->setLayout(mainLayout);
}

void MainWindow::makeConnections() {
    QObject::connect(addButton, &QPushButton::clicked, this, &MainWindow::openNewVideo);

    QObject::connect(pauseAllButton, &QPushButton::clicked, [&]() {
        for (auto& p: mediaPlayers)
            p->pause();
    });

    QObject::connect(unpauseAllButton, &QPushButton::clicked, [&]() {
        for (auto& p: mediaPlayers)
            p->play();
    });

    QObject::connect(muteAllButton, &QPushButton::clicked, [&]() {
        for (auto& p: mediaPlayers)
            p->mute();
    });

    QObject::connect(unmuteAllButton, &QPushButton::clicked, [&]() {
        for (auto& p: mediaPlayers)
            p->unmute();
    });

    QObject::connect(clearAllButton, &QPushButton::clicked, this, &MainWindow::clearAllVideos);

    QObject::connect(increaseSpeedButton, &QPushButton::clicked, [&]() {
        for (auto& p: mediaPlayers) {
            float currentRate = p->speed();
            float newRate = std::min(currentRate + SPEED_INCREMENT, MAX_PLAYBACK_SPEED);
            p->speed(newRate);
            if (&p == &mediaPlayers.first()) {
                updateSpeedDisplay(newRate);
            }
        }
    });

    QObject::connect(decreaseSpeedButton, &QPushButton::clicked, [&]() {
        for (auto& p: mediaPlayers) {
            float currentRate = p->speed();
            float newRate = std::max(currentRate - SPEED_INCREMENT, MIN_PLAYBACK_SPEED);
            p->speed(newRate);
            if (&p == &mediaPlayers.first()) {
                updateSpeedDisplay(newRate);
            }
        }
    });

    QObject::connect(resetSpeedButton, &QPushButton::clicked, [&]() {
        for (auto& p: mediaPlayers) {
            p->speed(1.0f);
        }
        updateSpeedDisplay(1.0f);
    });

    connect(seekSlider, &QSlider::sliderReleased, this, &MainWindow::handleSeekSliderReleased);
    connect(backendComboBox, &QComboBox::currentTextChanged, this, &MainWindow::handleBackendChanged);

    QTimer *sliderUpdateTimer = new QTimer(this);
    sliderUpdateTimer->setInterval(500);
    QObject::connect(sliderUpdateTimer, &QTimer::timeout, [&]() {
        if (!mediaPlayers.isEmpty()) {
            auto player = mediaPlayers.first();
            int64_t pos = player->time();
            int64_t length = player->duration();
            if (length > 0) {
                int value = static_cast<int>((double(pos) / length) * 1000);
                seekSlider->blockSignals(true);
                seekSlider->setValue(value);
                seekSlider->blockSignals(false);
            }
        }
    });
    sliderUpdateTimer->start();

    connect(centralWidget, &DropWidget::filesDropped, this, &MainWindow::handleFilesDropped);
}

void MainWindow::setupMenuBar() {
    menuBar = new QMenuBar(this);
    setMenuBar(menuBar);

    fileMenu = menuBar->addMenu("&File"); // &File creates shortcut
    openNewVideoAction = new QAction("&Open New Video...", this);
    fileMenu->addAction(openNewVideoAction);
    fileMenu->addSeparator();
    exitAction = new QAction("&Exit", this);
    fileMenu->addAction(exitAction);

    editMenu = menuBar->addMenu("&Edit");
    clearAction = new QAction("&Clear All", this);
    editMenu->addAction(clearAction);

    optionsMenu = menuBar->addMenu("&Options");
    settingsAction = new QAction("&Settings...", this);
    optionsMenu->addAction(settingsAction);

    connect(openNewVideoAction, &QAction::triggered, this, &MainWindow::openNewVideo);
    connect(clearAction, &QAction::triggered, this, &MainWindow::clearAllVideos);
    connect(settingsAction, &QAction::triggered, this, &MainWindow::openSettings);
    connect(exitAction, &QAction::triggered, this, &MainWindow::exitApplication);
}

void MainWindow::applyStyles() {
    QFont font("Segoe UI", 10);
    addButton->setFont(font);
    pauseAllButton->setFont(font);
    unpauseAllButton->setFont(font);
    muteAllButton->setFont(font);
    unmuteAllButton->setFont(font);
    clearAllButton->setFont(font);
    increaseSpeedButton->setFont(font);
    decreaseSpeedButton->setFont(font);
    resetSpeedButton->setFont(font);
    backendComboBox->setFont(font);
}

void MainWindow::setActivePlayerCreator(currentBackEnd backendType) {
    activePlayerCreator = PlayerFactory::ProduceChosenFactory(backendType);
    activeBackendType = backendType;
}

void MainWindow::updateSpeedDisplay(float rate) {
    speedDisplayLabel->setText(QString("Speed: %1x").arg(rate, 0, 'f', 2));
}

void MainWindow::updateSeekSlider(int64_t position, int64_t duration) {
    if (position <= duration) {
        int value = static_cast<int>((double(position) / duration) * 1000);
        seekSlider->blockSignals(true);
        seekSlider->setValue(value);
        seekSlider->blockSignals(false);
    } else {
        seekSlider->blockSignals(true);
        seekSlider->setValue(0);
        seekSlider->blockSignals(false);
    }
}

void MainWindow::resetUIOnPlayersCleared() const {
    speedDisplayLabel->setText(QString("Speed: 1.00x"));
    seekSlider->blockSignals(true);
    seekSlider->setValue(0);
    seekSlider->blockSignals(false);
}

void MainWindow::handleSeekSliderReleased() {
    if (!mediaPlayers.isEmpty()) {
        int value = seekSlider->value();
        for (auto& p: mediaPlayers) {
            int64_t length = p->duration();
            int64_t seekTo = static_cast<int64_t>((double(value) / 1000.0) * length);
            p->set_time(seekTo);
        }
    }
}

void MainWindow::handleBackendChanged(const QString& text) {
    qDebug() << "Selected backend: " << text.toStdString();
    QVector<QPair<QUrl, int64_t> > currently_playing;
    for (auto& p: mediaPlayers) {
        QPair<QUrl, int64_t> cur = {p->videoUrl, p->time()};
        currently_playing.append(cur);
        p->clear();
    }
    mediaPlayers.clear();
    speedDisplayLabel->setText(QString("Speed: 1.00x"));
    seekSlider->blockSignals(true);
    seekSlider->setValue(0);
    seekSlider->blockSignals(false);
    currentBackEnd newBackendType;
    if (text.contains("vlc", Qt::CaseInsensitive)) {
        newBackendType = VLCPlayerBackEnd;
    } else if (text.contains("QMediaPlayer", Qt::CaseInsensitive)) {
        newBackendType = QMediaPlayerBackEnd;
    } else if (text.contains("QWebEngine", Qt::CaseInsensitive)) {
        newBackendType = QWebEngineBackEnd;
    } else {
        qCritical() << "Unknown backend selected:" << text;
        return;
    }
    setActivePlayerCreator(newBackendType);
    for (auto& p: currently_playing) {
        addVideoPlayer(*videoLayout, p.first, mediaPlayers, activePlayerCreator);
    }
    for (int i = 0; i < currently_playing.length(); ++i) {
        mediaPlayers.at(i)->pause();
    }
    for (int i = 0; i < currently_playing.length(); ++i) {
        mediaPlayers.at(i)->play();
        qDebug() << "setting time to: " << currently_playing[i].second;
        mediaPlayers.at(i)->set_time(currently_playing[i].second);
    }
}

void MainWindow::openNewVideo() {
    openAndAddVideo(*this, *videoLayout, mediaPlayers, activePlayerCreator);
}

void MainWindow::clearAllVideos() {
    for (auto& p: mediaPlayers) {
        p->clear();
    }
    mediaPlayers.clear();
    resetUIOnPlayersCleared();
}

void MainWindow::openSettings() {
    SettingsDialog settingsDialog(SPEED_INCREMENT, MIN_PLAYBACK_SPEED, MAX_PLAYBACK_SPEED, currentStyle, this);
    // Connect the signal emitted by the settings dialog to a slot in MainWindow.
    connect(&settingsDialog, &SettingsDialog::settingsAccepted,
            this, &MainWindow::updatePlaybackSettings);
    connect(&settingsDialog, &SettingsDialog::styleAccepted,
            this, &MainWindow::updateApplicationStyle);
    settingsDialog.exec();
}

void MainWindow::updatePlaybackSettings(float newSpeedIncrement, float newMinSpeed, float newMaxSpeed) {
    SPEED_INCREMENT = newSpeedIncrement;
    MIN_PLAYBACK_SPEED = newMinSpeed;
    MAX_PLAYBACK_SPEED = newMaxSpeed;
    qDebug() << "Settings updated: Increment=" << SPEED_INCREMENT
            << ", Min=" << MIN_PLAYBACK_SPEED
            << ", Max=" << MAX_PLAYBACK_SPEED;
}


void MainWindow::updateApplicationStyle(const QString& newStyle) {
    currentStyle = newStyle;
    QApplication::setStyle(QStyleFactory::create(newStyle)); // Apply the new style
    qDebug() << "Application style updated to: " << newStyle;
}

void MainWindow::handleFilesDropped(const QList<QUrl>& urls) {
    for (const QUrl& url : urls) {
        if (url.isLocalFile()) {
            addVideoPlayer(*videoLayout, url, mediaPlayers, activePlayerCreator);
        }
    }
}


void MainWindow::exitApplication() {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Exit", "Are you sure you want to exit?",
                                  QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        qApp->quit();
    }
}
