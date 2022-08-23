#pragma once

#include "vector2d.h"

namespace king {

    enum class Direction {
        None,
        Up,
        Right,
        Down,
        Left
    };

    namespace direction {

        Direction opposite(const Direction &other);
        Direction direction_of_vector(const Vector2D &vector);

    } // namespace direction

} // namespace king
