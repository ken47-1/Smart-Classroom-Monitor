/* ==================== Config.h ==================== */
#pragma once

/* =============== INCLUDES =============== */
/* ============ THIRD-PARTY ============ */
#include <TFT_eSPI.h>

/* =============== TIMING =============== */
constexpr unsigned long SENSOR_READ_INTERVAL = 1000;  // ms between sensor reads

/* =============== COMFORT SCORE WEIGHTS =============== */
constexpr float WEIGHT_TEMP  = 0.40f;
constexpr float WEIGHT_HUMID = 0.40f;
constexpr float WEIGHT_LIGHT = 0.20f;

/* =============== COMFORT SCORE THRESHOLDS =============== */
// Temperature
constexpr float TEMP_IDEAL_LOW    = 23.0f;
constexpr float TEMP_IDEAL_HIGH   = 27.0f;
constexpr float TEMP_MAX_DIFF     = 10.0f;

// Humidity
constexpr float HUMID_IDEAL_LOW   = 40.0f;
constexpr float HUMID_IDEAL_HIGH  = 60.0f;
constexpr float HUMID_MAX_DIFF    = 40.0f;

// Light
constexpr float LIGHT_IDEAL_LOW   = 300.0f;
constexpr float LIGHT_IDEAL_HIGH  = 500.0f;
constexpr float LIGHT_MAX_DIFF    = 400.0f;

/* =============== COMFORT SCORE LEVELS =============== */
constexpr float LEVEL_EXCELLENT   = 90.0f;
constexpr float LEVEL_COMFORTABLE = 75.0f;
constexpr float LEVEL_FAIR        = 60.0f;
constexpr float LEVEL_POOR        = 40.0f;

/* =============== SENSOR FALLBACK VALUES =============== */
constexpr float FALLBACK_TEMP  = 25.0f;
constexpr float FALLBACK_HUMID = 50.0f;
constexpr float FALLBACK_PRESS = 1013.25f;
constexpr float FALLBACK_LUX   = 400.0f;

/* =============== DISPLAY SETTINGS =============== */
constexpr uint8_t BACKLIGHT_BRIGHTNESS = 160;  // 0-255

/* =============== UI COLORS =============== */
constexpr uint16_t COLOR_INFO  = 0x9D13;  /* 160, 160, 160 */
constexpr uint16_t COLOR_OK    = TFT_GREEN;
constexpr uint16_t COLOR_ERROR = TFT_RED;