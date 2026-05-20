#pragma once
#include <lvgl.h>

enum ScreenID {
    SCREEN_MENU,
    SCREEN_SETTINGS,
    SCREEN_WIFI,
    SCREEN_BATTLESHIP,
    SCREEN_CLOCK,
    SCREEN_MEMORY,
    SCREEN_PONG,
    SCREEN_CONNECT4,
    SCREEN_DOTS_BOXES,
    SCREEN_CHECKERS,
    SCREEN_CHESS,
    SCREEN_ANAGRAM,
    SCREEN_WHACK_MOLE,
    SCREEN_CUP_PONG,
    SCREEN_GAME15,
    SCREEN_PICTIONARY,
    SCREEN_COUNT
};

struct ScreenDef {
    const char*  name;
    lv_obj_t*  (*create)();
    void       (*update)();
    void       (*destroy)();
    bool         is_game;
    uint8_t      max_players;
};

void     screen_manager_init();
void     screen_manager_switch(ScreenID id);
void     screen_manager_update();
ScreenID screen_manager_current();
void     screen_manager_back_to_menu();

// Defer an in-game screen swap to the main loop. The factory function is
// invoked AFTER the outgoing screen has been freed, so the new screen does
// not have to coexist with the old one in the LVGL heap. Use this for any
// internal transition between two memory-heavy screens (e.g. battleship
// placement -> battle). The factory MUST be a non-capturing lambda /
// plain function pointer.
typedef lv_obj_t* (*ScreenFactoryFn)();
void     screen_manager_swap_to(ScreenFactoryFn factory);
