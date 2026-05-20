#include "cst820.h"
#include <Arduino.h>

CST820::CST820(int8_t sda_pin, int8_t scl_pin, int8_t rst_pin, int8_t int_pin)
    : _sda(sda_pin), _scl(scl_pin), _rst(rst_pin), _int(int_pin) {
}

void CST820::begin() {
    // Initialize I2C
    if (_sda != -1 && _scl != -1) {
        Wire.begin(_sda, _scl);
    } else {
        Wire.begin();
    }
    
    // Configure interrupt pin
    if (_int != -1) {
        pinMode(_int, OUTPUT);
        digitalWrite(_int, HIGH);
        delay(1);
        digitalWrite(_int, LOW);
        delay(1);
    }
    
    // Reset touch controller
    if (_rst != -1) {
        pinMode(_rst, OUTPUT);
        digitalWrite(_rst, LOW);
        delay(10);
        digitalWrite(_rst, HIGH);
        delay(300);
    }
    
    // Initialize touch controller
    i2c_write(0xFE, 0xFF);
}

bool CST820::touched() {
    return (bool)i2c_read(0x02);
}

bool CST820::getTouch(uint16_t *x, uint16_t *y, uint8_t *gesture) {
    bool finger_detected = touched();
    
    if (gesture) {
        *gesture = i2c_read(0x01);
        // Validate gesture
        if (!(*gesture == GESTURE_SLIDE_UP || *gesture == GESTURE_SLIDE_DOWN ||
              *gesture == GESTURE_SLIDE_LEFT || *gesture == GESTURE_SLIDE_RIGHT ||
              *gesture == GESTURE_SINGLE_TAP || *gesture == GESTURE_DOUBLE_TAP ||
              *gesture == GESTURE_LONG_PRESS)) {
            *gesture = GESTURE_NONE;
        }
    }
    
    // Read touch coordinates
    uint8_t data[4];
    i2c_read_continuous(0x03, data, 4);
    *x = ((data[0] & 0x0F) << 8) | data[1];
    *y = ((data[2] & 0x0F) << 8) | data[3];
    
    return finger_detected;
}

uint8_t CST820::i2c_read(uint8_t addr) {
    uint8_t data = 0;

    Wire.beginTransmission(CST820_I2C_ADDR);
    Wire.write(addr);
    if (Wire.endTransmission(false) != 0) return 0;  // no ACK — bail out
    if (Wire.requestFrom(CST820_I2C_ADDR, 1) == 0) return 0;

    while (Wire.available()) {
        data = Wire.read();
    }

    return data;
}

uint8_t CST820::i2c_read_continuous(uint8_t addr, uint8_t *data, uint32_t length) {
    Wire.beginTransmission(CST820_I2C_ADDR);
    Wire.write(addr);
    if (Wire.endTransmission(true)) {
        return 1;
    }
    
    Wire.requestFrom(CST820_I2C_ADDR, length);
    for (uint32_t i = 0; i < length; i++) {
        *data++ = Wire.available() ? Wire.read() : 0;
    }

    return 0;
}

void CST820::i2c_write(uint8_t addr, uint8_t data) {
    Wire.beginTransmission(CST820_I2C_ADDR);
    Wire.write(addr);
    Wire.write(data);
    Wire.endTransmission();
}