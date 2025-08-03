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

#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QComboBox>
#include <QDialog>
#include <QDoubleSpinBox>

class SettingsDialog : public QDialog {
    Q_OBJECT

public:
    explicit SettingsDialog(double speedIncrement, double minSpeed, double maxSpeed, const QString& currentStyle,
                            QWidget* parent = nullptr);

signals:
    void settingsAccepted(double newSpeedIncrement, double newMinSpeed, double newMaxSpeed);

    void styleAccepted(const QString& newStyle);

private slots:
    void acceptSettings();

private:
    QDoubleSpinBox* speedIncrementSpinBox;
    QDoubleSpinBox* minPlaybackSpeedSpinBox;
    QDoubleSpinBox* maxPlaybackSpeedSpinBox;
    QComboBox* styleComboBox;
};

#endif // SETTINGSDIALOG_H
