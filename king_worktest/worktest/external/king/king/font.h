#pragma once

#include "SDL_ttf.h"

#include <memory>

namespace king {

    namespace font {

        std::shared_ptr<TTF_Font> Load(int size);

    } // namespace font

} // namespace king
