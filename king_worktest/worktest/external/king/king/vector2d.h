#pragma once

namespace king {

    struct Vector2D {
        int x, y;

        Vector2D operator+(const Vector2D &add) const {
            return {x + add.x, y + add.y};
        }

        Vector2D &operator+=(const Vector2D &add) {
            x += add.x;
            y += add.y;
            return *this;
        }

        Vector2D &operator+=(int add) {
            x += add;
            y += add;
            return *this;
        }

        Vector2D operator-(const Vector2D &subtract) const {
            return {x - subtract.x, y - subtract.y};
        }

        Vector2D &operator-=(const Vector2D &subtract) {
            x -= subtract.x;
            y -= subtract.y;
            return *this;
        }

        Vector2D &operator-=(int subtract) {
            x -= subtract;
            y -= subtract;
            return *this;
        }



        bool operator==(const Vector2D &other) const {
            return x == other.x && y == other.y;
        }

        bool operator!=(const Vector2D &other) const {
            return !(*this == other);
        }
    };

}
