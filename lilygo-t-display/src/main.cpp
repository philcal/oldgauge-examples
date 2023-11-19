
#include <Arduino.h>

// #include <TFT_eSPI.h>
// #include <SPI.h>
// #include "WiFi.h"
// #include <Wire.h>
// #include <Button2.h>
// #include "esp_adc_cal.h"
// #include "dial.h"

#include <Oldgauge.h>
#include "lilygo-t-display.h"


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
    // bool useMockData = false;
    // myGauge = new Oldgauge(useMockData, MYPIN_ENTER_CONFIG_MODE, MYPIN_PAIRING_LED, MYPIN_ERROR_LED);

    Serial.begin(115200);
    Serial.setDebugOutput(true);
    delay(1000);
    Serial.println("--------------------------------------------------");
    Serial.println("Start");

Serial.println("ok 1");
    // Register Our Devices
    myGauge = new Oldgauge(MYPIN_ENTER_CONFIG_MODE);
    myGauge->useMockData(false);
    OG_Layout *layout1 = myGauge->createLayout("layout1");
    // ChannelSelector* mySelector = new ChannelSelector("selector");
    // myGauge->setChannelSelector(mySelector);
    layout1->addOutputDevice(new OG_StatusLEDs("leds", MYPIN_PAIRING_LED, MYPIN_ERROR_LED));
    layout1->addOutputDevice(new OG_T_Display("meter"));

    // Get going
    myGauge->setup();

    // Use the buttons on the board to select channels
    if (MYPIN_CHANNEL_UP >= 0) {
        attachInterrupt(MYPIN_CHANNEL_UP, upButtonHandler, CHANGE);
    }
    if (MYPIN_CHANNEL_DOWN >= 0) {
        attachInterrupt(MYPIN_CHANNEL_DOWN, downButtonHandler, CHANGE);
    }
}

void loop()
{
    Serial.println("loop()");

    myGauge->loop();
    delay(1000);
}
