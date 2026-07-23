/* ==================== display_manager.h ==================== */
#pragma once

/* =============== INCLUDES =============== */
/* ============ THIRD-PARTY ============ */
#include <TFT_eSPI.h>

/* =============== API =============== */
class DisplayManager {
public:
    DisplayManager();
    bool begin();
    void clear();
    void setBacklight(uint8_t brightness);
    TFT_eSPI& getTFT();

private:
    TFT_eSPI _tft;
    bool _initialized;
};