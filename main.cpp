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
#include <algorithm>
#include <iostream>
#include <QApplication>
#include <QApplication>
#include <QAudioOutput>
#include <QFileDialog>
#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QSlider>
#include <QtGlobal> // For Q_OS_WIN, Q_OS_LINUX, Q_OS_MACOS
#include <QTimer>
#include <QUrl>
#include <QVBoxLayout>
#include <QWidget>
#include <thread>
#include "AddVideo.h"
#include "vlc.hpp" // uses libvlcpp from https://github.com/videolan/libvlcpp
// #include <vlc/vlc.h> // no longer needed as libvlcpp used instead


const float SPEED_INCREMENT = 0.25f;
const float MIN_PLAYBACK_SPEED = 0.25f;
const float MAX_PLAYBACK_SPEED = 2.0f;

int main(int argc, char *argv[]) {
    // Force Qt to use the XCB (X11) platform plugin
    // This will make the application run via XWayland if on a Wayland session.
    // This must be set before QApplication is constructed.
    // Must be used until VLC supports embedding in native wayland windows: https://code.videolan.org/videolan/vlc/-/issues/16106
    qputenv("QT_QPA_PLATFORM", "xcb");
    QApplication a(argc, argv);
    QMainWindow w;
    QFont font("Segoe UI", 10);
    QWidget centralWidget = QWidget(&w);
    //QGridLayout gridLayout = QGridLayout();
    QPushButton *addButton = new QPushButton("Add Video", &w);
    QPushButton *pauseAllButton = new QPushButton("Pause All", &w);
    QPushButton *unpauseAllButton = new QPushButton("Unpause All", &w);
    QPushButton *clearAllButton = new QPushButton("Clear All", &w);
    QPushButton *increaseSpeedButton = new QPushButton("Speed+", &w);
    QPushButton *decreaseSpeedButton = new QPushButton("Speed-", &w);
    QPushButton *resetSpeedButton = new QPushButton("Speed0", &w);
    QLabel *speedDisplayLabel = new QLabel("Speed: 1.00x", &w);
    speedDisplayLabel->setMinimumWidth(100);
    speedDisplayLabel->setAlignment(Qt::AlignCenter);

    QSlider *seekSlider = new QSlider(Qt::Horizontal, &w);
    seekSlider->setRange(0, 1000);
    seekSlider->setSingleStep(1);
    seekSlider->setPageStep(10);
    seekSlider->setTracking(true);


    addButton->setFont(font);
    pauseAllButton->setFont(font);
    unpauseAllButton->setFont(font);
    clearAllButton->setFont(font);
    increaseSpeedButton->setFont(font);
    decreaseSpeedButton->setFont(font);
    resetSpeedButton->setFont(font);


    QWidget *buttonContainer = new QWidget();
    QHBoxLayout *buttonLayout = new QHBoxLayout(buttonContainer);
    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(pauseAllButton);
    buttonLayout->addWidget(unpauseAllButton);
    buttonLayout->addWidget(clearAllButton);
    buttonLayout->addWidget(increaseSpeedButton);
    buttonLayout->addWidget(decreaseSpeedButton);
    buttonLayout->addWidget(resetSpeedButton);
    buttonLayout->addWidget(speedDisplayLabel);
    buttonLayout->addStretch();
    buttonLayout->setContentsMargins(0, 0, 0, 0);
    buttonLayout->setSpacing(8);
    buttonContainer->setLayout(buttonLayout);

    QWidget *videoContainer = new QWidget();
    QGridLayout *videoLayout = new QGridLayout(videoContainer);
    videoLayout->setContentsMargins(0, 0, 0, 0);
    videoLayout->setSpacing(10);
    videoContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addWidget(videoContainer);
    QFrame *line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    mainLayout->addWidget(line);
    mainLayout->addWidget(seekSlider);
    mainLayout->addWidget(buttonContainer);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(10);
    centralWidget.setLayout(mainLayout);

    QVector<VLCPlayer> vlcPlayers;

    QObject::connect(addButton, &QPushButton::clicked, [&]() {
        openAndAddVideoVLC(w, *videoLayout, vlcPlayers);
    });

    QObject::connect(pauseAllButton, &QPushButton::clicked, [&]() {
        for (auto &p : vlcPlayers)
            p.mediaPlayer->pause();
    });

    QObject::connect(unpauseAllButton, &QPushButton::clicked, [&]() {
        for (auto &p : vlcPlayers)
            p.mediaPlayer->play();
    });

    QObject::connect(clearAllButton, &QPushButton::clicked, [&]() {
        for (auto &p : vlcPlayers) {
            p.mediaPlayer->stop(); //TODO: check resource deallocation
            delete p.mediaPlayer;
            delete p.vlcInstance;
            p.videoWidget->deleteLater();
        }
        vlcPlayers.clear();
        speedDisplayLabel->setText(QString("Speed: 1.00x"));
    });

    auto updateSpeedDisplay = [&](float rate) {
        speedDisplayLabel->setText(QString("Speed: %1x").arg(rate, 0, 'f', 2));
    };

    QObject::connect(increaseSpeedButton, &QPushButton::clicked, [&]() {
        for (auto &p : vlcPlayers) {
            float currentRate = p.mediaPlayer->rate();
            float newRate = std::min(currentRate + SPEED_INCREMENT, MAX_PLAYBACK_SPEED);
            p.mediaPlayer->setRate(newRate);
            if (&p == &vlcPlayers.first()) {
                updateSpeedDisplay(newRate);
                p.mediaPlayer->setVolume(p.mediaPlayer->volume());
            }
        }
    });

    QObject::connect(decreaseSpeedButton, &QPushButton::clicked, [&]() {
        for (auto &p : vlcPlayers) {
            float currentRate = p.mediaPlayer->rate();
            float newRate = std::max(currentRate - SPEED_INCREMENT, MIN_PLAYBACK_SPEED);
            p.mediaPlayer->setRate(newRate);
            if (&p == &vlcPlayers.first()) {
                updateSpeedDisplay(newRate);
            }
        }
    });

    QObject::connect(resetSpeedButton, &QPushButton::clicked, [&]() {
        for (auto &p : vlcPlayers) {
            p.mediaPlayer->setRate(1.0f);
        }
        updateSpeedDisplay(1.0f);
    });

    QObject::connect(seekSlider, &QSlider::sliderReleased, [&]() {
        if (!vlcPlayers.isEmpty()) {
            int value = seekSlider->value();
            for (auto &p : vlcPlayers) {
                int64_t length = p.mediaPlayer->media()->duration();
                int64_t seekTo = static_cast<int64_t>((double(value) / 1000.0) * length);
                p.mediaPlayer->setTime(seekTo);
            }
        }
    });

    QTimer *sliderUpdateTimer = new QTimer(&w);
    sliderUpdateTimer->setInterval(500);
    QObject::connect(sliderUpdateTimer, &QTimer::timeout, [&]() {
        if (!vlcPlayers.isEmpty()) {
            auto player = vlcPlayers.first();
            int64_t pos = player.mediaPlayer->time();
            int64_t length = player.mediaPlayer->media()->duration();
            if (length > 0) {
                int value = static_cast<int>((double(pos) / length) * 1000);
                seekSlider->blockSignals(true);
                seekSlider->setValue(value);
                seekSlider->blockSignals(false);
            }
        }
    });
    sliderUpdateTimer->start();

    w.setCentralWidget(&centralWidget);
    w.resize(800, 600);
    w.show();
    return a.exec();
}