/* ==================== bme280.cpp ==================== */
#include "sensors/bme280.h"

/* =============== INCLUDES =============== */
/* ============ THIRD-PARTY ============ */
#include <Adafruit_BME280.h>

/* =============== INTERNAL STATE =============== */
/* ============ STATIC VARS ============ */
static Adafruit_BME280 bme;
static bool initialized = false;

/* =============== PUBLIC API =============== */
/* ============ LIFECYCLE ============ */
bool BME280::begin(uint8_t address, TwoWire *wire) {
    initialized = bme.begin(address, wire);
    return initialized;
}

bool BME280::is_initialized() {
    return initialized;
}

/* ============ READ ============ */
BME280Data BME280::read() {
    BME280Data data = {0.0f, 0.0f, 0.0f, false};
    
    if (!initialized) {
        return data;
    }
    
    data.temperature = bme.readTemperature();
    data.humidity = bme.readHumidity();
    data.pressure = bme.readPressure() / 100.0F;
    data.valid = true;
    
    return data;
}