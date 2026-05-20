#include "game15.h"
#include "../../ui/ui_common.h"
#include "../../ui/screen_manager.h"
#include <cstdlib>
#include <ctime>

lv_obj_t* Game15::createScreen() {
    screen_ = lv_obj_create(nullptr);
    lv_obj_set_style_bg_color(screen_, lv_color_hex(0x1a1a1a), 0);
    
    // Горизонтальная ориентация - поворачиваем экран
    lv_obj_set_size(screen_, 320, 240);
    
    // Кнопка "Назад" (слева вверху)
    ui_create_back_btn(screen_);
    
    // Заголовок (по центру вверху)
    lv_obj_t* title = lv_label_create(screen_);
    lv_label_set_text(title, "15 PUZZLE");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(title, lv_color_hex(0xFFFFFF), 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 8);
    
    // Счетчик ходов (по центру под заголовком)
    move_label_ = lv_label_create(screen_);
    lv_label_set_text(move_label_, "Moves: 0");
    lv_obj_set_style_text_font(move_label_, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(move_label_, lv_color_hex(0xFFFFFF), 0);
    lv_obj_align(move_label_, LV_ALIGN_TOP_MID, 0, 28);
    
    // Кнопка "Новая игра" (справа вверху)
    lv_obj_t* new_game_btn = lv_btn_create(screen_);
    lv_obj_set_size(new_game_btn, 60, 30);
    lv_obj_align(new_game_btn, LV_ALIGN_TOP_RIGHT, -5, 5);
    lv_obj_add_event_cb(new_game_btn, new_game_event_handler, LV_EVENT_CLICKED, this);
    
    lv_obj_t* btn_label = lv_label_create(new_game_btn);
    lv_label_set_text(btn_label, "New");
    lv_obj_set_style_text_font(btn_label, &lv_font_montserrat_12, 0);
    lv_obj_center(btn_label);
    
    // Создание плиток - центрируем на экране
    int board_width = BOARD_SIZE * TILE_SIZE + (BOARD_SIZE - 1) * TILE_GAP;
    int board_height = BOARD_SIZE * TILE_SIZE + (BOARD_SIZE - 1) * TILE_GAP;
    int board_offset_x = (320 - board_width) / 2;
    int board_offset_y = (240 - board_height) / 2 + 10;
    
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            tiles_[i][j] = lv_btn_create(screen_);
            lv_obj_set_size(tiles_[i][j], TILE_SIZE, TILE_SIZE);
            lv_obj_set_pos(tiles_[i][j],
                          board_offset_x + j * (TILE_SIZE + TILE_GAP),
                          board_offset_y + i * (TILE_SIZE + TILE_GAP));
            
            lv_obj_add_event_cb(tiles_[i][j], tile_event_handler, LV_EVENT_CLICKED, this);
            
            // Стиль плитки
            lv_obj_set_style_bg_color(tiles_[i][j], lv_color_hex(0x2196F3), 0);
            lv_obj_set_style_shadow_width(tiles_[i][j], 5, 0);
            lv_obj_set_style_shadow_color(tiles_[i][j], lv_color_hex(0x000000), 0);
            lv_obj_set_style_shadow_ofs_x(tiles_[i][j], 2, 0);
            lv_obj_set_style_shadow_ofs_y(tiles_[i][j], 2, 0);
            lv_obj_set_style_radius(tiles_[i][j], 6, 0);
        }
    }
    
    // Сообщение о победе (скрыто по умолчанию)
    win_label_ = lv_label_create(screen_);
    lv_label_set_text(win_label_, "YOU WIN!");
    lv_obj_set_style_text_font(win_label_, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(win_label_, lv_color_hex(0x00FF00), 0);
    lv_obj_align(win_label_, LV_ALIGN_CENTER, 0, -20);
    lv_obj_add_flag(win_label_, LV_OBJ_FLAG_HIDDEN);
    
    // Инициализация игры
    init_game();
    shuffle_board();
    draw_board();
    update_move_counter();
    
    return screen_;
}

void Game15::update() {
    // Игра управляется событиями, обновление не требуется
}

void Game15::destroy() {
    // Old screen is deleted by lv_scr_load_anim(auto_del=true) in
    // screen_manager_switch — deleting it here (synchronously, from inside
    // the Back button's click event) would free the object mid-event.
    screen_ = nullptr;
}

void Game15::init_game() {
    int num = 1;
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            board_[i][j] = num++;
        }
    }
    board_[BOARD_SIZE - 1][BOARD_SIZE - 1] = 0;
    empty_row_ = BOARD_SIZE - 1;
    empty_col_ = BOARD_SIZE - 1;
}

void Game15::shuffle_board() {
    // Выполняем 500 случайных допустимых ходов
    srand(millis());
    
    for (int i = 0; i < 500; i++) {
        int dir = rand() % 4;
        int new_row = empty_row_;
        int new_col = empty_col_;
        
        switch (dir) {
            case 0: new_row--; break; // Вверх
            case 1: new_row++; break; // Вниз
            case 2: new_col--; break; // Влево
            case 3: new_col++; break; // Вправо
        }
        
        if (new_row >= 0 && new_row < BOARD_SIZE && 
            new_col >= 0 && new_col < BOARD_SIZE) {
            board_[empty_row_][empty_col_] = board_[new_row][new_col];
            board_[new_row][new_col] = 0;
            empty_row_ = new_row;
            empty_col_ = new_col;
        }
    }
    
    move_count_ = 0;
}

void Game15::draw_board() {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board_[i][j] == 0) {
                lv_obj_add_flag(tiles_[i][j], LV_OBJ_FLAG_HIDDEN);
            } else {
                lv_obj_clear_flag(tiles_[i][j], LV_OBJ_FLAG_HIDDEN);
                
                // Обновляем текст на плитке
                lv_obj_t* label = lv_obj_get_child(tiles_[i][j], 0);
                if (label == nullptr) {
                    label = lv_label_create(tiles_[i][j]);
                    lv_obj_center(label);
                    lv_obj_set_style_text_font(label, &lv_font_montserrat_20, 0);
                }
                
                char text[3];
                snprintf(text, sizeof(text), "%d", board_[i][j]);
                lv_label_set_text(label, text);
                lv_obj_center(label);
                
                // Меняем цвет плитки в зависимости от правильности позиции
                int correct_num = i * BOARD_SIZE + j + 1;
                if (board_[i][j] == correct_num) {
                    lv_obj_set_style_bg_color(tiles_[i][j], lv_color_hex(0x4CAF50), 0);
                } else {
                    lv_obj_set_style_bg_color(tiles_[i][j], lv_color_hex(0x2196F3), 0);
                }
            }
        }
    }
}

bool Game15::move_tile(int row, int col) {
    // Проверяем, можно ли двигать плитку
    if ((abs(row - empty_row_) == 1 && col == empty_col_) ||
        (abs(col - empty_col_) == 1 && row == empty_row_)) {
        board_[empty_row_][empty_col_] = board_[row][col];
        board_[row][col] = 0;
        empty_row_ = row;
        empty_col_ = col;
        move_count_++;
        return true;
    }
    return false;
}

bool Game15::check_win() {
    int num = 1;
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (i == BOARD_SIZE - 1 && j == BOARD_SIZE - 1) {
                return board_[i][j] == 0;
            }
            if (board_[i][j] != num++) {
                return false;
            }
        }
    }
    return true;
}

void Game15::update_move_counter() {
    char text[20];
    snprintf(text, sizeof(text), "Moves: %d", move_count_);
    lv_label_set_text(move_label_, text);
}

void Game15::show_win_message() {
    lv_obj_clear_flag(win_label_, LV_OBJ_FLAG_HIDDEN);
    
    // Анимация победы
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, win_label_);
    lv_anim_set_values(&a, 0, 255);
    lv_anim_set_time(&a, 500);
    lv_anim_set_repeat_count(&a, 3);
    lv_anim_set_playback_time(&a, 300);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_style_opa);
    lv_anim_start(&a);
    
    Serial.printf("🎉 Game won in %d moves!\n", move_count_);
}

void Game15::tile_event_handler(lv_event_t* e) {
    Game15* game = (Game15*)lv_event_get_user_data(e);
    if (game->game_won_) return;
    
    lv_obj_t* obj = lv_event_get_target(e);
    
    // Найти координаты плитки
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (game->tiles_[i][j] == obj && game->board_[i][j] != 0) {
                if (game->move_tile(i, j)) {
                    game->draw_board();
                    game->update_move_counter();
                    
                    if (game->check_win()) {
                        game->game_won_ = true;
                        game->show_win_message();
                    }
                }
                return;
            }
        }
    }
}

void Game15::new_game_event_handler(lv_event_t* e) {
    Game15* game = (Game15*)lv_event_get_user_data(e);
    game->game_won_ = false;
    game->move_count_ = 0;
    
    if (game->win_label_ != nullptr) {
        lv_obj_add_flag(game->win_label_, LV_OBJ_FLAG_HIDDEN);
    }
    
    game->shuffle_board();
    game->draw_board();
    game->update_move_counter();
}
