#pragma once

#include "sprite.h"
#include "quad_match.h"

#include <memory>
#include <vector>

namespace king {

    class Board {
    public:

        std::set<BoardIndex> FindMatchesInColumnsAndRows(const std::set<size_t> &columns, const std::set<size_t> &rows) const;
        std::set<BoardIndex> FindMatchesInColumn(size_t column) const;
        std::set<BoardIndex> FindMatchesInRow(size_t row) const;
        std::vector<QuadMatch> FindQuadMatches(BoardIndex start_index, BoardIndex end_index) const;
        void Swap(const BoardIndex &lhs, const BoardIndex &rhs); // std::swap will cause bugs

        auto &operator[](const BoardIndex &index) { return board_[index.index()]; }

        const auto &operator[](const BoardIndex &index) const { return board_[index.index()]; }

        auto begin() const { return board_.begin(); }

        auto end() const { return board_.end(); }

    private:
        std::array<std::unique_ptr<Sprite>, kGridSize * kGridSize> board_;
    };

} // namespace king

