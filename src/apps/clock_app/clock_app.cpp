#include "clock_app.h"
#include "../../ui/ui_common.h"
#include "../../ui/screen_manager.h"
#include <Arduino.h>
#include <time.h>

static lv_obj_t* screen_ = nullptr;
static lv_obj_t* lbl_time_ = nullptr;
static lv_obj_t* lbl_date_ = nullptr;

static void back_cb(lv_event_t*) { 
    screen_manager_back_to_menu(); 
}

lv_obj_t* clock_app_create() {
    screen_ = ui_create_screen();
    
    // Back button
    lv_obj_t* btn_back = ui_create_btn(screen_, LV_SYMBOL_LEFT " Menu", 100, 32);
    lv_obj_align(btn_back, LV_ALIGN_TOP_LEFT, 4, 4);
    lv_obj_add_event_cb(btn_back, back_cb, LV_EVENT_CLICKED, NULL);
    
    // Title
    lv_obj_t* lbl_title = lv_label_create(screen_);
    lv_label_set_text(lbl_title, "Clock");
    lv_obj_set_style_text_font(lbl_title, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(lbl_title, UI_COLOR_PRIMARY, 0);
    lv_obj_align(lbl_title, LV_ALIGN_TOP_MID, 0, 8);
    
    // Time display with seconds - large (24-hour format)
    lbl_time_ = lv_label_create(screen_);
    lv_obj_set_style_text_font(lbl_time_, &lv_font_montserrat_48, 0);
    lv_obj_set_style_text_color(lbl_time_, lv_color_hex(0x4ecca3), 0);
    lv_obj_align(lbl_time_, LV_ALIGN_CENTER, 0, -10);
    lv_label_set_text(lbl_time_, "00:00:00");
    
    // Date display
    lbl_date_ = lv_label_create(screen_);
    lv_obj_set_style_text_font(lbl_date_, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(lbl_date_, UI_COLOR_DIM, 0);
    lv_obj_align(lbl_date_, LV_ALIGN_BOTTOM_MID, 0, -20);
    lv_label_set_text(lbl_date_, "");
    
    return screen_;
}

void clock_app_update() {
    static uint32_t last = 0;
    if (millis() - last < 100) return;
    last = millis();
    
    struct tm t;
    if (!getLocalTime(&t, 0)) return;
    
    // Update time (HH:MM:SS in 24-hour format)
    char time_buf[16];
    snprintf(time_buf, sizeof(time_buf), "%02d:%02d:%02d", 
             t.tm_hour, t.tm_min, t.tm_sec);
    if (lbl_time_) lv_label_set_text(lbl_time_, time_buf);
    
    // Update date
    static const char* days[] = {"Sunday","Monday","Tuesday","Wednesday",
                                  "Thursday","Friday","Saturday"};
    static const char* months[] = {"Jan","Feb","Mar","Apr","May","Jun",
                                    "Jul","Aug","Sep","Oct","Nov","Dec"};
    char date_buf[48];
    snprintf(date_buf, sizeof(date_buf), "%s, %s %d, %d", 
             days[t.tm_wday], months[t.tm_mon], t.tm_mday, 1900 + t.tm_year);
    if (lbl_date_) lv_label_set_text(lbl_date_, date_buf);
}

void clock_app_destroy() {
    screen_ = nullptr;
    lbl_time_ = nullptr;
    lbl_date_ = nullptr;
}
