/* ==================== ui.h ==================== */
#pragma once

/* =============== INCLUDES =============== */
/* ============ PROJECT ============ */
#include "config/HardwareConfig.h"
#include "display_manager.h"
#include "comfort_score.h"

/* ============ THIRD-PARTY ============ */
#include <TFT_eSPI.h>

/* =============== API =============== */
class UI {
public:
    UI(DisplayManager& display);
    void begin();
    void update(float temp, float humidity, float pressure, float lux, float comfort, float tempScore, float humidScore, float lightScore);
    void drawSplash(const char* subtitle, uint16_t color);
    void updateSplashSubtitle(const char* subtitle, uint16_t color);

private:
    /* --------- Sliced Rendering Core --------- */
    void _renderFrame(float temp, float humidity, float pressure, float lux, float comfort,
                    float tempScore, float humidScore, float lightScore);
    void _drawScene(int sliceY, int sliceH, float temp, float humidity, float pressure, float lux,
                    float comfort, float tempScore, float humidScore, float lightScore);

    /* --------- Drawing Primitives --------- */
    void _drawBorder(int sliceY, int sliceH, uint16_t color);
    void _drawHeader(int sliceY, int sliceH);
    void _drawComfortScore(int sliceY, int sliceH, float score, uint16_t color);
    void _drawMetrics(int sliceY, int sliceH, float temp, float humid, float press, float lux,
                    float tempScore, float humidScore, float lightScore);
    void _drawCard(int sliceY, int sliceH, int x, int y, int w, int h,
                const char* label, const char* value, const char* unit,
                uint16_t valueColor, uint16_t borderColor,
                uint16_t borderThickness = 3);

    /* --------- Helper Utility --------- */
    inline bool _isIntersecting(int elemY, int elemH, int sliceY, int sliceH);

    /* --------- Splash Layout --------- */
    static constexpr int SPLASH_PAD       = 0;
    static constexpr int SPLASH_THICKNESS = 5;

    /* --------- Layout Constants --------- */
    static constexpr int MARGIN            = 7;
    static constexpr int BORDER_PAD        = 0;
    static constexpr int BORDER_THICKNESS  = 5;
    static constexpr int INNER_GAP         = 4;
    static constexpr int CARD_WIDTH        = 142;     // 148px - 12px
    static constexpr int CARD_HEIGHT       = 39;      // 35px
    static constexpr int CARD_VERTICAL_GAP = 10 + 8;  // 0px + text offset (0px is not recommended)
    static constexpr int BUF_HEIGHT        = SCREEN_HEIGHT / 2;

    /* --------- Typography Registry --------- */
    static const GFXfont* const FONT_HEADER;
    static const GFXfont* const FONT_COMFORT_LBL;
    static const GFXfont* const FONT_COMFORT_VAL;
    static const GFXfont* const FONT_CARD_LBL;
    static const GFXfont* const FONT_CARD_VAL;

	/* --------- Cached Metrics --------- */
	int _headerH;
	int _comfortLabelH;
	int _comfortValueH;
	int _cardLabelH;
	int _cardValueH;

	/* --------- Cached Layout --------- */
	int _headerTop;
	int _headerAreaH;
	int _dividerY;

    /* --------- State --------- */
    DisplayManager& _display;
    TFT_eSPI* _tft;
    TFT_eSprite _sliceSpr;
};