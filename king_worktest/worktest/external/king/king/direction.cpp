#include "direction.h"

#include <stdexcept>

namespace king {

    namespace direction {

        Direction opposite(const Direction &other) {
            switch (other) {
                case Direction::Up:
                    return Direction::Down;
                case Direction::Right:
                    return Direction::Left;
                case Direction::Down:
                    return Direction::Up;
                case Direction::Left:
                    return Direction::Right;
                case Direction::None:
                    throw std::runtime_error("No opposite to Direction::None");
            }
        }

        Direction direction_of_vector(const Vector2D &vector) {
            if (vector.x == vector.y) { return Direction::None; }
            if (abs(vector.x) > abs(vector.y)) { return vector.x > 0 ? Direction::Right : Direction::Left; }
            return vector.y > 0 ? Direction::Down : Direction::Up;
        }

    } // namespace direction

} // namespace king
