/* ==================== bme280.h ==================== */
#pragma once

/* =============== INCLUDES =============== */
/* ============ CORE ============ */
#include <Wire.h>

/* =============== TYPES =============== */
/* ============ STRUCTS ============ */
struct BME280Data {
    float temperature;
    float humidity;
    float pressure;
    bool valid;
};

/* =============== API =============== */
namespace BME280 {
    /* --------- Public Functions --------- */
    bool begin(uint8_t address = 0x76, TwoWire *wire = &Wire);
    BME280Data read();
    bool is_initialized();
}