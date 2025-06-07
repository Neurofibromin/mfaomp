#include "MainWindow.h"
#include <iostream>
#include <QComboBox>
#include <QDebug>
#include <QFont>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QWebEngineProfile>
#include "AddVideo.h"


const float SPEED_INCREMENT = 0.25f;
const float MIN_PLAYBACK_SPEED = 0.25f;
const float MAX_PLAYBACK_SPEED = 2.0f;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), mediaPlayers() {
    createWidgets();
    applyStyles();
    makeConnections();
    setCentralWidget(centralWidget);
    resize(800, 600);
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
    backendComboBox->addItem("VLC Backend");
    backendComboBox->addItem("QMediaPlayer Backend");
    backendComboBox->addItem("QWebEngine Backend");

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
    QFrame *line = new QFrame(this);
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
    QObject::connect(addButton, &QPushButton::clicked, [&]() {
        openAndAddVideo(*this, *videoLayout, mediaPlayers);
    });

    QObject::connect(pauseAllButton, &QPushButton::clicked, [&]() {
        for (auto &p : mediaPlayers)
            p->pause();
    });

    QObject::connect(unpauseAllButton, &QPushButton::clicked, [&]() {
        for (auto &p : mediaPlayers)
            p->play();
    });

    QObject::connect(muteAllButton, &QPushButton::clicked, [&]() {
        for (auto &p : mediaPlayers)
            p->mute();
    });

    QObject::connect(unmuteAllButton, &QPushButton::clicked, [&]() {
        for (auto &p : mediaPlayers)
            p->unmute();
    });

    QObject::connect(clearAllButton, &QPushButton::clicked, [&]() {
        for (auto &p : mediaPlayers) {
            p->clear();
        }
        mediaPlayers.clear();
        resetUIOnPlayersCleared();
    });

    QObject::connect(increaseSpeedButton, &QPushButton::clicked, [&]() {
        for (auto &p : mediaPlayers) {
            float currentRate = p->speed();
            float newRate = std::min(currentRate + SPEED_INCREMENT, MAX_PLAYBACK_SPEED);
            p->speed(newRate);
            if (&p == &mediaPlayers.first()) {
                updateSpeedDisplay(newRate);
            }
        }
    });

    QObject::connect(decreaseSpeedButton, &QPushButton::clicked, [&]() {
        for (auto &p : mediaPlayers) {
            float currentRate = p->speed();
            float newRate = std::max(currentRate - SPEED_INCREMENT, MIN_PLAYBACK_SPEED);
            p->speed(newRate);
            if (&p == &mediaPlayers.first()) {
                updateSpeedDisplay(newRate);
            }
        }
    });

    QObject::connect(resetSpeedButton, &QPushButton::clicked, [&]() {
        for (auto &p : mediaPlayers) {
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
        for (auto &p : mediaPlayers) {
            int64_t length = p->duration();
            int64_t seekTo = static_cast<int64_t>((double(value) / 1000.0) * length);
            p->set_time(seekTo);
        }
    }
}

void MainWindow::handleBackendChanged(const QString& text) {
    std::cout << "Selected backend: " << text.toStdString() << std::endl;
    QVector<QPair<QUrl, int64_t>> currently_playing;
    for (auto &p : mediaPlayers) {
        QPair<QUrl, int64_t> cur = {p->videoUrl, p->time()};
        currently_playing.append(cur);
        p->clear();
    }
    mediaPlayers.clear();
    speedDisplayLabel->setText(QString("Speed: 1.00x"));
    seekSlider->blockSignals(true);
    seekSlider->setValue(0);
    seekSlider->blockSignals(false);
    if (text.contains("vlc", Qt::CaseInsensitive)) {
        CurrentBackEndStatusSingleton::getInstance().setCurrentBackEnd(VLCPlayerBackEnd);
    }
    else if (text.contains("QMediaPlayer", Qt::CaseInsensitive)) {
        CurrentBackEndStatusSingleton::getInstance().setCurrentBackEnd(QMediaPlayerBackEnd);
    }
    else if (text.contains("QWebEngine", Qt::CaseInsensitive)) {
        CurrentBackEndStatusSingleton::getInstance().setCurrentBackEnd(QWebEngineBackEnd);
    }
    for (auto &p : currently_playing) {
        addVideoPlayer(*videoLayout, p.first, mediaPlayers);
    }
    for (int i = 0; i < currently_playing.length(); ++i) {
        mediaPlayers.at(i)->pause();
    }
    for (int i = 0; i < currently_playing.length(); ++i) {
        mediaPlayers.at(i)->play();
        std::cout << "setting time to: " << currently_playing[i].second << std::endl;
        mediaPlayers.at(i)->set_time(currently_playing[i].second);
    }
}
