#include "swap.h"

namespace king {

    namespace animation {

        static const int kSwapSpeed = 1;

        void Swap::Update() {
            if (is_done_) return;
            switch (moving_direction_) {
                case Direction::Up:
                    sprite_->position.y -= kSwapSpeed;
                    is_done_ = sprite_->position.y <= destination_.y;
                    break;
                case Direction::Right:
                    sprite_->position.x += kSwapSpeed;
                    is_done_ = sprite_->position.x >= destination_.x;
                    break;
                case Direction::Down:
                    sprite_->position.y += kSwapSpeed;
                    is_done_ = sprite_->position.y >= destination_.y;
                    break;
                case Direction::Left:
                    sprite_->position.x -= kSwapSpeed;
                    is_done_ = sprite_->position.x <= destination_.x;
                    break;
                case Direction::None:
                    throw std::runtime_error("Swap with direction none");
            }

            if (is_done_) {
                sprite_->position = destination_;
                if (on_done_ != nullptr) {
                    on_done_();
                }
            }
        }

        void Swap::Finish() {
            sprite_->position = destination_;
            is_done_ = true;
            // Sprites with on_done_ functions doesn't get finished
        }

    } // namespace animation

} // namespace king
