#include "texture.h"

#include "SDL_image.h"

#include <cstring>
#include <map>

namespace king {

    Texture &Texture::LazyLoad(SDL_Renderer &renderer, Texture::Type type) {
        static std::map<Texture::Type, Texture *> cache;

        if (cache.find(type) != cache.end()) {
            return *cache.at(type);
        }

        Texture *texture = LoadNewTexture(renderer, type);
        cache.insert(std::pair<Texture::Type, Texture *>{type, texture});
        return *texture;
    }


    std::unique_ptr<Texture> Texture::LoadNew(SDL_Renderer &renderer, Texture::Type type) {
        return std::unique_ptr<Texture>(LoadNewTexture(renderer, type));
    }

    std::unique_ptr<Texture> Texture::CreateForText(SDL_Renderer &renderer, const std::string &text, TTF_Font &font, SDL_Color color) {

        if (auto surface = TTF_RenderText_Blended(&font, text.c_str(), color)) {
            return std::unique_ptr<Texture>{NewTextureFromSurface(renderer, surface, Type::Text)};
        }

        throw std::runtime_error{SDL_GetError()};
    }

    Texture *Texture::LoadNewTexture(SDL_Renderer &renderer, Texture::Type type) {
        const char *file_name;
        char assets_directory[512] = ASSETS_DIRECTORY;
        switch (type) {
            case Texture::Type::Background :
                file_name = strcat(assets_directory, "Background.jpg");
                break;
            case Texture::Type::Blue :
                file_name = strcat(assets_directory, "Blue.png");
                break;
            case Texture::Type::Green :
                file_name = strcat(assets_directory, "Green.png");
                break;
            case Texture::Type::Purple :
                file_name = strcat(assets_directory, "Purple.png");
                break;
            case Texture::Type::Red :
                file_name = strcat(assets_directory, "Red.png");
                break;
            case Texture::Type::Yellow :
                file_name = strcat(assets_directory, "Yellow.png");
                break;
            default:
                throw std::runtime_error{"Illegal argument. Can't LazyLoad Type::Text or None"};
        }

        if (auto surface = IMG_Load(file_name)) {
            return NewTextureFromSurface(renderer, surface, type);
        }

        throw std::runtime_error{SDL_GetError()};
    }

    Texture *Texture::NewTextureFromSurface(SDL_Renderer &renderer, SDL_Surface *surface, Texture::Type type) {
        auto texture = SDL_CreateTextureFromSurface(&renderer, surface);
        SDL_FreeSurface(surface);
        int width, height;
        SDL_QueryTexture(texture, nullptr, nullptr, &width, &height);

        return new Texture{
                width,
                height,
                std::unique_ptr<SDL_Texture, std::function<void(SDL_Texture *)>>(texture, SDL_DestroyTexture),
                type
        };
    }

} // namespace king
