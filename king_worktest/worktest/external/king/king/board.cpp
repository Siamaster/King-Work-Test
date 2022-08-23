#include "board.h"


namespace king {

    std::set<BoardIndex> Board::FindMatchesInColumnsAndRows(const std::set<size_t> &columns, const std::set<size_t> &rows) const {
        std::set<BoardIndex> indices_to_remove{};
        for (auto &column : columns) {
            auto new_indices_to_remove = FindMatchesInColumn(column);
            indices_to_remove.insert(new_indices_to_remove.begin(), new_indices_to_remove.end());
        }

        for (auto &row : rows) {
            auto new_indices_to_remove = FindMatchesInRow(row);
            indices_to_remove.insert(new_indices_to_remove.begin(), new_indices_to_remove.end());
        }

        return indices_to_remove;
    }

    std::set<BoardIndex> Board::FindMatchesInColumn(size_t column) const {
        std::set<BoardIndex> indices_to_remove{};
        int matches_found = 1;
        auto last_sprite_type = board_[BoardIndex::index(column, 0)]->texture.type();
        for (size_t row = 1; row < kGridSize; row++) {
            bool last_column = row == kLastIndex;
            auto sprite_type = board_[BoardIndex::index(column, row)]->texture.type();
            if (sprite_type == last_sprite_type) {
                matches_found++;
                if (!last_column) { continue; }
            }
            if (matches_found >= 3) {
                for (int i = 0; i < matches_found; i++) {
                    size_t row_index_to_remove = row - (last_column ? 0 : 1) - i;
                    indices_to_remove.insert(BoardIndex{column, row_index_to_remove});
                }
            }
            matches_found = 1;
            last_sprite_type = sprite_type;
        }
        return indices_to_remove;
    }

    std::set<BoardIndex> Board::FindMatchesInRow(size_t row) const {
        std::set<BoardIndex> indices_to_remove{};
        int matches_found = 1;
        auto last_sprite_type = board_[BoardIndex::index(0, row)]->texture.type();
        for (size_t column = 1; column < kGridSize; column++) {
            bool last_column = column == kLastIndex;
            auto sprite_type = board_[BoardIndex::index(column, row)]->texture.type();
            if (sprite_type == last_sprite_type) {
                matches_found++;
                if (!last_column) { continue; }
            }
            if (matches_found >= 3) {
                for (int i = 0; i < matches_found; i++) {
                    size_t column_index_to_remove = column - (last_column ? 0 : 1) - i;
                    indices_to_remove.insert(BoardIndex{column_index_to_remove, row});
                }
            }
            matches_found = 1;
            last_sprite_type = sprite_type;
        }
        return indices_to_remove;
    }

    std::vector<QuadMatch> Board::FindQuadMatches(BoardIndex start_index, BoardIndex end_index) const {
        std::vector<QuadMatch> quad_matches{};
        for (size_t column = start_index.column + 1; column <= end_index.column; column++) {
            for (size_t row = start_index.row + 1; row <= end_index.row; row++) {
                if (row == 0 || column == 0 || row == kGridSize || column == kGridSize) { continue; }
                auto type = board_[BoardIndex::index(column, row)]->texture.type();
                if (type == board_[BoardIndex::index(column - 1, row - 1)]->texture.type() &&
                    type == board_[BoardIndex::index(column, row - 1)]->texture.type() &&
                    type == board_[BoardIndex::index(column - 1, row)]->texture.type() &&
                    type == board_[BoardIndex::index(column, row)]->texture.type()) {

                    QuadMatch quad_match{
                            {BoardIndex{column - 1, row - 1},
                                    BoardIndex{column, row - 1},
                                    BoardIndex{column - 1, row},
                                    BoardIndex{column, row}}
                    };
                    quad_matches.emplace_back(quad_match);
                }
            }
        }
        return quad_matches;
    }

    void Board::Swap(const BoardIndex &lhs, const BoardIndex &rhs) {
        std::unique_ptr<Sprite> temp = std::move(board_[lhs.index()]);
        board_[lhs.index()] = std::move(board_[rhs.index()]);
        board_[rhs.index()] = std::move(temp);
    }

} // namespace king