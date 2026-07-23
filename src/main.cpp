/* ==================== main.cpp ==================== */
#include <Arduino.h>
#include <Wire.h>

/* =============== INCLUDES =============== */
/* ============ PROJECT ============ */
#include "config/HardwareConfig.h"
#include "config/Config.h"
#include "display/display_manager.h"
#include "display/ui.h"
#include "sensors/bme280.h"
#include "sensors/bh1750.h"
#include "comfort_score.h"

/* =============== INTERNAL STATE =============== */
/* ============ STATIC VARS ============ */
static unsigned long lastReadTime = 0;

static bool bme280Available = false;
static bool bh1750Available = false;

static float lastValidTemp = FALLBACK_TEMP;
static float lastValidHumidity = FALLBACK_HUMID;
static float lastValidPressure = FALLBACK_PRESS;
static float lastValidLux = FALLBACK_LUX;

/* ============ I2C INSTANCES ============ */
static TwoWire i2cBH1750 = TwoWire(0);
static TwoWire i2cBME280 = TwoWire(1);

/* =============== DISPLAY =============== */
static DisplayManager display;
static UI ui(display);

/* =============== INTERNAL HELPERS =============== */
/* ============ SENSOR READ ============ */
static bool read_bme280(float &temp, float &humidity, float &pressure) {
    if (!bme280Available) {
        temp = lastValidTemp;
        humidity = lastValidHumidity;
        pressure = lastValidPressure;
        return false;
    }
    
    BME280Data data = BME280::read();
    if (!data.valid) {
        temp = lastValidTemp;
        humidity = lastValidHumidity;
        pressure = lastValidPressure;
        return false;
    }
    
    lastValidTemp = data.temperature;
    lastValidHumidity = data.humidity;
    lastValidPressure = data.pressure;
    temp = data.temperature;
    humidity = data.humidity;
    pressure = data.pressure;
    return true;
}

static bool read_bh1750(float &lux) {
    if (!bh1750Available) {
        lux = lastValidLux;
        return false;
    }
    
    BH1750Data data = BH1750Sensor::read();
    if (!data.valid) {
        lux = lastValidLux;
        return false;
    }
    
    lastValidLux = data.lux;
    lux = data.lux;
    return true;
}

/* ============ READ AND UPDATE DISPLAY ============ */
static void read_and_update() {
    float temp, humidity, pressure, lux;
    
    read_bme280(temp, humidity, pressure);
    read_bh1750(lux);
    
    float comfort = ComfortScore::calculate(temp, humidity, lux);
    float tempScore, humidScore, lightScore;
    ComfortScore::get_individual_scores(temp, humidity, lux, tempScore, humidScore, lightScore);
    ui.update(temp, humidity, pressure, lux, comfort, tempScore, humidScore, lightScore);
}

/* =============== SETUP =============== */
void setup() {
    #if DEBUG_FRAMEBUFFER_DUMP
        Serial.begin(921600);
    #else
        Serial.begin(115200);
    #endif

    /* --- Display --- */
    display.begin();
    display.setBacklight(BACKLIGHT_BRIGHTNESS);
    ui.begin();
    
    ui.drawSplash("Booting...", COLOR_INFO);
    delay(250);
    
    /* --- I2C Buses --- */
    ui.updateSplashSubtitle("Initializing I2C...", COLOR_INFO);
    i2cBH1750.begin(BH1750_SDA, BH1750_SCL);
    i2cBH1750.setClock(100000);
    i2cBME280.begin(BME280_SDA, BME280_SCL);
    i2cBME280.setClock(100000);
    delay(250);
    
    /* --- BME280 --- */
    ui.updateSplashSubtitle("Initializing BME280...", COLOR_INFO);
    bme280Available = BME280::begin(BME280_ADDR, &i2cBME280);
    delay(250);
    if (bme280Available) {
        ui.updateSplashSubtitle("BME280 OK", COLOR_OK);
        delay(250);
    } else {
        ui.updateSplashSubtitle("! BME280 not found !", COLOR_ERROR);
        delay(750);
    }
    
    /* --- BH1750 --- */
    ui.updateSplashSubtitle("Initializing BH1750...", COLOR_INFO);
    bh1750Available = BH1750Sensor::begin(BH1750_ADDR, &i2cBH1750);
    delay(250);
    if (bh1750Available) {
        ui.updateSplashSubtitle("BH1750 OK", COLOR_OK);
        delay(250);
    } else {
        ui.updateSplashSubtitle("! BH1750 not found !", COLOR_ERROR);
        delay(750);
    }

    /* --- Final Status --- */
    if (bme280Available && bh1750Available) {
        ui.updateSplashSubtitle("All sensors OK", COLOR_OK);
        delay(500);
    } else if (!bme280Available && !bh1750Available) {
        ui.updateSplashSubtitle("! BME280 & BH1750 FAILED !", COLOR_ERROR);
        delay(1000);
    } else if (!bme280Available) {
        ui.updateSplashSubtitle("! BME280 FAILED !", COLOR_ERROR);
        delay(1000);
    } else if (!bh1750Available) {
        ui.updateSplashSubtitle("! BH1750 FAILED !", COLOR_ERROR);
        delay(1000);
    }
    
    read_and_update();
}

/* =============== LOOP =============== */
void loop() {
    if (millis() - lastReadTime >= SENSOR_READ_INTERVAL) {
        read_and_update();
        lastReadTime = millis();
    }
    vTaskDelay(1);
}
