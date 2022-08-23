#include "explode.h"

#include "king/math.h"

namespace king {

    namespace animation {

        static const int kTravelDistance = 200;
        static const float kDuration = 1500; // milliseconds

        void Explode::Update(int milliseconds) {
            if (is_done_) return;

            static auto ease_out_function = math::EaseOutSine;

            top_right_.x = static_cast<int>(ease_out_function(current_time, initial_top_right_.x, kTravelDistance, kDuration));
            top_right_.y = static_cast<int>(ease_out_function(current_time, initial_top_right_.y, -kTravelDistance, kDuration));
            bottom_right_.x = static_cast<int>(ease_out_function(current_time, initial_bottom_right_.x, kTravelDistance, kDuration));
            bottom_right_.y = static_cast<int>(ease_out_function(current_time, initial_bottom_right_.y, kTravelDistance, kDuration));
            bottom_left_.x = static_cast<int>(ease_out_function(current_time, initial_bottom_left_.x, -kTravelDistance, kDuration));
            bottom_left_.y = static_cast<int>(ease_out_function(current_time, initial_bottom_left_.y, kTravelDistance, kDuration));
            top_left_.x = static_cast<int>(ease_out_function(current_time, initial_top_left_.x, -kTravelDistance, kDuration));
            top_left_.y = static_cast<int>(ease_out_function(current_time, initial_top_right_.y, -kTravelDistance, kDuration));

            width_ = static_cast<int>(ease_out_function(current_time, texture_->width(), -texture_->width(), kDuration));
            height_ = static_cast<int>(ease_out_function(current_time, texture_->height(), -texture_->height(), kDuration));
            alpha_ = static_cast<Uint8>(ease_out_function(current_time, kStartAlpha, -kStartAlpha, kDuration));

            current_time += milliseconds;
            is_done_ = current_time >= kDuration;
        }

        void Explode::Render(SDL_Renderer &renderer) const {
            SDL_SetTextureAlphaMod(texture_->sdl_texture(), alpha_);
            SDL_Rect rect = {top_right_.x, top_right_.y, width_, height_};
            SDL_RenderCopy(&renderer, texture_->sdl_texture(), nullptr, &rect);
            rect = {bottom_right_.x, bottom_right_.y, width_, height_};
            SDL_RenderCopy(&renderer, texture_->sdl_texture(), nullptr, &rect);
            rect = {bottom_left_.x, bottom_left_.y, width_, height_};
            SDL_RenderCopy(&renderer, texture_->sdl_texture(), nullptr, &rect);
            rect = {top_left_.x, top_left_.y, width_, height_};
            SDL_RenderCopy(&renderer, texture_->sdl_texture(), nullptr, &rect);
        }

    } // namespace animation

} // namespace king
