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
#include "config.h"
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
#ifdef HAVE_SDL2
#include <SDL2/SDL.h>
#endif
#include <QApplication>
#include <QOpenGLContext>
#include <QOffscreenSurface>
#include <QDebug>

int main(int argc, char* argv[]) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--version" || arg == "-v") {
            std::cout << APP_DESCRIPTION << " version " << APP_VERSION << std::endl;
            return 0;
        }
    }
    // Create a temporary context to check the OpenGL renderer string.
#ifdef linux
    qputenv("QT_QPA_PLATFORM", "xcb");
    qputenv("SDL_VIDEODRIVER", "x11"); //SDL2 with Qt only works on x11, otherwise segfault?
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
#endif
    bool useSoftwareRendering = false;
    { //in scope so the QGuiApplication gets cleaned up before QApplication starts
        QGuiApplication tempApp(argc, argv); // Minimal QGuiApplication to allow OpenGL initialization
        QOffscreenSurface surface;
        surface.create();
        QOpenGLContext context;
        context.create();
        if (context.makeCurrent(&surface)) {
            if (const GLubyte* renderer = glGetString(GL_RENDERER)) {
                QString rendererString = QString::fromLatin1(reinterpret_cast<const char*>(renderer));
                std::cout << "Detected OpenGL renderer:" << rendererString.toStdString() << '\n';
                if (rendererString.startsWith("NV", Qt::CaseInsensitive)) { //https://nouveau.freedesktop.org/CodeNames.html
                    std::cout << "Nouveau driver detected. Forcing software rendering." << '\n';
                    useSoftwareRendering = true;
                }
            }
            context.doneCurrent();
        }
        surface.destroy();
    }
    if (useSoftwareRendering)
        qputenv("QT_QUICK_BACKEND", "software");
    // Simpler check
    if (!useSoftwareRendering && QFileInfo::exists("/sys/module/nouveau")) {
        std::cout << "Nouveau kernel module detected. Forcing software rendering to prevent potential crash." << '\n';
        qputenv("QT_QUICK_BACKEND", "software");
    }

    // Force Qt to use the XCB (X11) platform plugin
    // This will make the application run via XWayland if on a Wayland session.
    // This must be set before QApplication is constructed.
    // Must be used until VLC supports embedding in native wayland windows: https://code.videolan.org/videolan/vlc/-/issues/16106
#ifdef linux
    qputenv("QT_QPA_PLATFORM", "xcb");
    qputenv("SDL_VIDEODRIVER", "x11"); //SDL2 with Qt only works on x11, otherwise segfault?
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
#endif
    QApplication a(argc, argv);
    // a.setStyle("windows"); //https://forum.qt.io/topic/127907/where-can-i-find-win95-win2000-stylesheet/4
    a.setWindowIcon(QIcon(":/packaging/mfaomp_icon_32.png"));
#ifdef HAVE_SDL2
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        qCritical("Failed to initialize SDL_INIT_VIDEO: %s", SDL_GetError());
        return -1;
    }
#endif
    MainWindow w;

#ifdef HAVE_QTWEBENGINE
    QWebEngineProfile::defaultProfile()->settings()->setAttribute(
        QWebEngineSettings::PlaybackRequiresUserGesture, false);
#endif
    w.show();
    int ret = a.exec();
#ifdef HAVE_SDL2
    SDL_Quit();
#endif
    return ret;
}
