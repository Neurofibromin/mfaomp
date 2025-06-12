#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>

class SettingsDialog : public QDialog {
    Q_OBJECT

public:
    explicit SettingsDialog(float speedIncrement, float minSpeed, float maxSpeed, QWidget *parent = nullptr);

    signals:
        void settingsAccepted(float newSpeedIncrement, float newMinSpeed, float newMaxSpeed);

private slots:
    void acceptSettings();

private:
    QDoubleSpinBox *speedIncrementSpinBox;
    QDoubleSpinBox *minPlaybackSpeedSpinBox;
    QDoubleSpinBox *maxPlaybackSpeedSpinBox;
};

#endif // SETTINGSDIALOG_H