#include "font.h"

#include <cstring>

namespace king {

    namespace font {

        std::shared_ptr<TTF_Font> Load(int size) {

            char assets_directory[512] = ASSETS_DIRECTORY;
            const char *file_name = strcat(assets_directory, "fonts/Roboto-Bold.ttf");

            if (auto font = TTF_OpenFont(file_name, size)) {
                return std::shared_ptr<TTF_Font>{font, TTF_CloseFont};
            }

            throw std::runtime_error{SDL_GetError()};
        }

    } // namespace font

} // namespace king
