#pragma once

#include "king/board_index.h"
#include "king/direction.h"
#include "king/sprite.h"

namespace king {

    namespace animation {

        class Swap {
        public:

            Swap(Sprite *sprite, const Vector2D &destination, Direction moving_direction, std::function<void()> on_done = nullptr) :
                    sprite_{sprite},
                    destination_{destination},
                    moving_direction_{moving_direction},
                    on_done_{std::move(on_done)} {}

            void Update();
            void Finish();

            auto sprite() const { return sprite_; }

            auto destination() const { return destination_; }

            auto is_done() const { return is_done_; }

        private:
            Sprite *sprite_;
            Vector2D destination_;
            Direction moving_direction_;
            std::function<void()> on_done_;

            bool is_done_ = false;
        };

    } // namespace animation

} // namespace king


