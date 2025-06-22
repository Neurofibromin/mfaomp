//MainWindow.h
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

#include <QMainWindow>
#include <QVector>
#include <QTimer>
#include <functional>
#include "BackEndEnum.h"
#include "PlayerFactory.h"

// Forward declarations
class QAction;
class QComboBox;
class QGridLayout;
class QLabel;
class QMenu;
class QMenuBar;
class QPushButton;
class QSlider;
class QVBoxLayout;
class DropWidget;
class SettingsDialog;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

    ~MainWindow() override;

private slots:
    // UI Update Slots
    void updateSpeedDisplay(float speed);

    void updateSeekSliderDisplay(int64_t position, int64_t duration);

    void resetPlaybackControls();

    // Action Slots
    void openNewVideo();

    void clearAllVideos();

    void openSettingsDialog();

    void exitApplication();

    void handleFilesDropped(const QList<QUrl>& urls);

    void handleSeekSliderReleased();

    void handleBackendChanged(const QString& text);

    // Settings Update Slots
    void updatePlaybackSettings(float newSpeedIncrement, float newMinSpeed, float newMaxSpeed);

    void updateApplicationStyle(const QString& newStyle);

    // Player Control Slots
    void pauseAllPlayers();

    void playAllPlayers();

    void muteAllPlayers();

    void unmuteAllPlayers();

    void changeAllPlayersSpeed(float delta);

    void resetAllPlayersSpeed();

    void updateSeekSliderFromFirstPlayer();

private:
    // UI Initialization
    void initializeUI();

    void setupMenuBar();

    QWidget* setupControlsPanel();

    void setupBottomBar(QVBoxLayout* mainLayout, QWidget* controlsPanel);

    void applyStyles();

    // Connections
    void initializeConnections();

    void connectMenuItems();

    void connectPlayerControlButtons();

    void connectOtherControls();

    // Backend Management
    void setActivePlayerCreator(BackEndManager::BackEnd backendType);

    void populateBackendComboBox();

    // UI Elements
    DropWidget* m_dropWidget = nullptr;
    QWidget* m_videoContainer = nullptr;
    QGridLayout* m_videoLayout = nullptr;

    QPushButton* m_addButton = nullptr;
    QPushButton* m_pauseAllButton = nullptr;
    QPushButton* m_unpauseAllButton = nullptr;
    QPushButton* m_muteAllButton = nullptr;
    QPushButton* m_unmuteAllButton = nullptr;
    QPushButton* m_clearAllButton = nullptr;
    QPushButton* m_increaseSpeedButton = nullptr;
    QPushButton* m_decreaseSpeedButton = nullptr;
    QPushButton* m_resetSpeedButton = nullptr;
    QLabel* m_speedDisplayLabel = nullptr;
    QSlider* m_seekSlider = nullptr;
    QComboBox* m_backendComboBox = nullptr;

    QVector<MediaPlayerBase *> m_mediaPlayers = QVector<MediaPlayerBase *>();

    // Menu bar components
    QMenuBar* m_menuBar = nullptr;
    QMenu* m_fileMenu = nullptr;
    QMenu* m_editMenu = nullptr;
    QMenu* m_optionsMenu = nullptr;
    QAction* m_openNewVideoAction = nullptr;
    QAction* m_clearAction = nullptr;
    QAction* m_settingsAction = nullptr;
    QAction* m_exitAction = nullptr;
    QString m_currentStyle;

    // Playback settings
    float SPEED_INCREMENT = 0.25f;
    float MIN_PLAYBACK_SPEED = 0.25f;
    float MAX_PLAYBACK_SPEED = 2.0f;

    CreatePlayerFuncPtrType m_activePlayerCreator = nullptr;
    BackEndManager::BackEnd m_activeBackendType = BackEndManager::BackEnd::QMediaPlayer;

    // Constants
    const int SEEK_SLIDER_MAX_VALUE = 1000;
    const int SLIDER_UPDATE_INTERVAL_MS = 500;
};

#endif // MAINWINDOW_H
