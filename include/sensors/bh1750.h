/* ==================== bh1750.h ==================== */
#pragma once

/* =============== INCLUDES =============== */
/* ============ CORE ============ */
#include <Wire.h>

/* =============== TYPES =============== */
/* ============ STRUCTS ============ */
struct BH1750Data {
    float lux;
    bool valid;
};

/* =============== API =============== */
namespace BH1750Sensor {
    /* --------- Public Functions --------- */
    bool begin(uint8_t address = 0x23, TwoWire *wire = &Wire);
    BH1750Data read();
    bool is_initialized();
}