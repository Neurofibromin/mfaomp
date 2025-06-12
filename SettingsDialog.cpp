#include "SettingsDialog.h"

#include <QMessageBox>

SettingsDialog::SettingsDialog(float speedIncrement, float minSpeed, float maxSpeed, QWidget *parent)
    : QDialog(parent) {
    setWindowTitle("Playback Settings");
    setFixedSize(300, 200);

    speedIncrementSpinBox = new QDoubleSpinBox(this);
    minPlaybackSpeedSpinBox = new QDoubleSpinBox(this);
    maxPlaybackSpeedSpinBox = new QDoubleSpinBox(this);

    speedIncrementSpinBox->setRange(0.01, 5.0);
    speedIncrementSpinBox->setSingleStep(0.01);
    speedIncrementSpinBox->setValue(speedIncrement);
    speedIncrementSpinBox->setDecimals(2);

    minPlaybackSpeedSpinBox->setRange(0.01, 10.0);
    minPlaybackSpeedSpinBox->setSingleStep(0.01);
    minPlaybackSpeedSpinBox->setValue(minSpeed);
    minPlaybackSpeedSpinBox->setDecimals(2);

    maxPlaybackSpeedSpinBox->setRange(0.1, 10.0);
    maxPlaybackSpeedSpinBox->setSingleStep(0.01);
    maxPlaybackSpeedSpinBox->setValue(maxSpeed);
    maxPlaybackSpeedSpinBox->setDecimals(2);

    QLabel *speedIncrementLabel = new QLabel("Speed Increment:", this);
    QLabel *minPlaybackSpeedLabel = new QLabel("Min Playback Speed:", this);
    QLabel *maxPlaybackSpeedLabel = new QLabel("Max Playback Speed:", this);

    QPushButton *okButton = new QPushButton("OK", this);
    QPushButton *cancelButton = new QPushButton("Cancel", this);

    QFormLayout *formLayout = new QFormLayout();
    formLayout->addRow(speedIncrementLabel, speedIncrementSpinBox);
    formLayout->addRow(minPlaybackSpeedLabel, minPlaybackSpeedSpinBox);
    formLayout->addRow(maxPlaybackSpeedLabel, maxPlaybackSpeedSpinBox);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addStretch();

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(formLayout);
    mainLayout->addStretch();
    mainLayout->addLayout(buttonLayout);
    // should check if minplaybackspeed <= maxplaybackspeed
    connect(okButton, &QPushButton::clicked, this, &SettingsDialog::acceptSettings);
    connect(cancelButton, &QPushButton::clicked, this, &SettingsDialog::reject);
}

void SettingsDialog::acceptSettings() {
    float minSpeed = minPlaybackSpeedSpinBox->value();
    float maxSpeed = maxPlaybackSpeedSpinBox->value();
    if (minSpeed > maxSpeed) {
        QMessageBox::warning(this, "Input Error", "Minimum playback speed cannot be greater than maximum playback speed.");
        return;
    }
    emit settingsAccepted(speedIncrementSpinBox->value(), minSpeed, maxSpeed);
    accept();
}
