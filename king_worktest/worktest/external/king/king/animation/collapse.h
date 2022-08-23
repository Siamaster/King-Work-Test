#pragma once

#include "king/sprite.h"

#include <map>

namespace king {

    namespace animation {

        class Collapse {
        public:

            Collapse(std::map<Sprite *, int> destination_y_positions, std::function<void()> on_done)
                    : destination_y_positions_{std::move(destination_y_positions)},
                      on_done_{std::move(on_done)} {}

            void Update();

            auto is_done() const { return is_done_; }

        private:

            std::map<Sprite *, int> destination_y_positions_;
            std::function<void()> on_done_;

            float speed_ = .0f;

            bool is_done_ = false;
        };

    } // namespace animation

} // namespace king
