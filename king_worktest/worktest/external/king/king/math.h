#pragma once

#include <math.h>

namespace king {

    namespace math {

        const float PI = 3.1415927f;

        float EaseOutCubic(float current_time, float start_value, float total_change, float duration) {
            return total_change * ((current_time = current_time / duration - 1) * current_time * current_time + 1) + start_value;
        }

        float EaseOutQuad(float current_time, float start_value, float total_change, float duration) {
            return -total_change * (current_time /= duration) * (current_time - 2) + start_value;
        }

        float EaseOutSine(float current_time, float start_value, float total_change, float duration) {
            return total_change * sin(current_time / duration * (PI / 2)) + start_value;
        }

    } // namespace math

} // namespace king