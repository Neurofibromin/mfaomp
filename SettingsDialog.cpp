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

#include "SettingsDialog.h"

#include <iostream>
#include <QApplication>
#include <QFormLayout>
#include <QMessageBox>
#include <QPushButton>
#include <QLabel>
#include <QStyleFactory>

SettingsDialog::SettingsDialog(float speedIncrement, float minSpeed, float maxSpeed, const QString& currentStyle,
                               QWidget* parent)
    : QDialog(parent) {
    setWindowTitle("Playback Settings");
    setFixedSize(300, 200);

    speedIncrementSpinBox = new QDoubleSpinBox(this);
    speedIncrementSpinBox->setObjectName("speedIncrementSpinBox");
    minPlaybackSpeedSpinBox = new QDoubleSpinBox(this);
    minPlaybackSpeedSpinBox->setObjectName("minPlaybackSpeedSpinBox");
    maxPlaybackSpeedSpinBox = new QDoubleSpinBox(this);
    maxPlaybackSpeedSpinBox->setObjectName("maxPlaybackSpeedSpinBox");
    styleComboBox = new QComboBox(this);
    styleComboBox->setObjectName("styleComboBox");

    QStringList availableStyles = QStyleFactory::keys();
    styleComboBox->addItems(availableStyles);
    int currentIndex = styleComboBox->findText(currentStyle);
    if (currentIndex != -1) {
        styleComboBox->setCurrentIndex(currentIndex);
    }

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

    QLabel* speedIncrementLabel = new QLabel("Speed Increment:", this);
    QLabel* minPlaybackSpeedLabel = new QLabel("Min Playback Speed:", this);
    QLabel* maxPlaybackSpeedLabel = new QLabel("Max Playback Speed:", this);
    QLabel* styleLabel = new QLabel("Application Style:", this);

    QPushButton* okButton = new QPushButton("OK", this);
    okButton->setObjectName("okButton");
    QPushButton* cancelButton = new QPushButton("Cancel", this);
    cancelButton->setObjectName("cancelButton");

    QFormLayout* formLayout = new QFormLayout();
    formLayout->addRow(speedIncrementLabel, speedIncrementSpinBox);
    formLayout->addRow(minPlaybackSpeedLabel, minPlaybackSpeedSpinBox);
    formLayout->addRow(maxPlaybackSpeedLabel, maxPlaybackSpeedSpinBox);
    formLayout->addRow(styleLabel, styleComboBox);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addStretch();

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(formLayout);
    mainLayout->addStretch();
    mainLayout->addLayout(buttonLayout);
    connect(okButton, &QPushButton::clicked, this, &SettingsDialog::acceptSettings);
    connect(cancelButton, &QPushButton::clicked, this, &SettingsDialog::reject);
}

void SettingsDialog::acceptSettings() {
    std::cout << styleComboBox->currentText().toStdString() << " is the chosen style";
    double minSpeed = minPlaybackSpeedSpinBox->value();
    double maxSpeed = maxPlaybackSpeedSpinBox->value();
    if (minSpeed > maxSpeed) {
        QMessageBox::warning(this, "Input Error",
                             "Minimum playback speed cannot be greater than maximum playback speed.");
        return;
    }
    emit settingsAccepted(speedIncrementSpinBox->value(), minSpeed, maxSpeed);
    qDebug() << styleComboBox->currentText() << " is the chosen style";
    emit styleAccepted(styleComboBox->currentText());
    accept();
}
