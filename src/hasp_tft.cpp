#include "ArduinoJson.h"
#include "ArduinoLog.h"
#include "TFT_eSPI.h"

#include "hasp_tft.h"

#if defined(ARDUINO_ARCH_ESP8266)
ADC_MODE(ADC_VCC); // tftShowConfig measures the voltage on the pin
#endif

int8_t getPinName(int8_t pin);

void tftSetup(TFT_eSPI & tft)
{
    tftShowConfig(tft);
}

void tftLoop()
{
    // Nothing to do here
}

void tftStop()
{}

String tftDriverName()
{
#if defined(ILI9341_DRIVER)
    return F("ILI9341");
#elif defined(ST7735_DRIVER)
    return F("ST7735");
#elif defined(ILI9163_DRIVER)
    return F("ILI9163");
#elif defined(S6D02A1_DRIVER)
    return F("S6D02A1");
#elif defined(ST7796_DRIVER)
    return F("ST7796");
#elif defined(ILI9486_DRIVER)
    return F("ILI9486");
#elif defined(ILI9481_DRIVER)
    return F("ILI9481");
#elif defined(ILI9488_DRIVER)
    return F("ILI9488");
#elif defined(HX8357D_DRIVER)
    return F("HX8357D");
#elif defined(EPD_DRIVER)
    return F("EPD");
#elif defined(ST7789_DRIVER)
    return F("ST7789");
#elif defined(R61581_DRIVER)
    return F("R61581");
#elif defined(ST7789_2_DRIVER)
    return F("ST7789_2");
#elif defined(RM68140_DRIVER)
    return F("RM68140");
#endif
    return F("Unknown");
}

void tftOffsetInfo(uint8_t pin, uint8_t x_offset, uint8_t y_offset)
{
    if(x_offset != 0) {
        Log.verbose(F("TFT: R%u x offset = %i"), pin, x_offset);
    }
    if(y_offset != 0) {
        Log.verbose(F("TFT: R%u y offset = %i"), pin, y_offset);
    }
}

void tftPinInfo(const __FlashStringHelper * pinfunction, int8_t pin)
{
    if(pin != -1) {
        Log.verbose(F("TFT: %s = D%i (GPIO %i)"), pinfunction, getPinName(pin), pin);
    }
}

void tftShowConfig(TFT_eSPI & tft)
{
    setup_t tftSetup;
    tft.getSetup(tftSetup);

    Log.verbose(F("TFT: TFT_eSPI ver = %s"), tftSetup.version.c_str());
    Log.verbose(F("TFT: Processor    = ESP%x"), tftSetup.esp);
    Log.verbose(F("TFT: Frequency    = %i MHz"), ESP.getCpuFreqMHz());

#if defined(ARDUINO_ARCH_ESP8266)
    Log.verbose(F("TFT: Voltage      = %2.2f V"), ESP.getVcc() / 918.0); // 918 empirically determined
#endif
    Log.verbose(F("TFT: Transactions = %s"), (tftSetup.trans == 1) ? PSTR("Yes") : PSTR("No"));
    Log.verbose(F("TFT: Interface    = %s"), (tftSetup.serial == 1) ? PSTR("SPI") : PSTR("Parallel"));
#if defined(ARDUINO_ARCH_ESP8266)
    Log.verbose(F("TFT: SPI overlap  = %s"), (tftSetup.overlap == 1) ? PSTR("Yes") : PSTR("No"));
#endif
    if(tftSetup.tft_driver != 0xE9D) // For ePaper displays the size is defined in the sketch
    {
        Log.verbose(F("TFT: Display driver = %s"), tftDriverName().c_str()); // tftSetup.tft_driver);
        Log.verbose(F("TFT: Display width  = %i"), tftSetup.tft_width);
        Log.verbose(F("TFT: Display height = %i"), tftSetup.tft_height);
    } else if(tftSetup.tft_driver == 0xE9D)
        Log.verbose(F("Display driver = ePaper"));

    // Offsets, not all used yet
    tftOffsetInfo(0, tftSetup.r0_x_offset, tftSetup.r0_y_offset);
    tftOffsetInfo(1, tftSetup.r1_x_offset, tftSetup.r1_y_offset);
    tftOffsetInfo(2, tftSetup.r2_x_offset, tftSetup.r2_y_offset);
    tftOffsetInfo(3, tftSetup.r3_x_offset, tftSetup.r3_y_offset);
    /* replaced by tftOffsetInfo
    //    if(tftSetup.r1_x_offset != 0) Serial.printf("R1 x offset = %i \n", tftSetup.r1_x_offset);
    //    if(tftSetup.r1_y_offset != 0) Serial.printf("R1 y offset = %i \n", tftSetup.r1_y_offset);
    //    if(tftSetup.r2_x_offset != 0) Serial.printf("R2 x offset = %i \n", tftSetup.r2_x_offset);
    //    if(tftSetup.r2_y_offset != 0) Serial.printf("R2 y offset = %i \n", tftSetup.r2_y_offset);
    //    if(tftSetup.r3_x_offset != 0) Serial.printf("R3 x offset = %i \n", tftSetup.r3_x_offset);
    //    if(tftSetup.r3_y_offset != 0) Serial.printf("R3 y offset = %i \n", tftSetup.r3_y_offset);
    */

    tftPinInfo(F("MOSI   "), tftSetup.pin_tft_mosi);
    tftPinInfo(F("MISO   "), tftSetup.pin_tft_miso);
    tftPinInfo(F("SCLK   "), tftSetup.pin_tft_clk);

#if defined(ARDUINO_ARCH_ESP8266)
    if(tftSetup.overlap == true) {
        Log.verbose(F("Overlap selected, following pins MUST be used:"));

        Log.verbose(F("MOSI     = SD1 (GPIO 8)"));
        Log.verbose(F("MISO     = SD0 (GPIO 7)"));
        Log.verbose(F("SCK      = CLK (GPIO 6)"));
        Log.verbose(F("TFT_CS   = D3  (GPIO 0)"));

        Log.verbose(F("TFT_DC and TFT_RST pins can be tftSetup defined"));
    }
#endif

    tftPinInfo(F("TFT_CS "), tftSetup.pin_tft_cs);
    tftPinInfo(F("TFT_DC "), tftSetup.pin_tft_dc);
    tftPinInfo(F("TFT_RST"), tftSetup.pin_tft_rst);

    tftPinInfo(F("TOUCH_RST"), tftSetup.pin_tch_cs);

    tftPinInfo(F("TFT_WR "), tftSetup.pin_tft_wr);
    tftPinInfo(F("TFT_RD "), tftSetup.pin_tft_rd);

    tftPinInfo(F("TFT_D0 "), tftSetup.pin_tft_d0);
    tftPinInfo(F("TFT_D1 "), tftSetup.pin_tft_d1);
    tftPinInfo(F("TFT_D2 "), tftSetup.pin_tft_d2);
    tftPinInfo(F("TFT_D3 "), tftSetup.pin_tft_d3);
    tftPinInfo(F("TFT_D4 "), tftSetup.pin_tft_d4);
    tftPinInfo(F("TFT_D5 "), tftSetup.pin_tft_d5);
    tftPinInfo(F("TFT_D6 "), tftSetup.pin_tft_d6);
    tftPinInfo(F("TFT_D7 "), tftSetup.pin_tft_d7);

    /*
        uint16_t fonts = tft.fontsLoaded();
        if(fonts & (1 << 1)) Log.verbose(F("Font GLCD   loaded\n"));
        if(fonts & (1 << 2)) Log.verbose(F("Font 2      loaded\n"));
        if(fonts & (1 << 4)) Log.verbose(F("Font 4      loaded\n"));
        if(fonts & (1 << 6)) Log.verbose(F("Font 6      loaded\n"));
        if(fonts & (1 << 7)) Log.verbose(F("Font 7      loaded\n"));
        if(fonts & (1 << 9))
            Log.verbose(F("Font 8N     loaded\n"));
        else if(fonts & (1 << 8))
            Log.verbose(F("Font 8      loaded\n"));
        if(fonts & (1 << 15)) Log.verbose(F("Smooth font enabled\n"));
    */

    if(tftSetup.serial == 1) {
        Log.verbose(F("TFT: Display SPI frequency = %4.1f MHz"), (float)tftSetup.tft_spi_freq / 10.0f);
    }
    if(tftSetup.pin_tch_cs != -1) {
        Log.verbose(F("TFT: Touch SPI frequency   = %4.1f MHz"), (float)tftSetup.tch_spi_freq / 10.0f);
    }
}

// Get pin name for ESP8266
int8_t getPinName(int8_t pin)
{
// For ESP32 pin labels on boards use the GPIO number
#if defined(ARDUINO_ARCH_ESP32)
    return pin;
#endif

    // For ESP8266 the pin labels are not the same as the GPIO number
    // These are for the NodeMCU pin definitions:
    //        GPIO       Dxx
    if(pin == 16) return 0;
    if(pin == 5) return 1;
    if(pin == 4) return 2;
    if(pin == 0) return 3;
    if(pin == 2) return 4;
    if(pin == 14) return 5;
    if(pin == 12) return 6;
    if(pin == 13) return 7;
    if(pin == 15) return 8;
    if(pin == 3) return 9;
    if(pin == 1) return 10;
    if(pin == 9) return 11;
    if(pin == 10) return 12;

    return -1; // Invalid pin
}
