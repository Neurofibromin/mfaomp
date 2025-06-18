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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QLabel>
#include <QMainWindow>
#include <QMenuBar>
#include <QTimer>
#include <QVector>
#include "DropWidget.h"
#include "MediaPlayers.h"
#include "SettingsDialog.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void updateSpeedDisplay(float speed);
    void updateSeekSlider(int64_t position, int64_t duration);
    void resetUIOnPlayersCleared() const;
    void handleSeekSliderReleased();
    void handleBackendChanged(const QString &text);
    void openNewVideo();
    void clearAllVideos();
    void openSettings();
    void exitApplication();
    void updatePlaybackSettings(float newSpeedIncrement, float newMinSpeed, float newMaxSpeed);
    void updateApplicationStyle(const QString& newStyle);

private:
    void createWidgets();
    void makeConnections();

    void setupMenuBar();

    void applyStyles();

    DropWidget *centralWidget = nullptr;
    QWidget *videoContainer = nullptr;
    QGridLayout *videoLayout = nullptr;

    QPushButton *addButton = nullptr;
    QPushButton *pauseAllButton = nullptr;
    QPushButton *unpauseAllButton = nullptr;
    QPushButton *muteAllButton = nullptr;
    QPushButton *unmuteAllButton = nullptr;
    QPushButton *clearAllButton = nullptr;
    QPushButton *increaseSpeedButton = nullptr;
    QPushButton *decreaseSpeedButton = nullptr;
    QPushButton *resetSpeedButton = nullptr;
    QLabel *speedDisplayLabel = nullptr;
    QSlider *seekSlider = nullptr;
    QComboBox *backendComboBox = nullptr;
    QVector<MediaPlayerBase*> mediaPlayers;

    // Menu bar components
    QMenuBar *menuBar;
    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *optionsMenu;
    QAction *openNewVideoAction;
    QAction *clearAction;
    QAction *settingsAction;
    QAction *exitAction;
    QString currentStyle;

    float SPEED_INCREMENT = 0.25f;
    float MIN_PLAYBACK_SPEED = 0.25f;
    float MAX_PLAYBACK_SPEED = 2.0f;
};

#endif // MAINWINDOW_H