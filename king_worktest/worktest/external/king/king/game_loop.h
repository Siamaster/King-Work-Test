#pragma once

#include "game.h"

namespace king {

    class GameLoop {
    public:

        explicit GameLoop(Game &game) : _game{game} {}

        void Run();
    private:
        Game &_game;
    };

} // namespace king
