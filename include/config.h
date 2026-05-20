#pragma once

// ── Display (ST7789 - JC2432W328RC) ──
#define TFT_BL_PIN      27
#define SCREEN_WIDTH    320
#define SCREEN_HEIGHT   240

// ── CST820 Touch (I2C capacitive) ──
#define TOUCH_SDA       33
#define TOUCH_SCL       32
#define TOUCH_RST       25
#define TOUCH_INT       21

// ── RGB LED (active LOW) ──
#define LED_R_PIN        4
#define LED_G_PIN       16
#define LED_B_PIN       17

// ── Audio ──
#define SPEAKER_PIN     22

// ── Light sensor ──
#define LDR_PIN         34

// ── Buttons ──
#define BOOT_BTN_PIN     0

// ── Networking ──
#define DISCOVERY_PORT  4328
#define OTA_PORT          80
#define MAX_PEERS          8
#define ESPNOW_CHANNEL    1

// ── LVGL buffers ──
#define LVGL_BUF_LINES   8
