
#include <Arduino.h>
#include <Oldgauge.h>
#include "OG_TTGO_T_Display.h"


Oldgauge *myGauge;


void setup()
{
    Serial.begin(115200);
    Serial.setDebugOutput(true);
    // delay(1000);
    Serial.println("--------------------------------------------------");
    Serial.println("Start");

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
    // if (MYPIN_CHANNEL_UP >= 0) {
        // attachInterrupt(, modeButtonHandler, CHANGE);
        // pinMode(MYPIN_MENU_BUTTON, INPUT_PULLDOWN);
        attachInterrupt(MYPIN_MENU_BUTTON, []{
            int value = digitalRead(MYPIN_MENU_BUTTON);
            if (value) {
                myGauge->runInNextLoop([]{
                    // Serial.println("Running in the loop #1");
                    // myGauge->enterMenuMode();
                    myGauge->triggerEvent(1000);
                    // myGauge->channelUp();
                });
            }
        }, CHANGE);
    // }
    // if (MYPIN_CHANNEL_DOWN >= 0) {
        pinMode(MYPIN_MENUITEM_BUTTON, INPUT_PULLDOWN);
        attachInterrupt(MYPIN_MENUITEM_BUTTON, []{
            // Serial.println("menuItem");
            int value = digitalRead(MYPIN_MENUITEM_BUTTON);
            if (value) {
                myGauge->runInNextLoop([]{
                    Serial.println("CHANNEL DOWN");
                    Serial.println("");
                    // Serial.println("Running in the loop #1");
                    // myGauge->enterMenuMode();
                    // myGauge->triggerEvent(OG_MODE_MENU);
//                    myGauge->channelDown();
                    // myGauge->channelUp();
                });
                // myGauge->scheduleEvent(CHANNELDOWN);
                // myGauge->triggerEvent(OG_MODE_MENU);
            }
        }, CHANGE);
        // }, FALLING);
    // }
}

void loop()
{
    // Serial.println("loop()");

    myGauge->loop();
    // delay(1000);
}
