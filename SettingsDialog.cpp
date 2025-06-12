#include "SettingsDialog.h"

#include <iostream>
#include <QApplication>
#include <QMessageBox>
#include <qstylefactory.h>

SettingsDialog::SettingsDialog(float speedIncrement, float minSpeed, float maxSpeed, QWidget *parent)
    : QDialog(parent) {
    setWindowTitle("Playback Settings");
    setFixedSize(300, 200);

    speedIncrementSpinBox = new QDoubleSpinBox(this);
    minPlaybackSpeedSpinBox = new QDoubleSpinBox(this);
    maxPlaybackSpeedSpinBox = new QDoubleSpinBox(this);
    styleComboBox = new QComboBox(this);

    // QStringList availableStyles = QApplication::styleFactory()->keys();
    QStringList availableStyles = QStyleFactory::keys();
    styleComboBox->addItems(availableStyles);
    // int currentIndex = styleComboBox->findText(currentStyle);
    // if (currentIndex != -1) {
    //     styleComboBox->setCurrentIndex(currentIndex);
    // }

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
    QLabel *styleLabel = new QLabel("Application Style:", this);

    QPushButton *okButton = new QPushButton("OK", this);
    QPushButton *cancelButton = new QPushButton("Cancel", this);

    QFormLayout *formLayout = new QFormLayout();
    formLayout->addRow(speedIncrementLabel, speedIncrementSpinBox);
    formLayout->addRow(minPlaybackSpeedLabel, minPlaybackSpeedSpinBox);
    formLayout->addRow(maxPlaybackSpeedLabel, maxPlaybackSpeedSpinBox);
    formLayout->addRow(styleLabel, styleComboBox);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addStretch();

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(formLayout);
    mainLayout->addStretch();
    mainLayout->addLayout(buttonLayout);
    connect(okButton, &QPushButton::clicked, this, &SettingsDialog::acceptSettings);
    connect(cancelButton, &QPushButton::clicked, this, &SettingsDialog::reject);
}

void SettingsDialog::acceptSettings() {
    std::cout << styleComboBox->currentText().toStdString() << " is the chosen style";
    float minSpeed = minPlaybackSpeedSpinBox->value();
    float maxSpeed = maxPlaybackSpeedSpinBox->value();
    if (minSpeed > maxSpeed) {
        QMessageBox::warning(this, "Input Error", "Minimum playback speed cannot be greater than maximum playback speed.");
        return;
    }
    emit settingsAccepted(speedIncrementSpinBox->value(), minSpeed, maxSpeed);
    std::cout << styleComboBox->currentText().toStdString() << " is the chosen style";
    emit styleAccepted(styleComboBox->currentText());
    accept();
}
