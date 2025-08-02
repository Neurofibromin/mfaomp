#include "SDL2Struct.h"

#include <QTimer>
#include <QShowEvent>
#include <QPaintEvent>
#include <QDebug> // For logging

SDLWidget::SDLWidget(QWidget *parent) : QWidget(parent) {
    // These attributes are important for allowing SDL to render directly
    // to this widget's area without Qt interference.
    setAttribute(Qt::WA_PaintOnScreen, true);
    setAttribute(Qt::WA_NoSystemBackground, true);
    setAutoFillBackground(false);

    m_renderTimer = new QTimer(this);
    connect(m_renderTimer, &QTimer::timeout, this, &SDLWidget::onRender);
}

SDLWidget::~SDLWidget() {
    cleanupSDL();
    // m_renderTimer is a child of SDLWidget, so Qt will handle its deletion.
}

QPaintEngine* SDLWidget::paintEngine() const {
    // Returning nullptr tells Qt that this widget handles its own painting
    // and does not use Qt's QPainter-based system for its primary content.
    return nullptr;
}

void SDLWidget::showEvent(QShowEvent *event) {
    QWidget::showEvent(event); // Call the base class implementation

    // Initialize SDL only once the widget is actually being shown and has a valid winId()
    if (!m_isSDLInitialized && isVisible()) {
        initializeSDL();
    }
}

void SDLWidget::paintEvent(QPaintEvent *event) {
    // Q_UNUSED(event);
    // Do nothing here. SDL is responsible for all rendering within this widget's area.
    // This override (along with WA_PaintOnScreen and paintEngine()) prevents Qt
    // from trying to paint the widget itself.
}

void SDLWidget::initializeSDL() {
    if (m_isSDLInitialized) {
        return;
    }

    if (!this->testAttribute(Qt::WA_WState_Created)) {
        qDebug() << "SDLWidget: Native window not yet created, attempting createWinId().";
        this->createWinId(); // Force creation of the native window
    }

    WId windowId = this->winId();
    if (windowId == 0) {
        qCritical("SDLWidget: winId() is 0. SDL_CreateWindowFrom will likely fail.");
        // Forcing native window creation could be attempted here if necessary,
        // but showEvent should generally ensure winId() is valid.
        // this->createWinId();
        // windowId = this->winId();
        // if(windowId == 0) {
        //    qCritical("SDLWidget: winId() still 0 after createWinId(). Aborting SDL init.");
        //    return;
        // }
    }

    qCritical() << "SDLWidget: Attempting to use winId:" << QString::number(windowId, 16);
    // SDLWidget: Attempting to use winId: "3"
    m_sdlWindow = SDL_CreateWindowFrom(reinterpret_cast<void *>(windowId)); //SIGSEGV here
    if (!m_sdlWindow) {
        qCritical("SDLWidget: SDL_CreateWindowFrom failed: %s", SDL_GetError());
        return;
    }
    qDebug() << "SDLWidget: SDL_Window created successfully.";

    m_sdlRenderer = SDL_CreateRenderer(m_sdlWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!m_sdlRenderer) {
        qCritical("SDLWidget: SDL_CreateRenderer failed: %s", SDL_GetError());
        SDL_DestroyWindow(m_sdlWindow); // Clean up the window if renderer creation fails
        m_sdlWindow = nullptr;
        return;
    }
    qDebug() << "SDLWidget: SDL_Renderer created successfully.";

    SDL_SetRenderDrawBlendMode(m_sdlRenderer, SDL_BLENDMODE_BLEND); // Optional

    m_isSDLInitialized = true;
    m_renderTimer->start(16); // Target roughly 60 FPS (1000ms / 16ms ~= 62.5 FPS)
    qDebug() << "SDLWidget: SDL initialized and render timer started.";
}

void SDLWidget::cleanupSDL() {
    if (m_renderTimer) {
        m_renderTimer->stop();
    }

    if (m_sdlRenderer) {
        SDL_DestroyRenderer(m_sdlRenderer);
        m_sdlRenderer = nullptr;
        qDebug() << "SDLWidget: SDL_Renderer destroyed.";
    }
    if (m_sdlWindow) {
        // The window created by SDL_CreateWindowFrom should be destroyed by SDL.
        SDL_DestroyWindow(m_sdlWindow);
        m_sdlWindow = nullptr;
        qDebug() << "SDLWidget: SDL_Window destroyed.";
    }
    m_isSDLInitialized = false;
}

void SDLWidget::onRender() {
    if (!m_isSDLInitialized || !m_sdlRenderer || !m_sdlWindow) {
        // Don't attempt to render if SDL isn't ready or has been cleaned up
        return;
    }

    // Simple animation: move a rectangle back and forth
    m_rectX += 5 * m_rectDirection;
    if (m_rectX > this->width() - 50 || m_rectX < 0) {
        m_rectDirection *= -1;
        m_rectX += 5 * m_rectDirection; // Nudge to prevent getting stuck at edges
    }

    // Clear the renderer (background color)
    SDL_SetRenderDrawColor(m_sdlRenderer, 0, 0, 0, 255); // Black
    SDL_RenderClear(m_sdlRenderer);

    // Draw a red rectangle
    SDL_Rect fillRect = {m_rectX, this->height() / 2 - 25, 50, 50};
    SDL_SetRenderDrawColor(m_sdlRenderer, 255, 0, 0, 255); // Red
    SDL_RenderFillRect(m_sdlRenderer, &fillRect);

    // Present the frame
    SDL_RenderPresent(m_sdlRenderer);
}


SDL2Struct::SDL2Struct(const QUrl& videoUrl) : MediaPlayerBase(videoUrl) {
    videoWidget = new SDLWidget();
    videoWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    QObject::connect(videoWidget, &QWidget::customContextMenuRequested,
        MediaPlayerBase::createContextMenu);
}

void SDL2Struct::play() {
}

void SDL2Struct::pause() {
}

void SDL2Struct::mute() {
}

void SDL2Struct::unmute() {
}

void SDL2Struct::clear() {
    delete videoWidget;
    videoWidget = nullptr;
}

float SDL2Struct::speed(float sp) {
    return 1.0f;
}

int64_t SDL2Struct::time() {
    return 0;
}

void SDL2Struct::set_time(int64_t time) {
}

float SDL2Struct::duration() {
    return 2.0f;
}

SDL2Struct::~SDL2Struct() {
    SDL2Struct::clear();
}

QWidget* SDL2Struct::getVideoWidget() {
    return videoWidget;
}

QMenu* SDL2Struct::createContextMenu(QWidget* parent) {
    auto* menu = new QMenu(parent);
    menu->addAction("SDL2: Custom Action", []() {
        // A placeholder action
    });
    return menu;
}