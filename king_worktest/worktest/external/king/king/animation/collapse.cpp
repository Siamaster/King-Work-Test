#include "collapse.h"

namespace king {

    namespace animation {

        static const int kMaxSpeed = 10;
        static const float kAcceleration = .25f;

        void Collapse::Update() {
            if (is_done_) { return; }

            bool all_animations_are_done = true;
            for (auto &[key, val] : destination_y_positions_) {
                if (key->position.y >= val) {
                    key->position.y = val;
                } else {
                    key->position.y += static_cast<int>(round(speed_));
                    all_animations_are_done = false;
                }
            }

            is_done_ = all_animations_are_done;

            if (speed_ > kMaxSpeed) {
                speed_ = kMaxSpeed;
            } else {
                speed_ += kAcceleration;
            }

            if (is_done_) {
                on_done_();
            }
        }

    } // namespace animation

} // namespace king
