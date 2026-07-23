/* ==================== comfort_score.cpp ==================== */
#include "comfort_score.h"
#include "config/Config.h"

/* =============== INCLUDES =============== */
/* ============ CORE ============ */
#include <math.h>

/* =============== INTERNAL HELPERS =============== */
/* ============ SCORING ============ */

static float temperature_score(float temp) {
    float score = 100.0f;
    
    if (temp < TEMP_IDEAL_LOW) {
        float diff = TEMP_IDEAL_LOW - temp;
        score = 100.0f * (1.0f - diff / TEMP_MAX_DIFF);
    } else if (temp > TEMP_IDEAL_HIGH) {
        float diff = temp - TEMP_IDEAL_HIGH;
        score = 100.0f * (1.0f - diff / TEMP_MAX_DIFF);
    }
    
    return fmaxf(0.0f, fminf(100.0f, score));
}

static float humidity_score(float humidity) {
    float score = 100.0f;
    
    if (humidity < HUMID_IDEAL_LOW) {
        float diff = HUMID_IDEAL_LOW - humidity;
        score = 100.0f * (1.0f - diff / HUMID_MAX_DIFF);
    } else if (humidity > HUMID_IDEAL_HIGH) {
        float diff = humidity - HUMID_IDEAL_HIGH;
        score = 100.0f * (1.0f - diff / HUMID_MAX_DIFF);
    }
    
    return fmaxf(0.0f, fminf(100.0f, score));
}

static float light_score(float lux) {
    float score = 100.0f;
    
    if (lux < LIGHT_IDEAL_LOW) {
        float diff = LIGHT_IDEAL_LOW - lux;
        score = 100.0f * (1.0f - diff / LIGHT_MAX_DIFF);
    } else if (lux > LIGHT_IDEAL_HIGH) {
        float diff = lux - LIGHT_IDEAL_HIGH;
        score = 100.0f * (1.0f - diff / LIGHT_MAX_DIFF);
    }
    
    return fmaxf(0.0f, fminf(100.0f, score));
}

/* =============== PUBLIC API =============== */
/* ============ CALCULATE ============ */
float ComfortScore::calculate(float temperature, float humidity, float lux) {
    float t = temperature_score(temperature);
    float h = humidity_score(humidity);
    float l = light_score(lux);
    
    float weighted_score = (t * WEIGHT_TEMP) + (h * WEIGHT_HUMID) + (l * WEIGHT_LIGHT);
    
    return fmaxf(0.0f, fminf(100.0f, weighted_score));
}

/* ============ GET INDIVIDUAL SCORES ============ */
void ComfortScore::get_individual_scores(float temp, float humid, float lux,
                                         float &tempScore, float &humidScore, float &lightScore) {
    tempScore = temperature_score(temp);
    humidScore = humidity_score(humid);
    lightScore = light_score(lux);
}

/* ============ GET LEVEL ============ */
ComfortLevel ComfortScore::get_level(float score) {
    if (score >= LEVEL_EXCELLENT) {
        return {ComfortStatus::EXCELLENT, "Excellent", TFT_GREEN};
    } else if (score >= LEVEL_COMFORTABLE) {
        return {ComfortStatus::COMFORTABLE, "Comfortable", TFT_GREEN};
    } else if (score >= LEVEL_FAIR) {
        return {ComfortStatus::FAIR, "Fair", TFT_YELLOW};
    } else if (score >= LEVEL_POOR) {
        return {ComfortStatus::POOR, "Poor", 0xFB00};  /* 255,  96,   0 */
    } else {
        return {ComfortStatus::UNCOMFORTABLE, "Uncomfortable", TFT_RED};
    }
}