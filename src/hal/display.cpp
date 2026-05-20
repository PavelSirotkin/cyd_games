#include "display.h"
#include "config.h"
#include "cst820.h"
#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();  // Non-static: accessible via extern from settings

// CST820 capacitive touch on I2C
static CST820 touch(TOUCH_SDA, TOUCH_SCL, TOUCH_RST, TOUCH_INT);

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf1[SCREEN_WIDTH * LVGL_BUF_LINES];
static lv_color_t buf2[SCREEN_WIDTH * LVGL_BUF_LINES];
static lv_disp_drv_t disp_drv;
static lv_indev_drv_t indev_drv;
static lv_disp_t* disp = nullptr;

static hw_timer_t* lvgl_tick_timer = nullptr;

static void IRAM_ATTR lvgl_tick_isr() {
    lv_tick_inc(LV_TICK_PERIOD_MS);
}

static void disp_flush(lv_disp_drv_t* drv, const lv_area_t* area, lv_color_t* color_p) {
    uint32_t w = area->x2 - area->x1 + 1;
    uint32_t h = area->y2 - area->y1 + 1;
    tft.startWrite();
    tft.setAddrWindow(area->x1, area->y1, w, h);
    tft.pushColors((uint16_t*)color_p, w * h, true);
    tft.endWrite();
    lv_disp_flush_ready(drv);
}

static void touch_read(lv_indev_drv_t* drv, lv_indev_data_t* data) {
    uint16_t x, y;
    if (touch.getTouch(&x, &y)) {
        data->state = LV_INDEV_STATE_PR;
        // CST820 returns coordinates in portrait mode, rotate to landscape
        data->point.x = y;
        data->point.y = SCREEN_HEIGHT - 1 - x;
        
        // Clamp to screen bounds
        if (data->point.x < 0) data->point.x = 0;
        if (data->point.x >= SCREEN_WIDTH) data->point.x = SCREEN_WIDTH - 1;
        if (data->point.y < 0) data->point.y = 0;
        if (data->point.y >= SCREEN_HEIGHT) data->point.y = SCREEN_HEIGHT - 1;
    } else {
        data->state = LV_INDEV_STATE_REL;
    }
}

void display_init() {
    // Init display
    tft.init();
    tft.setRotation(1);  // Landscape 320x240
    tft.fillScreen(TFT_BLACK);

    // Init CST820 capacitive touch
    touch.begin();

    // Init LVGL
    lv_init();

    lv_disp_draw_buf_init(&draw_buf, buf1, buf2, SCREEN_WIDTH * LVGL_BUF_LINES);

    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res  = SCREEN_WIDTH;
    disp_drv.ver_res  = SCREEN_HEIGHT;
    disp_drv.flush_cb = disp_flush;
    disp_drv.draw_buf = &draw_buf;
    disp = lv_disp_drv_register(&disp_drv);

    lv_indev_drv_init(&indev_drv);
    indev_drv.type    = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = touch_read;
    lv_indev_drv_register(&indev_drv);

    // Hardware timer for LVGL tick (timer 0, prescaler 80 = 1MHz)
    lvgl_tick_timer = timerBegin(0, 80, true);
    timerAttachInterrupt(lvgl_tick_timer, &lvgl_tick_isr, true);
    timerAlarmWrite(lvgl_tick_timer, LV_TICK_PERIOD_MS * 1000, true);
    timerAlarmEnable(lvgl_tick_timer);
}

lv_disp_t* display_get() {
    return disp;
}

void display_set_inverted(bool inverted) {
    tft.invertDisplay(inverted);
}
