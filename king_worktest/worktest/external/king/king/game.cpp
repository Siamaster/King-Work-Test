#include "game.h"

#include "font.h"

#include <algorithm>
#include <random>

namespace king {

    static const int kJewelSize = 42;
    static const int kBoardOffsetX = 327;
    static const int kBoardOffsetY = 100;

    void Game::Init() {

        window_ = std::unique_ptr<SDL_Window, std::function<void(SDL_Window *)>>(
                SDL_CreateWindow("KingTest", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 0, 0, SDL_WINDOW_HIDDEN),
                SDL_DestroyWindow
        );

        renderer_ = SDL_CreateRenderer(window_.get(), -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

        SDL_SetRenderDrawColor(renderer_, 255, 255, 255, 255); // It never changes

        Texture &background = Texture::LazyLoad(*renderer_, Texture::Type::Background);

        SDL_SetWindowSize(window_.get(), background.width(), background.height());
        SDL_SetWindowPosition(window_.get(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
        SDL_ShowWindow(window_.get());

        // Initialize texts
        const auto font = king::font::Load(24);
        score_text_ = std::make_unique<Text>(*renderer_, font, "Score: 0", Vector2D{38, 110});
        time_text_ = std::make_unique<Text>(*renderer_, font, time_text_text(), Vector2D{38, 170});
        game_over_text_ = std::make_unique<Text>(*renderer_, font, "Game Over", Vector2D{400, 250}, SDL_Color{255, 0, 0, 255});

        // Initialize board
        for (size_t column = 0; column < kGridSize; column++) {
            for (size_t row = 0; row < kGridSize; row++) {
                auto &random_texture = random_sprite_texture();
                const auto new_type = random_texture.type();
                // prevent starting matches
                Texture::Type above_type = row >= 1 ? board_[{column, row - 1}]->texture.type() : Texture::Type::None;
                Texture::Type left_type = column >= 1 ? board_[{column - 1, row}]->texture.type() : Texture::Type::None;

                if ((row >= 2 && above_type == new_type && board_[{column, row - 2}]->texture.type() == new_type) ||
                    (column >= 2 && left_type == new_type && board_[{column - 2, row}]->texture.type() == new_type) ||
                    (row >= 1 && column >= 1 && new_type == above_type && new_type == left_type &&
                     new_type == board_[{column - 1, row - 1}]->texture.type())) {
                    row--;
                    continue;
                }
                board_[{column, row}] = std::make_unique<Sprite>(Sprite{random_texture, jewel_position({column, row})});
            }
        }

        is_running_ = true;
    }

    void Game::HandleEvents() {
        SDL_Event event{};
        while (SDL_PollEvent(&event) != 0) {
            switch (event.type) {
                case SDL_QUIT:
                    is_running_ = false;
                    break;
                case SDL_KEYDOWN:
                    is_running_ = event.key.keysym.sym != SDLK_ESCAPE;
                    break;
                case SDL_MOUSEBUTTONDOWN: {
                    // Only use left mouse button. Also don't permit swap while animating collapses. Also ignore if click mode is on.
                    // And don't start anything if there is an ongoing swap animation
                    if (event.button.button != SDL_BUTTON_LEFT || collapse_animation_ != nullptr || swap_data_.click_mode ||
                        !swap_animations_.empty()) { return; }


                    int column = (event.motion.x - kBoardOffsetX) / kJewelSize;
                    if (column < 0 || column >= kGridSize) { return; }
                    int row = (event.motion.y - kBoardOffsetY) / kJewelSize;
                    if (row < 0 || row >= kGridSize) { return; }

                    // Click was on a sprite
                    swap_data_.index = {static_cast<size_t>(column), static_cast<size_t>(row)};
                    swap_data_.focused_sprite = board_[{swap_data_.index.index()}].get();
                    swap_data_.initial_position = swap_data_.focused_sprite->position;
                    swap_data_.local_dragging_point = {event.motion.x - swap_data_.focused_sprite->position.x,
                                                       event.motion.y - swap_data_.focused_sprite->position.y};
                    swap_data_.mouse_location = {event.motion.x, event.motion.y};
                }
                    break;
                case SDL_MOUSEBUTTONUP:
                    if (event.button.button != SDL_BUTTON_LEFT) { return; } // Only use left mouse button
                    if (swap_data_.focused_sprite != nullptr) {
                        if (swap_data_.drag_and_drop_mode) {
                            CancelDragAndDrop();
                            return;
                        }
                        if (swap_data_.click_mode) {
                            int column = (event.motion.x - kBoardOffsetX) / kJewelSize;
                            if (column < 0 || column >= kGridSize) { return; }
                            int row = (event.motion.y - kBoardOffsetY) / kJewelSize;
                            if (row < 0 || row >= kGridSize) { return; }

                            BoardIndex index{static_cast<size_t>(column), static_cast<size_t>(row)};
                            if (swap_data_.clickable_indices.find(index) != swap_data_.clickable_indices.end()) {
                                swap_data_.swap_index = index;
                                swap_data_.swap_sprite = board_[index].get();
                                swap_data_.initial_swap_position = swap_data_.swap_sprite->position;
                                swap_data_.direction = direction::direction_of_vector(
                                        swap_data_.initial_position - swap_data_.initial_swap_position);
                                PerformSwap(swap_data_);
                            }
                            EndSwap();
                            return;
                        }
                        EnterClickMode();
                    }
                    break;
                case SDL_MOUSEMOTION:
                    if (swap_data_.focused_sprite != nullptr && !swap_data_.click_mode) {
                        swap_data_.drag_and_drop_mode = true;
                        swap_data_.mouse_location = {event.motion.x, event.motion.y};
                    }
                    break;
                default:
                    break;
            }
        }
    }

    void Game::Update(int milliseconds) {
        if (game_over_) { return; }

        time_left_ -= milliseconds;

        if (time_left_ < 0) {
            game_over_ = true;
            return;
        }

        // First loop
        for (auto &animation : swap_animations_) {
            if (!animation.is_done()) {
                animation.Update();
            }
        }

        // Then remove those that are done
        swap_animations_.erase(std::remove_if(swap_animations_.begin(), swap_animations_.end(),
                                              [](const animation::Swap &animation) {
                                                  return animation.is_done();
                                              }), swap_animations_.end());

        if (collapse_animation_ != nullptr) {
            collapse_animation_->Update();
            if (collapse_animation_->is_done()) {
                collapse_animation_.reset();
            }
        }

        for (auto iterator = explode_animations_.begin(); iterator != explode_animations_.end();) {
            if (iterator->is_done()) {
                iterator = explode_animations_.erase(iterator);
            } else {
                iterator->Update(milliseconds);
                iterator++;
            }
        }

        if (swap_data_.drag_and_drop_mode) {
            HandleDragAndDrop();
        }

        time_text_->SetText(*renderer_, time_text_text());
        score_text_->SetText(*renderer_, score_text_text());
    }

    void Game::Render() const {
        SDL_RenderClear(renderer_);

        // Render background

        Texture &background = Texture::LazyLoad(*renderer_, Texture::Type::Background);
        SDL_RenderCopy(renderer_, background.sdl_texture(), nullptr, nullptr);

        // Render board

        for (auto &jewel : board_) {
            Render(jewel->position, jewel->texture);
        }

        for (auto &animation : explode_animations_) {
            animation.Render(*renderer_);
        }

        if (swap_data_.click_mode) {
            SDL_Rect rects[swap_data_.clickable_indices.size()];
            int index = 0;
            for (auto &clickable_index : swap_data_.clickable_indices) {
                Vector2D position = jewel_position(clickable_index);
                rects[index++] = {position.x, position.y, kJewelSize - 5, kJewelSize - 5};
            }
            SDL_RenderDrawRects(renderer_, rects, swap_data_.clickable_indices.size());
        } else if (swap_data_.drag_and_drop_mode) {
            // Focused sprite should always be above in drag and drop mode
            Render(swap_data_.focused_sprite->position, swap_data_.focused_sprite->texture);
        }

        // Render texts

        Render(score_text_->position(), score_text_->texture());
        Render(time_text_->position(), time_text_->texture());

        if (game_over_) {
            Render(game_over_text_->position(), game_over_text_->texture());
        }

        SDL_RenderPresent(renderer_);
    }

    void Game::Render(const Vector2D &position, const Texture &texture) const {
        SDL_Rect rect = {position.x, position.y, texture.width(), texture.height()};
        SDL_RenderCopy(renderer_, texture.sdl_texture(), nullptr, &rect);
    }

    void Game::EnterClickMode() {
        swap_data_.click_mode = true;
        if (swap_data_.index.row != 0) {
            swap_data_.clickable_indices.insert(BoardIndex{swap_data_.index.column, swap_data_.index.row - 1});
        }
        if (swap_data_.index.column != 0) {
            swap_data_.clickable_indices.insert(BoardIndex{swap_data_.index.column - 1, swap_data_.index.row});
        }
        if (swap_data_.index.row != kLastIndex) {
            swap_data_.clickable_indices.insert(BoardIndex{swap_data_.index.column, swap_data_.index.row + 1});
        }
        if (swap_data_.index.column != kLastIndex) {
            swap_data_.clickable_indices.insert(BoardIndex{swap_data_.index.column + 1, swap_data_.index.row});
        }
    }

    void Game::HandleDragAndDrop() {
        Vector2D drag_distance = swap_data_.mouse_location - swap_data_.local_dragging_point - swap_data_.initial_position;
        if (drag_distance.x == drag_distance.y) { return; }

        // Only move horizontally or vertically
        if (abs(drag_distance.x) > abs(drag_distance.y)) {
            drag_distance.y = 0;
        } else {
            drag_distance.x = 0;
        }

        Direction direction = direction::direction_of_vector(drag_distance);

        if (swap_data_.direction != direction && swap_data_.direction != Direction::None) {
            // Animate back a cancelled swap. Direction of swap will be the direction that the cancelled swap will need to travel in.
            swap_animations_.emplace_back(swap_data_.swap_sprite, swap_data_.initial_swap_position, swap_data_.direction);
        }

        int swap_column, swap_row;

        switch (direction) {
            case Direction::Up:
                swap_column = static_cast<int>(swap_data_.index.column);
                swap_row = static_cast<int>(swap_data_.index.row) - 1;
                break;
            case Direction::Right:
                swap_column = static_cast<int>(swap_data_.index.column) + 1;
                swap_row = static_cast<int>(swap_data_.index.row);
                break;
            case Direction::Down:
                swap_column = static_cast<int>(swap_data_.index.column);
                swap_row = static_cast<int>(swap_data_.index.row) + 1;
                break;
            case Direction::Left:
                swap_column = static_cast<int>(swap_data_.index.column) - 1;
                swap_row = static_cast<int>(swap_data_.index.row);
                break;
            case Direction::None:
                throw std::runtime_error("Shouldn't happen");
        }

        bool swap_is_possible = swap_column >= 0 && swap_column < kGridSize && swap_row >= 0 && swap_row < kGridSize;

        if (swap_is_possible) {
            swap_data_.swap_index = {static_cast<size_t>(swap_column), static_cast<size_t>(swap_row)};
            swap_data_.direction = direction;
            swap_data_.swap_sprite = board_[swap_data_.swap_index].get();

            // Remove ongoing swap animation on the swap sprite.
            swap_animations_.erase(std::remove_if(swap_animations_.begin(), swap_animations_.end(),
                                                  [this](const animation::Swap &animation) {
                                                      return animation.sprite() == swap_data_.swap_sprite;
                                                  }), swap_animations_.end());

            swap_data_.initial_swap_position = jewel_position(swap_data_.swap_index);

            if (abs(drag_distance.x) > kJewelSize || abs(drag_distance.y) > kJewelSize) {
                swap_data_.swap_sprite->position = swap_data_.initial_swap_position;
                PerformSwap(swap_data_);
                EndSwap();
                return;
            }

            swap_data_.focused_sprite->position = swap_data_.initial_position + drag_distance;
            swap_data_.swap_sprite->position = swap_data_.initial_swap_position - drag_distance;
        }
    }

    void Game::CancelDragAndDrop() {
        Vector2D drag_distance = swap_data_.focused_sprite->position - swap_data_.initial_position;
        int drag_limit = kJewelSize / 2;
        bool swap_succeeded = abs(drag_distance.x) > drag_limit || abs(drag_distance.y) > drag_limit;

        if (swap_data_.direction != Direction::None) {
            Direction opposite_direction = direction::opposite(swap_data_.direction);
            if (swap_succeeded) {
                if (swap_data_.focused_sprite->position != swap_data_.initial_swap_position) { // There is no animation to do
                    swap_animations_.emplace_back(swap_data_.focused_sprite, swap_data_.initial_swap_position, swap_data_.direction);
                    SwapData copy = swap_data_;
                    swap_animations_.emplace_back(swap_data_.swap_sprite, swap_data_.initial_position,
                                                  opposite_direction, std::bind(&Game::PerformSwap, this, copy));
                }
            } else if (swap_data_.focused_sprite->position != swap_data_.initial_position) {
                swap_animations_.emplace_back(swap_data_.focused_sprite, swap_data_.initial_position, opposite_direction);
                swap_animations_.emplace_back(swap_data_.swap_sprite, swap_data_.initial_swap_position, swap_data_.direction);
            }
        }
        EndSwap();
    }

    void Game::EndSwap() {
        swap_data_.focused_sprite = nullptr; // We only handle drag & drop when this is not null
        swap_data_.direction = Direction::None; // Needed to differentiate new swaps from the last one
        swap_data_.click_mode = false;
        swap_data_.drag_and_drop_mode = false;
        swap_data_.clickable_indices.clear();
    }

    void Game::PerformSwap(const SwapData &swap_data) { // Important to use local variable throughout the function
        swap_data.focused_sprite->position = swap_data.initial_swap_position;
        swap_data.swap_sprite->position = swap_data.initial_position;

        board_.Swap(swap_data.index, swap_data.swap_index);

        // Remove all possible matches for the swap
        std::vector<QuadMatch> quad_matches{};
        std::set<size_t> search_columns{}, search_rows{};
        search_columns.insert(swap_data.index.column);
        search_rows.insert(swap_data.index.row);
        size_t lowest_column, lowest_row;
        if (swap_data.swap_index.column == swap_data.index.column) { // Swap on same column
            search_rows.insert(swap_data.swap_index.row);
            lowest_column = swap_data.index.column;
            lowest_row = std::min(swap_data.index.row, swap_data.swap_index.row);
        } else {
            search_columns.insert(swap_data.swap_index.column);
            lowest_column = std::min(swap_data.index.column, swap_data.swap_index.column);
            lowest_row = swap_data.index.row;
        }

        size_t zero = 0; // otherwise, I need to cast. There is no literal for this
        size_t start_column = std::max(lowest_column - 1, zero);
        size_t start_row = std::max(lowest_row - 1, zero);
        size_t end_column = std::min(lowest_column + 2, kLastIndex);
        size_t end_row = std::min(lowest_row + 2, kLastIndex);

        quad_matches = board_.FindQuadMatches({start_column, start_row}, {end_column, end_row});

        std::set<BoardIndex> indices_to_remove = board_.FindMatchesInColumnsAndRows(search_columns, search_rows);

        for (auto &quad_match : quad_matches) {
            indices_to_remove.insert(quad_match.indices.begin(), quad_match.indices.end());
            AddExplodeAnimation(quad_match);
        }

        if (indices_to_remove.empty()) { // No matches found, swap back.
            board_.Swap(swap_data.index, swap_data.swap_index);
            if (swap_data.click_mode) {
                swap_data.focused_sprite->position = swap_data.initial_position;
                swap_data.swap_sprite->position = swap_data.initial_swap_position;
                return;
            }
            swap_animations_.emplace_back(swap_data.focused_sprite, swap_data.initial_position,
                                          direction::opposite(swap_data.direction));
            swap_animations_.emplace_back(swap_data.swap_sprite, swap_data.initial_swap_position, swap_data.direction);
            return;
        }

        score_ += score(indices_to_remove.size(), quad_matches.size());

        // First finish all swap animation
        for (auto &animation : swap_animations_) {
            animation.Finish();
        }

        // Then remove at indices
        for (auto &index : indices_to_remove) {
            board_[index].reset();
        }

        Collapse();
    }

    void Game::Collapse() {

        std::map<Sprite *, int> new_y_positions{};
        size_t highest_row_with_sprite = 0;
        size_t start_column_index = kGridSize;
        size_t end_column_index = 0;

        for (size_t column = 0; column < kGridSize; column++) {
            size_t highest_row_with_sprite_in_column = kGridSize;
            for (size_t row = kLastIndex;; row--) {
                if (board_[{column, row}] == nullptr) {
                    highest_row_with_sprite_in_column = row;
                    break;
                }
                if (row == 0) { break; }
            }

            if (highest_row_with_sprite_in_column == kGridSize) { continue; } // nothing to collapse in this column

            highest_row_with_sprite = std::max<size_t>(highest_row_with_sprite, highest_row_with_sprite_in_column);
            if (start_column_index == kGridSize) {
                start_column_index = column;
            }
            end_column_index = column;

            std::vector<Sprite *> sprites_above_lowest_row;

            if (highest_row_with_sprite_in_column > 0) {
                // Swap jewels to new locations
                for (size_t row = highest_row_with_sprite_in_column - 1, swaps = 0;; row--) {
                    std::unique_ptr<Sprite> &sprite = board_[{column, row}];
                    if (sprite != nullptr) {
                        sprites_above_lowest_row.emplace_back(sprite.get());
                        size_t row_index_to_swap_with = highest_row_with_sprite_in_column - swaps;
                        // Find new y position for each falling jewel
                        new_y_positions[sprite.get()] = kBoardOffsetY + kJewelSize * static_cast<int>(row_index_to_swap_with);
                        board_.Swap({column, row}, {column, row_index_to_swap_with});
                        swaps++;
                    }
                    if (row == 0) { break; }
                }
            }

            // Create new jewel for each sprite removed
            size_t jewels_removed_in_row = highest_row_with_sprite_in_column + 1 - sprites_above_lowest_row.size();
            for (size_t row = 0; row < jewels_removed_in_row; row++) {
                Vector2D position{kBoardOffsetX + kJewelSize * static_cast<int>(column),
                                  static_cast<int>(kBoardOffsetY - (kJewelSize * (jewels_removed_in_row - row)))};
                auto new_sprite = new Sprite{random_sprite_texture(), position};
                board_[{column, row}].reset(new_sprite);

                new_y_positions[new_sprite] = kBoardOffsetY + kJewelSize * static_cast<int>(row);
            }
        }

        std::set<size_t> search_columns{}, search_rows{};
        for (size_t row = 0; row <= highest_row_with_sprite; row++) {
            search_rows.insert(row);
        }

        for (size_t column = start_column_index; column <= end_column_index; column++) {
            search_columns.insert(column);
        }

        collapse_animation_ = std::make_unique<animation::Collapse>(new_y_positions,
                                                                    std::bind(&Game::ReCollapse, this, search_columns, search_rows));
    }

    void Game::ReCollapse(const std::set<size_t> &columns, const std::set<size_t> &rows) {
        std::set<BoardIndex> indices_to_remove = board_.FindMatchesInColumnsAndRows(columns, rows);

        size_t quad_begin_column = *columns.cbegin();
        if (quad_begin_column != 0) { quad_begin_column--; }
        size_t quad_begin_row = *rows.cbegin();
        if (quad_begin_row != 0) { quad_begin_row--; }
        size_t quad_end_column = *columns.crbegin();
        if (quad_end_column != kLastIndex) { quad_end_column++; }
        size_t quad_end_row = *rows.crbegin();
        if (quad_end_row != kLastIndex) { quad_end_row++; }

        auto quad_matches = board_.FindQuadMatches({quad_begin_column, quad_begin_row}, {quad_end_column, quad_end_row});

        for (auto &quad_match : quad_matches) {
            indices_to_remove.insert(quad_match.indices.begin(), quad_match.indices.end());
            AddExplodeAnimation(quad_match);
        }

        score_ += score(indices_to_remove.size(), quad_matches.size());

        if (!indices_to_remove.empty()) {
            for (auto &index : indices_to_remove) {
                board_[index].reset();
            }
            Collapse();
        }
    }

    void Game::AddExplodeAnimation(const QuadMatch &quad_match) {
        BoardIndex index_of_top_left = *quad_match.indices.cbegin();
        explode_animations_.emplace_back(
                Texture::LoadNew(*renderer_, board_[index_of_top_left]->texture.type()),
                jewel_position({index_of_top_left.column + 1, index_of_top_left.row}),
                jewel_position({index_of_top_left.column + 1, index_of_top_left.row + 1}),
                jewel_position({index_of_top_left.column, index_of_top_left.row + 1}),
                jewel_position(index_of_top_left));
    }

    Texture &Game::random_sprite_texture() const {

        static std::random_device seed;
        static std::mt19937 generator{seed()};
        static std::uniform_int_distribution<int> distributor{0, 4};
        int random = distributor(generator);
        switch (random) {
            case 0:
                return Texture::LazyLoad(*renderer_, Texture::Type::Blue);
            case 1:
                return Texture::LazyLoad(*renderer_, Texture::Type::Green);
            case 2:
                return Texture::LazyLoad(*renderer_, Texture::Type::Purple);
            case 3:
                return Texture::LazyLoad(*renderer_, Texture::Type::Red);
            default:
                return Texture::LazyLoad(*renderer_, Texture::Type::Yellow);
        }
    }

    Vector2D Game::jewel_position(BoardIndex index) const {
        return {
                kBoardOffsetX + kJewelSize * static_cast<int>(index.column),
                kBoardOffsetY + kJewelSize * static_cast<int>(index.row)
        };
    }

} // namespace king