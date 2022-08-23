#pragma once

#include "animation/collapse.h"
#include "animation/explode.h"
#include "animation/swap.h"
#include "board.h"
#include "text.h"
#include "quad_match.h"

#include <map>
#include <set>
#include <vector>

namespace king {

    class Game {
    public:

        void Init();

        void HandleEvents();
        void Update(int milliseconds);
        void Render() const;

        auto isRunning() const { return is_running_; }

    private:

        // Inner classes

        struct SwapData {
            Sprite *focused_sprite = nullptr;
            Sprite *swap_sprite = nullptr;
            BoardIndex index{}, swap_index{};
            Vector2D initial_position{}, local_dragging_point{}, mouse_location{}, initial_swap_position{};
            Direction direction = Direction::None;
            bool drag_and_drop_mode = false;
            bool click_mode = false;
            std::set<BoardIndex> clickable_indices;
        } swap_data_{};

        // Methods
        void Render(const Vector2D &position, const Texture &texture) const;

        void EnterClickMode();

        void HandleDragAndDrop();

        void CancelDragAndDrop();

        void EndSwap();

        void PerformSwap(const SwapData &swap_data); // Must take copy

        void Collapse();
        void ReCollapse(const std::set<size_t> &columns, const std::set<size_t> &rows);

        void AddExplodeAnimation(const QuadMatch &quad_match);

        // Getters

        Texture &random_sprite_texture() const;

        std::string time_text_text() const { return "Time: " + std::to_string(time_left_ / 1000); }

        std::string score_text_text() const { return "Score: " + std::to_string(score_); }

        Vector2D jewel_position(BoardIndex index) const;

        uint score(uint indices_removed, uint quads) const { return indices_removed * 5 + quads * 20; };

        // Variables

        bool is_running_ = false;

        std::unique_ptr<SDL_Window, std::function<void(SDL_Window *)>> window_ = nullptr;
        SDL_Renderer *renderer_ = nullptr;

        std::unique_ptr<Text> score_text_ = nullptr;
        std::unique_ptr<Text> time_text_ = nullptr;
        std::unique_ptr<Text> game_over_text_ = nullptr;

        int time_left_ = 6000 * 1000; // milliseconds
        bool game_over_ = false;

        uint64_t score_ = 0;

        std::vector<animation::Swap> swap_animations_;
        std::unique_ptr<animation::Collapse> collapse_animation_ = nullptr;
        std::vector<animation::Explode> explode_animations_;

        Board board_{};
    };

} // namespace king

