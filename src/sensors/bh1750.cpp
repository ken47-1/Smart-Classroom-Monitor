/* ==================== bh1750.cpp ==================== */
#include "sensors/bh1750.h"

/* =============== INCLUDES =============== */
/* ============ THIRD-PARTY ============ */
#include <BH1750.h>

/* =============== INTERNAL STATE =============== */
/* ============ STATIC VARS ============ */
static BH1750 lightMeter;
static bool initialized = false;
static TwoWire* _wire = &Wire;

/* =============== PUBLIC API =============== */
/* ============ LIFECYCLE ============ */
bool BH1750Sensor::begin(uint8_t address, TwoWire *wire) {
    _wire = wire;
    initialized = lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE, address, wire);
    return initialized;
}

bool BH1750Sensor::is_initialized() {
    return initialized;
}

/* ============ READ ============ */
BH1750Data BH1750Sensor::read() {
    BH1750Data data = {0.0f, false};
    
    if (!initialized) {
        return data;
    }
    
    if (lightMeter.measurementReady()) {
        data.lux = lightMeter.readLightLevel();
        data.valid = true;
    }
    
    return data;
}