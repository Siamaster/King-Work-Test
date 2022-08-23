#pragma once

#include "SDL_ttf.h"

#include <functional>
#include <memory>

namespace king {

    class Texture {
    public:

        enum class Type {
            None,
            Background,
            Blue,
            Green,
            Purple,
            Red,
            Yellow,
            Text
        };

        static Texture &LazyLoad(SDL_Renderer &renderer, Texture::Type type);
        static std::unique_ptr<Texture> LoadNew(SDL_Renderer &renderer, Texture::Type type);

        static std::unique_ptr<Texture> CreateForText(SDL_Renderer &renderer, const std::string &text, TTF_Font &font, SDL_Color color);

        auto width() const { return width_; }

        auto height() const { return height_; }

        auto sdl_texture() const { return sdl_texture_.get(); }

        auto type() const { return type_; }

    private:
        int width_, height_;
        std::unique_ptr<SDL_Texture, std::function<void(SDL_Texture *)>> sdl_texture_;
        Type type_;

        Texture(int width, int height, std::unique_ptr<SDL_Texture, std::function<void(SDL_Texture *)>> texture, Type type)
                : width_(width),
                  height_(height),
                  sdl_texture_(move(texture)),
                  type_(type) {}


        static Texture *LoadNewTexture(SDL_Renderer &renderer, Type type);
        static Texture *NewTextureFromSurface(SDL_Renderer &renderer, SDL_Surface *surface, Type type);
    };

} // namespace king
