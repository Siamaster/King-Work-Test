#pragma once

#include <cstdlib>

namespace king {

    const int kGridSize = 8;
    const size_t kLastIndex = kGridSize - 1;

    struct BoardIndex {
        size_t column, row;

        static size_t index(size_t column, size_t row) { return row * kGridSize + column; }

        size_t index() const { return index(column, row); }

        bool operator<(const BoardIndex &other) const {
            return index() < other.index();
        }

        bool operator==(const BoardIndex &other) const {
            return column == other.column && row == other.row;
        }
    };

}