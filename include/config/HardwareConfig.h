/* ==================== HardwareConfig.h ==================== */
#pragma once

/* =============== I2C BUS =============== */
/* --- BME280 --- */
#define BME280_SCL  22
#define BME280_SDA  21

/* --- BH1750 --- */
#define BH1750_SCL  17
#define BH1750_SDA  16

/* =============== SENSOR ADDRESSES =============== */
#define BME280_ADDR  0x76
#define BH1750_ADDR  0x23

/* =============== DISPLAY =============== */
/* 
   FOR REFERENCE ONLY.
   CHANGE IN User_Setup.h
*/
#define TFT_MISO 19
#define TFT_MOSI 23
#define TFT_SCLK 18
#define TFT_CS   15
#define TFT_DC    2
#define TFT_RST   4
#define TFT_BL    5

#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 240