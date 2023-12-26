/*
 *  https://www.instructables.com/Blinking-an-LED-With-ESP32/
 *
 * Reference:
 *  Pins - not all pins do all things
 *  https://www.studiopieters.nl/esp32-pinout/
 *  https://lastminuteengineers.com/esp32-pinout-reference/
 *  https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/gpio.html
 *  https://www.espressif.com/sites/default/files/documentation/esp32_datasheet_en.pdf
 * 
 *  https://surplustronics.co.nz/products/9368-esp32-development-board-38-pin-espwroom-32s
 * 
 *  Button to enter config mode
 *  https://github.com/tzapu/WiFiManager/blob/master/examples/Advanced/Advanced.ino
 * 
 *  Saving values to file system on device
 *  https://github.com/tzapu/WiFiManager/blob/master/examples/Parameters/SPIFFS/AutoConnectWithFSParameters/AutoConnectWithFSParameters.ino
 * 
 *  Introducing SPIFFS
 *  https://randomnerdtutorials.com/esp32-vs-code-platformio-spiffs/
 *  https://circuitdigest.com/microcontroller-projects/rotary-encoder-module-interfacing-with-arduino#:~:text=Connecting%20the%20rotary%20encoder%20module,will%20configure%20that%20as%20pin
 * 
 *  Rotary encoder and servo
 *  https://lastminuteengineers.com/rotary-encoder-arduino-tutorial/
 *  
 *  Interrupts
 *  https://circuitdigest.com/microcontroller-projects/esp32-interrupt
 */
#include <Arduino.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager
#include <HTTPClient.h>
#include <Arduino_JSON.h>
#include <Oldgauge.h>


// #include "esp-30pin.h"
#include "esp-38pin.h"

// const String URL = "http://192.168.68.101:3000/usage/12345";
const String URL = "https://u4rurbd9ic.execute-api.ap-southeast-1.amazonaws.com/staging/metrics";

// Oldgauge* myGauge;
Oldgauge* myGauge;

RotaryEncoderChannelSelector* channelSelector;
void encoderButtonHandler() {
  channelSelector->encoderButtonCallback();
}
void encoderRotateHandler() {
  channelSelector->encoderRotateCallback();
}

void setup()
{
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println("\n\n\nsetup()");
  
  // Explicitly set mode, esp defaults to STA+AP
  WiFi.mode(WIFI_STA);

  bool useMockData = true;
  myGauge = new Oldgauge(MYPIN_ENTER_CONFIG_MODE);
    // useMockData, MYPIN_ENTER_CONFIG_MODE, MYPIN_PAIRING_LED, MYPIN_ERROR_LED);

  // channelSelector = new RotaryEncoderChannelSelector(myGauge,
  //   PIN_ROTARY_SW, PIN_ROTARY_CLK, PIN_ROTARY_DAT,
  //   encoderButtonHandler, encoderRotateHandler);

  // Register Our Devices
  OG_Layout* layout1 = myGauge->createLayout("layout1");
  layout1->addOutputDevice(new OG_PwmMeter("Meter 1", MYPIN_METER1, MYCHANNEL_METER1));

  layout1->addOutputDevice(new OG_ChannelSelectionLEDs(myGauge, "leds",
    MYPIN_WHITE_LED_1, MYPIN_WHITE_LED_2, MYPIN_WHITE_LED_3,
    MYPIN_WHITE_LED_4, MYPIN_WHITE_LED_5, MYPIN_WHITE_LED_6));

  myGauge->setup();

  Serial.println("local ip");
  Serial.println(WiFi.localIP());
}

void loop()
{
  myGauge->loop();
  delay(100);
}
