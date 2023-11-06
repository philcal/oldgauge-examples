/*
 * Initially based on https://github.com/JakubAndrysek/OG_T_Display, TTGO_Hello_world project.
 *
 * For sprites, see https://github.com/Bodmer/TFT_eSPI/blob/master/examples/Sprite/Animated_dial/Animated_dial.ino
 *
 * How to use TFT_eSPI library and ESP32 (PROGRAMING TUTORIAL PART 1.)
 * https://www.youtube.com/watch?v=WFVjsxFMbSM
 *
 * How to use TFT_eSPI library and ESP32 (PROGRAMING TUTORIAL PART 2.)
 * https://www.youtube.com/watch?v=AyumJUAFdZ0
 *
 * How to program TTGO T-Display - PART3 (Images and Custom Fonts)
 * https://www.youtube.com/watch?v=R-qFKemDFyM
 *
 * RGB Color Picker
 * https://rgbcolorpicker.com/565
 *
 * ImageConverter (UTFT)
 * http://www.rinkydinkelectronics.com/t_imageconverter565.php
 *
 * Font Converter
 * https://oleddisplay.squix.ch/
 */

#include <Arduino.h>

#include <TFT_eSPI.h>
#include <SPI.h>
#include "WiFi.h"
#include <Wire.h>
#include <Button2.h>
#include "esp_adc_cal.h"
#include "dial.h"

#include <Oldgauge.h>
#include "esp-30pin.h"

#ifndef TFT_DISPOFF
#define TFT_DISPOFF 0x28
#endif

#ifndef TFT_SLPIN
#define TFT_SLPIN 0x10
#endif

// Font attached to this sketch
// #include "./NotoSansBold36.h"
// #define AA_FONT_LARGE NotoSansBold36

#define ADC_EN 14
#define ADC_PIN 34
#define BUTTON_1 35
#define BUTTON_2 0

TFT_eSPI tft = TFT_eSPI(160, 320); // Invoke custom library

int BG_COLOR = 0xf7db;
int fgColor = 0x8430;
int fgColor2 = TFT_BLUE;

Oldgauge *myGauge;
void upButtonHandler()
{
    int value = digitalRead(MYPIN_CHANNEL_UP);
    if (value) {
        myGauge->channelUp();
    }
}
void downButtonHandler()
{
    int value = digitalRead(MYPIN_CHANNEL_DOWN);
    if (value) {
        myGauge->channelDown();
    }
}

void setup()
{
    Serial.begin(115200);
    Serial.setDebugOutput(true);
    // Serial.println("Start");

    // Register Our Devices
    bool useMockData = true;
    myGauge = new Oldgauge(useMockData, -1, MYPIN_PAIRING_LED, MYPIN_ERROR_LED);
    OG_Layout *layout1 = new OG_Layout("layout1");
    myGauge->addLayout(layout1);
    // ChannelSelector* mySelector = new ChannelSelector("selector");
    // myGauge->setChannelSelector(mySelector);
    OG_OutputDevice *myDisplay = new OG_T_Display("display", &tft);
    layout1->addOutputDevice(myDisplay);

    // Prepare the TFT Display
    tft.init();
    tft.fontHeight(1);
    tft.setRotation(1);
    tft.fillScreen(BG_COLOR);

    // Use the buttons on the board to select channels
    attachInterrupt(MYPIN_CHANNEL_UP, upButtonHandler, CHANGE);
    attachInterrupt(MYPIN_CHANNEL_DOWN, downButtonHandler, CHANGE);

    // int middle = 110;
    // int middle2 = 65;
    // tft.setTextDatum(TL_DATUM);
    // tft.setTextColor(fgColor, BG_COLOR, true);
    // tft.drawString("Checking wifi connection...", 50, 15, 4);  //string,start x,start y, font weight {1;2;4;6;7;8}
    // tft.drawString("values:", middle, 50, 4);
    // tft.drawString("pin:", middle2, 85, 2);
    // tft.drawString("mac addr:", middle2, 100, 2);
    // tft.drawString("clusterId:", middle2, 115, 2);
}

void loop()
{
    // // Load the font and create the Sprite for reporting the value
    // spr.loadFont(AA_FONT_LARGE);
    // spr_width = spr.textWidth("777"); // 7 is widest numeral in this font
    // // spr.createSprite(spr_width, spr.fontHeight());
    // // spr.createSprite(80, spr.fontHeight());
    // spr.createSprite(100, 100);
    // spr.fillSprite(BG_COLOR);
    // spr.setTextColor(fgColor, BG_COLOR, true);
    // // spr.setTextDatum(MC_DATUM);
    // spr.setTextDatum(TL_DATUM);
    // // spr.setTextPadding(spr_width);

    // int value = 123;
    // spr.drawString(String(value), spr_width/2, spr.fontHeight()/2, 7);

    // // spr.drawNumber(0, spr_width/2, spr.fontHeight()/2);
    // spr.drawString("Hello", 0, 0, 4);
    // // spr.set
    // spr.drawCircle(40, 40, 35, fgColor);

    if (myGauge->enterConfigModeIfTilted())
    {
        return;
    }

    myGauge->inTheLoop();
    delay(3000);
}
