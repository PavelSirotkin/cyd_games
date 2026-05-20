#pragma once
#include "../game_base.h"

class Game15 : public GameBase {
public:
    lv_obj_t* createScreen() override;
    void update() override;
    void destroy() override;

    const char* name() const override { return "15 Puzzle"; }
    uint8_t maxPlayers() const override { return 1; }

private:
    static const int BOARD_SIZE = 4;
    static const int TILE_SIZE = 42;
    static const int TILE_GAP = 3;
    
    lv_obj_t* screen_ = nullptr;
    lv_obj_t* tiles_[BOARD_SIZE][BOARD_SIZE] = {};
    lv_obj_t* move_label_ = nullptr;
    lv_obj_t* win_label_ = nullptr;
    
    int board_[BOARD_SIZE][BOARD_SIZE] = {};
    int empty_row_ = 3;
    int empty_col_ = 3;
    int move_count_ = 0;
    bool game_won_ = false;
    
    void init_game();
    void shuffle_board();
    void draw_board();
    bool move_tile(int row, int col);
    bool check_win();
    void update_move_counter();
    void show_win_message();
    
    static void tile_event_handler(lv_event_t* e);
    static void new_game_event_handler(lv_event_t* e);
};
