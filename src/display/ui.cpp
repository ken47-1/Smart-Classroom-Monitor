/* ==================== ui.cpp ==================== */
#include "display/ui.h"

/* =============== INCLUDES =============== */
/* ============ PROJECT ============ */
#include "config/Config.h"
#include "config/HardwareConfig.h"
#include "config/DebugConfig.h"

/* ============ CORE ============ */
#include <Arduino.h>
#include <cstdio>
#include <algorithm>

/* =============== DEBUG FRAMEBUFFER DUMP =============== */
#if DEBUG_FRAMEBUFFER_DUMP
static constexpr uint16_t DUMP_MAGIC = 0xA55A;
static constexpr uint16_t DUMP_END   = 0x5AA5;
static constexpr uint16_t FRAME_END  = 0x55AA;
static int _frameCounter = 0;

static void dumpSpriteSlice(TFT_eSprite& sprite, uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
    uint32_t dataLen = width * height * 2;

    Serial.write((uint8_t*)&DUMP_MAGIC, 2);
    Serial.write((uint8_t*)&x, 2);
    Serial.write((uint8_t*)&y, 2);
    Serial.write((uint8_t*)&width, 2);
    Serial.write((uint8_t*)&height, 2);
    Serial.write((uint8_t*)&dataLen, 4);

    Serial.write((uint8_t*)sprite.getPointer(), dataLen);

    Serial.write((uint8_t*)&DUMP_END, 2);
}

static void dumpFrameEnd()
{
    Serial.write((uint8_t*)&FRAME_END, 2);
}

#endif

/* =============== INTERNAL STATE =============== */
/* ============ TYPOGRAPHY REGISTRY ============ */
const GFXfont* const UI::FONT_HEADER       = &FreeSansBold12pt7b;
const GFXfont* const UI::FONT_COMFORT_LBL  = &FreeSans12pt7b;
const GFXfont* const UI::FONT_COMFORT_VAL  = &FreeSansBold24pt7b;
const GFXfont* const UI::FONT_CARD_LBL     = &FreeSans9pt7b;
const GFXfont* const UI::FONT_CARD_VAL     = &FreeSansBold12pt7b;

/* ============ PALETTE ============ */
static constexpr uint16_t SPLASH_BORDER_COLOR = 0x9D13;  /* 160, 160, 160 */

static constexpr uint16_t COLOR_BG    = TFT_BLACK;
static constexpr uint16_t COLOR_DIV   = 0x9D13;  /* 160, 160, 160 */
static constexpr uint16_t COLOR_TEXT  = TFT_WHITE;

static constexpr uint16_t COLOR_TEMP  = 0xFB00;  /* 255,  96,   0 */
static constexpr uint16_t COLOR_HUMID = 0x051F;  /*   0, 160, 255 */
static constexpr uint16_t COLOR_PRESS = TFT_SKYBLUE;
static constexpr uint16_t COLOR_LIGHT = TFT_GOLD;

/* ============ HELPERS ============ */
static uint16_t getMetricColor(float score) {
    if (score >= 80.0f) return TFT_GREEN;
    if (score >= 60.0f) return TFT_YELLOW;
    if (score >= 40.0f) return 0xFB00;  /* 255,  96,   0 */
    return TFT_RED;
}

/* =============== PUBLIC API =============== */
/* ============ LIFECYCLE ============ */
UI::UI(DisplayManager& display)
    : _display(display)
    , _tft(nullptr)
    , _sliceSpr(&display.getTFT())
    , _headerH(0)
    , _comfortLabelH(0)
    , _comfortValueH(0)
    , _cardLabelH(0)
    , _cardValueH(0)
{
    _tft = &_display.getTFT();
}

void UI::begin() {
    if (!_tft) {
        Serial.println("TFT FAILED!");
        return;
    }

    _tft->fillScreen(COLOR_BG);
    _sliceSpr.setColorDepth(16);
    _sliceSpr.createSprite(SCREEN_WIDTH, BUF_HEIGHT);

    /* --- Cache Font Metrics --- */
    _sliceSpr.setFreeFont(FONT_HEADER);
    _headerH = _sliceSpr.fontHeight(1);

    _sliceSpr.setFreeFont(FONT_COMFORT_LBL);
    _comfortLabelH = _sliceSpr.fontHeight(1);

    _sliceSpr.setFreeFont(FONT_COMFORT_VAL);
    _comfortValueH = _sliceSpr.fontHeight(1);

    _sliceSpr.setFreeFont(FONT_CARD_LBL);
    _cardLabelH = _sliceSpr.fontHeight(1);

    _sliceSpr.setFreeFont(FONT_CARD_VAL);
    _cardValueH = _sliceSpr.fontHeight(1);

    /* --- Cache Layout Values --- */
    _headerTop = (BORDER_PAD + BORDER_THICKNESS) + MARGIN;
    _headerAreaH = (_headerH - 11) + MARGIN;
    _dividerY = _headerTop + _headerAreaH;
}

void UI::drawSplash(const char* subtitle, uint16_t color) {
    if (!_tft) return;

    _tft->fillScreen(COLOR_BG);

    // Draw border
    for (int i = 0; i < SPLASH_THICKNESS; i++) {
        _tft->drawRect(SPLASH_PAD + i, SPLASH_PAD + i,
                       SCREEN_WIDTH - (SPLASH_PAD * 2) - (i * 2),
                       SCREEN_HEIGHT - (SPLASH_PAD * 2) - (i * 2),
                       SPLASH_BORDER_COLOR);
    }

    // Draw title
    _tft->setTextColor(COLOR_TEXT, COLOR_BG);
    _tft->setFreeFont(FONT_HEADER);
    _tft->setTextDatum(MC_DATUM);
    _tft->drawString("Smart Classroom Monitor", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - 8);

    // Draw initial subtitle
    if (subtitle && strlen(subtitle) > 0) {
        _tft->setFreeFont(&FreeSans9pt7b);
        _tft->setTextColor(color, COLOR_BG);
        _tft->drawString(subtitle, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 28);
    }

    _tft->setTextDatum(TL_DATUM);
}

void UI::updateSplashSubtitle(const char* subtitle, uint16_t color) {
    if (!_tft) return;

    _tft->setFreeFont(&FreeSans9pt7b);
    int textHeight = _tft->fontHeight();
    int y = SCREEN_HEIGHT / 2 + 28 - (textHeight / 2);

    _tft->fillRect(BORDER_PAD + BORDER_THICKNESS, y, SCREEN_WIDTH - ((BORDER_PAD + BORDER_THICKNESS) * 2), textHeight, COLOR_BG);

    _tft->setTextColor(color, COLOR_BG);
    _tft->setTextDatum(MC_DATUM);
    _tft->drawString(subtitle, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 28);
    _tft->setTextDatum(TL_DATUM);
}

void UI::update(float temp, float humidity, float pressure, float lux, float comfort, 
                float tempScore, float humidScore, float lightScore) {
    if (!_tft) return;
    _renderFrame(temp, humidity, pressure, lux, comfort, tempScore, humidScore, lightScore);
}

/* =============== SLICED RENDERER ENGINE =============== */
void UI::_renderFrame(float temp, float humidity, float pressure, float lux, float comfort,
                      float tempScore, float humidScore, float lightScore) {
    for (int sliceY = 0; sliceY < SCREEN_HEIGHT; sliceY += BUF_HEIGHT) {
        int sliceH = std::min(BUF_HEIGHT, SCREEN_HEIGHT - sliceY);

        _sliceSpr.fillSprite(COLOR_BG);
        _drawScene(sliceY, sliceH, temp, humidity, pressure, lux, comfort,
                   tempScore, humidScore, lightScore);

#if DEBUG_FRAMEBUFFER_DUMP
        if (++_frameCounter % DEBUG_DUMP_EVERY_N == 0) {
            dumpSpriteSlice(_sliceSpr, 0, sliceY, SCREEN_WIDTH, sliceH);
        }
#endif

        _sliceSpr.pushSprite(0, sliceY, 0, 0, SCREEN_WIDTH, sliceH);
    }

#if DEBUG_FRAMEBUFFER_DUMP
    dumpFrameEnd();
#endif
}

void UI::_drawScene(int sliceY, int sliceH, float temp, float humidity, float pressure, float lux,
                    float comfort, float tempScore, float humidScore, float lightScore) {
    ComfortLevel level = ComfortScore::get_level(comfort);
    _drawBorder(sliceY, sliceH, level.color);
    _drawHeader(sliceY, sliceH);
    _drawComfortScore(sliceY, sliceH, comfort, level.color);
    _drawMetrics(sliceY, sliceH, temp, humidity, pressure, lux, tempScore, humidScore, lightScore);
}

/* =============== PRIVATE DRAWING METHODS =============== */
void UI::_drawBorder(int sliceY, int sliceH, uint16_t color) {
    if (!_isIntersecting(0, SCREEN_HEIGHT, sliceY, sliceH)) return;

    int bx = BORDER_PAD;
    int bw = SCREEN_WIDTH - (BORDER_PAD * 2);

    for (int i = 0; i < BORDER_THICKNESS; i++) {
        int currentY = BORDER_PAD + i;
        if (currentY >= sliceY && currentY < sliceY + sliceH) {
            _sliceSpr.drawFastHLine(bx + i, currentY - sliceY, bw - (i * 2), color);
        }

        currentY = SCREEN_HEIGHT - BORDER_PAD - 1 - i;
        if (currentY >= sliceY && currentY < sliceY + sliceH) {
            _sliceSpr.drawFastHLine(bx + i, currentY - sliceY, bw - (i * 2), color);
        }

        for (int sy = 0; sy < sliceH; sy++) {
            int globalY = sliceY + sy;
            if (globalY >= (BORDER_PAD + i) && globalY < (SCREEN_HEIGHT - BORDER_PAD - i)) {
                _sliceSpr.drawPixel(bx + i, sy, color);
                _sliceSpr.drawPixel(bx + bw - 1 - i, sy, color);
            }
        }
    }
}

void UI::_drawHeader(int sliceY, int sliceH) {
    if (!_isIntersecting(_headerTop, _headerAreaH + 2, sliceY, sliceH)) return;

    _sliceSpr.setTextColor(COLOR_TEXT, COLOR_BG);
    _sliceSpr.setFreeFont(FONT_HEADER);
    _sliceSpr.setTextDatum(TC_DATUM);
    _sliceSpr.drawString("Smart Classroom Monitor", SCREEN_WIDTH / 2, _headerTop);

    if (_dividerY >= sliceY && _dividerY < sliceY + sliceH) {
        _sliceSpr.drawFastHLine(BORDER_PAD + BORDER_THICKNESS, _dividerY - sliceY, SCREEN_WIDTH - ((BORDER_PAD + BORDER_THICKNESS) * 2), COLOR_DIV);
    }
}

void UI::_drawComfortScore(int sliceY, int sliceH, float score, uint16_t color) {
    int labelY = (_dividerY + 1) + 12 - sliceY;
    int valueY = (labelY + _comfortLabelH - 13) + MARGIN - sliceY;

    if (!_isIntersecting(_dividerY + 8, 40, sliceY, sliceH)) return;

    _sliceSpr.setTextColor(COLOR_TEXT, COLOR_BG);
    _sliceSpr.setFreeFont(FONT_COMFORT_LBL);
    _sliceSpr.setTextDatum(TC_DATUM);
    _sliceSpr.drawString("Comfort Score", SCREEN_WIDTH / 2, labelY);

    char buf[8];
    snprintf(buf, sizeof(buf), "%.0f%%", score);
    _sliceSpr.setTextColor(color, COLOR_BG);
    _sliceSpr.setFreeFont(FONT_COMFORT_VAL);
    _sliceSpr.setTextDatum(TC_DATUM);
    _sliceSpr.drawString(buf, SCREEN_WIDTH / 2, valueY);
}

void UI::_drawCard(int sliceY, int sliceH, int x, int y, int w, int h,
                   const char* label, const char* value, const char* unit,
                   uint16_t valueColor, uint16_t borderColor,
                   uint16_t borderThickness) {
    if (!_isIntersecting(y, h, sliceY, sliceH)) return;

    int localY = y - sliceY;

    // Draw rounded border
    _sliceSpr.fillRoundRect(x, localY, w, h, 4, borderColor);

    // Draw inner background
    _sliceSpr.fillRoundRect(x + borderThickness, localY + borderThickness,
                            w - (borderThickness * 2), h - (borderThickness * 2), 4, COLOR_BG);

    int labelX = x + 6;
    int valueX = x + 12;
    int labelY = localY - 3;
    int valueY = (localY + h - 2) - 6;

    _sliceSpr.setTextColor(COLOR_TEXT, COLOR_BG);
    _sliceSpr.setFreeFont(FONT_CARD_LBL);
    _sliceSpr.setTextDatum(ML_DATUM);
    _sliceSpr.drawString(label, labelX, labelY);

    _sliceSpr.setTextDatum(BL_DATUM);
    _sliceSpr.setCursor(valueX, valueY);

    _sliceSpr.setTextColor(valueColor, COLOR_BG);
    _sliceSpr.setFreeFont(FONT_CARD_VAL);
    _sliceSpr.print(value);

    _sliceSpr.setTextColor(COLOR_TEXT, COLOR_BG);
    _sliceSpr.setFreeFont(FONT_CARD_LBL);
    _sliceSpr.print(" ");
    _sliceSpr.print(unit);
}

void UI::_drawMetrics(int sliceY, int sliceH, float temp, float humid, float press, float lux,
                      float tempScore, float humidScore, float lightScore) {
    char buf[16];

    // Anchor columns
    int col1X = BORDER_PAD + BORDER_THICKNESS + MARGIN;
    int col2X = SCREEN_WIDTH - BORDER_PAD - BORDER_THICKNESS - MARGIN - CARD_WIDTH;

    // Anchor rows
    int row2Y = SCREEN_HEIGHT - BORDER_PAD - BORDER_THICKNESS - MARGIN - CARD_HEIGHT;
    int row1Y = row2Y - CARD_VERTICAL_GAP - CARD_HEIGHT;

    if (_isIntersecting(row1Y, CARD_HEIGHT, sliceY, sliceH)) {
        snprintf(buf, sizeof(buf), "%.1f", temp);
        _drawCard(sliceY, sliceH, col1X, row1Y, CARD_WIDTH, CARD_HEIGHT,
                  "[Temperature]", buf, "C", COLOR_TEMP, getMetricColor(tempScore));

        snprintf(buf, sizeof(buf), "%.0f", humid);
        _drawCard(sliceY, sliceH, col2X, row1Y, CARD_WIDTH, CARD_HEIGHT,
                  "[Humidity]", buf, "%", COLOR_HUMID, getMetricColor(humidScore));
    }

    if (_isIntersecting(row2Y, CARD_HEIGHT, sliceY, sliceH)) {
        snprintf(buf, sizeof(buf), "%.0f", press);
        _drawCard(sliceY, sliceH, col1X, row2Y, CARD_WIDTH, CARD_HEIGHT,
                  "[Pressure]", buf, "hPa", COLOR_PRESS, 0x051F);  /*   0, 160, 255 */

        snprintf(buf, sizeof(buf), "%.0f", lux);
        _drawCard(sliceY, sliceH, col2X, row2Y, CARD_WIDTH, CARD_HEIGHT,
                  "[Light]", buf, "lx", COLOR_LIGHT, getMetricColor(lightScore));
    }
}

/* =============== INLINE UTILITY =============== */
inline bool UI::_isIntersecting(int elemY, int elemH, int sliceY, int sliceH) {
    return (elemY < (sliceY + sliceH)) && ((elemY + elemH) > sliceY);
}