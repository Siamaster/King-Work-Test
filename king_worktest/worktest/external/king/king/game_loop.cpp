#include "game_loop.h"

namespace king {

    void GameLoop::Run() {
        _game.Init();

        const int frames_per_second = 60,
                preferred_milliseconds_per_frame = 1000 / frames_per_second;

        int last_tick = 0;
        while (_game.isRunning()) {
            int current_tick = SDL_GetTicks();
            int milliseconds_since_last_render = current_tick - last_tick;
            last_tick = current_tick;
            if (milliseconds_since_last_render < preferred_milliseconds_per_frame) {
                SDL_Delay(static_cast<Uint32>(preferred_milliseconds_per_frame - milliseconds_since_last_render));
                milliseconds_since_last_render = preferred_milliseconds_per_frame;
            }
            _game.HandleEvents();
            _game.Update(milliseconds_since_last_render);
            _game.Render();
        }
    }

} // namespace king
