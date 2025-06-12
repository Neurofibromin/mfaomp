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
#include <iostream>
#include <thread>
#include "DropWidget.h"
#include <QApplication>
#include <QWidget>
#ifdef HAVE_QTWEBENGINE
#include <QWebEngineSettings>
#include <QWebEngineProfile>
#endif
#include "MainWindow.h"
#include "config.h"

int main(int argc, char *argv[]) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--version" || arg == "-v") {
            std::cout << APP_DESCRIPTION << " version " << APP_VERSION << std::endl;
            return 0;
        }
    }
    // Force Qt to use the XCB (X11) platform plugin
    // This will make the application run via XWayland if on a Wayland session.
    // This must be set before QApplication is constructed.
    // Must be used until VLC supports embedding in native wayland windows: https://code.videolan.org/videolan/vlc/-/issues/16106
    qputenv("QT_QPA_PLATFORM", "xcb");
    QApplication a(argc, argv);
    // a.setStyle("windows"); //https://forum.qt.io/topic/127907/where-can-i-find-win95-win2000-stylesheet/4
    MainWindow w;
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
#ifdef HAVE_QTWEBENGINE
    QWebEngineProfile::defaultProfile()->settings()->setAttribute(
            QWebEngineSettings::PlaybackRequiresUserGesture, false);
#endif
    w.show();
    return a.exec();
}