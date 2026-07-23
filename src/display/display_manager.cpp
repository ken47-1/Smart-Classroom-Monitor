/* ==================== display_manager.cpp ==================== */
#include "display/display_manager.h"

/* =============== INCLUDES =============== */
/* ============ PROJECT ============ */
#include "config/HardwareConfig.h"

/* ============ CORE ============ */
#include <Arduino.h>

/* =============== PUBLIC API =============== */
/* ============ LIFECYCLE ============ */
DisplayManager::DisplayManager() : _initialized(false) {}

bool DisplayManager::begin() {
    _tft.init();
    _tft.setRotation(1);
    _tft.fillScreen(TFT_BLACK);
    _initialized = true;
    return _initialized;
}

void DisplayManager::clear() {
    if (_initialized) {
        _tft.fillScreen(TFT_BLACK);
    }
}

void DisplayManager::setBacklight(uint8_t brightness) {
    #ifdef TFT_BL
    pinMode(TFT_BL, OUTPUT);
    analogWrite(TFT_BL, brightness);
    #endif
}

TFT_eSPI& DisplayManager::getTFT() {
    return _tft;
}