#ifndef SDL2STRUCT_H
#define SDL2STRUCT_H

#include <QWidget>
#include <SDL2/SDL.h>

class QTimer;

class SDLWidget : public QWidget {
    Q_OBJECT

public:
    explicit SDLWidget(QWidget *parent = nullptr);
    ~SDLWidget() override;

protected:
    // Override showEvent to initialize SDL when the widget is about to be shown
    void showEvent(QShowEvent *event) override;

    // Override paintEvent to tell Qt we are handling painting
    void paintEvent(QPaintEvent *event) override;

    // Override to indicate custom painting, important for WA_PaintOnScreen
    QPaintEngine *paintEngine() const override;


private slots:
    void onRender(); // Slot for the QTimer to trigger SDL rendering

private:
    void initializeSDL();
    void cleanupSDL();

    SDL_Window *m_sdlWindow = nullptr;
    SDL_Renderer *m_sdlRenderer = nullptr;
    QTimer *m_renderTimer = nullptr;
    bool m_isSDLInitialized = false;

    // For a simple animation
    int m_rectX = 0;
    int m_rectDirection = 1;
};



#include "../MediaPlayers.h"

struct SDL2Struct : public MediaPlayerBase {
    SDLWidget* videoWidget = nullptr; //TODO: maybe can be QWidget* and moved to base class?

    explicit SDL2Struct(const QUrl& videoUrl, QWidget* parent);

    void play() override;

    void pause() override;

    void mute() override;

    void unmute() override;

    void clear() override;

    float speed(float sp = 0.0) override;

    int64_t time() override;

    void set_time(int64_t time) override;

    float duration() override;

    ~SDL2Struct() override;

    QWidget* getVideoWidget() override;

    QMenu* createContextMenu(QWidget* parent) override;


};

#endif //SDL2STRUCT_H
