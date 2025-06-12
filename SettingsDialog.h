#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QComboBox>
#include <QDialog>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QDebug>

class SettingsDialog : public QDialog {
    Q_OBJECT

public:
    explicit SettingsDialog(float speedIncrement, float minSpeed, float maxSpeed, const QString& currentStyle, QWidget *parent = nullptr);

    signals:
        void settingsAccepted(float newSpeedIncrement, float newMinSpeed, float newMaxSpeed);
        void styleAccepted(const QString& newStyle);

private slots:
    void acceptSettings();

private:
    QDoubleSpinBox *speedIncrementSpinBox;
    QDoubleSpinBox *minPlaybackSpeedSpinBox;
    QDoubleSpinBox *maxPlaybackSpeedSpinBox;
    QComboBox *styleComboBox;
};

#endif // SETTINGSDIALOG_H