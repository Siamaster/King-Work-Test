#pragma once

#include "board_index.h"

#include <set>

namespace king {

    struct QuadMatch {
        std::set<BoardIndex> indices;
    };

} // namespace king
