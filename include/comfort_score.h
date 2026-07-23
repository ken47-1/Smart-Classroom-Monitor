/* ==================== comfort_score.h ==================== */
#pragma once

/* =============== INCLUDES =============== */
/* ============ CORE ============ */
#include <Arduino.h>
#include <TFT_eSPI.h>  // Needed for color constants

/* =============== TYPES =============== */
/* ============ ENUMS ============ */
enum class ComfortStatus {
    EXCELLENT,
    COMFORTABLE,
    FAIR,
    POOR,
    UNCOMFORTABLE
};

/* ============ STRUCTS ============ */
struct ComfortLevel {
    ComfortStatus status;
    const char* label;
    uint16_t color;
};

/* =============== API =============== */
namespace ComfortScore {
    /* --------- Public Functions --------- */
    float calculate(float temperature, float humidity, float lux);
    void get_individual_scores(float temp, float humid, float lux,
                           float &tempScore, float &humidScore, float &lightScore);
    ComfortLevel get_level(float score);
}
