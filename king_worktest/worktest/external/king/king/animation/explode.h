#pragma once

#include "king/texture.h"
#include "king/vector2d.h"

namespace king {

    namespace animation {

        class Explode {
        public:

            Explode(std::unique_ptr<Texture> texture, const Vector2D &top_right, const Vector2D &bottom_right, const Vector2D &bottom_left,
                    const Vector2D &top_left) :
                    texture_{std::move(texture)},
                    top_right_{top_right},
                    bottom_right_{bottom_right},
                    bottom_left_{bottom_left},
                    top_left_{top_left},
                    initial_top_right_{top_right},
                    initial_bottom_right_{bottom_right},
                    initial_bottom_left_{bottom_left},
                    initial_top_left_{top_left} {}

            void Update(int milliseconds);
            void Render(SDL_Renderer &renderer) const;

            auto is_done() const { return is_done_; }

        private:
            static const Uint8 kStartAlpha = 255;

            std::unique_ptr<Texture> texture_;
            Vector2D top_right_, bottom_right_, bottom_left_, top_left_,
                    initial_top_right_, initial_bottom_right_, initial_bottom_left_, initial_top_left_;

            int current_time = 0;
            int width_ = texture_->width();
            int height_ = texture_->height();
            Uint8 alpha_ = kStartAlpha;

            bool is_done_ = false;
        };

    } // namespace animation

} // namespace king

