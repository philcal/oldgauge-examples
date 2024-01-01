
#include <Arduino.h>
#include <GxEPD2_BW.h>
#include <GxEPD2_3C.h>
#include <GxEPD2_4C.h>
#include <GxEPD2_7C.h>
#include <Fonts/FreeMonoBold9pt7b.h>


#include <Oldgauge.h>
#include "OG_TTGO_T_Display_T5.h"

// Include the Bounce2 library found here :
// https://github.com/thomasfredericks/Bounce2
// #include <Bounce2.h>


// INSTANTIATE A Bounce OBJECT.
// Bounce bounce = Bounce();


Oldgauge *myGauge = null;


// extern GxEPD2_BW<GxEPD2_213_BN, GxEPD2_213_BN::HEIGHT> display;




#define LONG_PRESS_TIME 3000
OG_Button* downButton;
OG_Button* upButton;



void setup()
{
    // bool useMockData = false;
    // myGauge = new Oldgauge(useMockData, MYPIN_ENTER_CONFIG_MODE, MYPIN_PAIRING_LED, MYPIN_ERROR_LED);

    Serial.begin(115200);
    Serial.setDebugOutput(true);
    // delay(1000);
    Serial.println("--------------------------------------------------");
    Serial.println("setup");
    delay(300);
    noInterrupts();

    // Prepare the display
    // display.init(115200, true, 2, false);
    // display.setRotation(1);

    // // Register Our Devices
    myGauge = new Oldgauge(MYPIN_ENTER_CONFIG_MODE);
    myGauge->useMockData(false);
    OG_Layout *layout1 = myGauge->createLayout("layout1");
    // // ChannelSelector* mySelector = new ChannelSelector("selector");
    // // myGauge->setChannelSelector(mySelector);
    // layout1->addOutputDevice(new OG_StatusLEDs("leds", MYPIN_PAIRING_LED, MYPIN_ERROR_LED));
    layout1->addOutputDevice(new OG_T_Display_T5("meter"));
    layout1->addOutputDevice(new OG_PwmMeter("Meter 1", MYPIN_METER1, MYCHANNEL_METER1));
    layout1->addOutputDevice(new OG_PwmMeter("Meter 2", MYPIN_METER2, MYCHANNEL_METER2));
    layout1->addOutputDevice(new OG_PwmMeter("Meter 3", MYPIN_METER3, MYCHANNEL_METER3));

    // Get going
    myGauge->setup();
    Serial.println("Back from myGauge->setup() YARP");

    // Use the button to select channels
    Serial.println("YARP 1a");
    downButton = new OG_Button(MYPIN_CHANNEL_DOWN, LONG_PRESS_TIME);
    Serial.println("YARP 1b");
    upButton = new OG_Button(MYPIN_CHANNEL_UP, -1);
    Serial.println("End of setup() YARP");
}

void loop()
{

  downButton->checkButton();
  switch (downButton->hasBeenPressed()) {
    case OG_SHORT_PRESS:
    case OG_LONG_PRESS:
      // short press - switch channel.
      Serial.println("switch channel.");
      myGauge->channelDown();
      break;

    // case OG_LONG_PRESS:
    //   // long press - enter config mode
    //   Serial.println("Long down press - enter config mode");
    //   myGauge->rebootAndStartCaptivePortal();
    //   return;

    default:
      // 
      break;
  }
  upButton->checkButton();
  if (upButton->hasBeenPressed()) {
    Serial.println("\n\n\nUp button!!!!!\n\n");
    myGauge->channelUp();
  }

  myGauge->loop();
  delay(100);
}
