#include <king/game_loop.h>

int main(int argc, char *argv[]) {

    // Initialize
    if (SDL_Init(SDL_INIT_VIDEO) != 0) { throw std::runtime_error{"Failed to initialize SDL"}; };
    if (TTF_Init() != 0) { throw std::runtime_error{"Failed to initialize SDL TTF"}; }

    // Run game in it's own block. We want game to be deleted before we quit the libraries.
    {
        king::Game game{};
        king::GameLoop game_loop{game};
        game_loop.Run();
    }

    // Quit
    SDL_Quit();
    TTF_Quit();

    return 0;
}


