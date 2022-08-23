#include "text.h"

namespace king {

    void Text::SetText(SDL_Renderer &renderer, const std::string &text) {
        if (text != text_) {
            text_ = text;
            texture_ = Texture::CreateForText(renderer, text_, *font_, color_);
        }
    }

} // namespace king
