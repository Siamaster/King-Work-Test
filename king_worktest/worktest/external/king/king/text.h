#pragma once

#include "texture.h"
#include "vector2d.h"

namespace king {

    class Text {
    public:
        Text(SDL_Renderer &renderer,
             const std::shared_ptr<TTF_Font> &font,
             const std::string &text,
             Vector2D position = {0, 0},
             SDL_Color color = {225, 225, 225, 255}) :
                position_(position),
                text_(text),
                color_(color),
                font_(font),
                texture_(Texture::CreateForText(renderer, text, *font, color)) {}

        void SetText(SDL_Renderer &renderer, const std::string &text);

        Vector2D &position() { return position_; }

        Texture &texture() { return *texture_; }

    private:
        Vector2D position_;
        std::string text_;
        SDL_Color color_;
        std::shared_ptr<TTF_Font> font_;
        std::unique_ptr<Texture> texture_;
    };

} // namespace king
