#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "DropWidget.h"
#include "MediaPlayers.h"

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

private:
    void createWidgets();
    void makeConnections();
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
};

#endif // MAINWINDOW_H