//PlayerFactory.h
#ifndef PLAYERFACTORY_H
#define PLAYERFACTORY_H

#include "MediaPlayers.h"

namespace PlayerFactory {
    MediaPlayerBase* createPlayer(const QUrl& videoUrl);
}

#endif // PLAYERFACTORY_H