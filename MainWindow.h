#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QAction>
#include <QMenuBar>
#include <QVector>
#include <QTimer>
#include <QPair>
#include <QMenu>
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

    float SPEED_INCREMENT = 0.25f;
    float MIN_PLAYBACK_SPEED = 0.25f;
    float MAX_PLAYBACK_SPEED = 2.0f;
};

#endif // MAINWINDOW_H