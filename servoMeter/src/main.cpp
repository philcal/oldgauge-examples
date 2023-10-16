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




// #include <ESP32Servo.h>
// Servo myservo;  // create servo object to control a servo
// // 16 servo objects can be created on the ESP32
 
// int pos = 0;    // variable to store the servo position
// // Recommended PWM GPIO pins on the ESP32 include 2,4,12-19,21-23,25-27,32-33 
// int servoPin = MYPIN_METER;




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

  // // Allow allocation of all timers
	// ESP32PWM::allocateTimer(0);
	// ESP32PWM::allocateTimer(1);
	// ESP32PWM::allocateTimer(2);
	// ESP32PWM::allocateTimer(3);
	// myservo.setPeriodHertz(50);    // standard 50 hz servo
	// myservo.attach(servoPin, 500, 2400); // attaches the servo on pin 18 to the servo object
	// // using default min/max of 1000us and 2000us
	// // different servos may require different min/max settings
	// // for an accurate 0 to 180 sweep
  // return;//ZZZZ
  
  // Explicitly set mode, esp defaults to STA+AP
  WiFi.mode(WIFI_STA);

  bool useMockData = true;
  myGauge = new Oldgauge(useMockData, MYPIN_ENTER_CONFIG_MODE, MYPIN_PAIRING_LED, MYPIN_ERROR_LED);

  channelSelector = new RotaryEncoderChannelSelector(myGauge,
    PIN_ROTARY_SW, PIN_ROTARY_CLK, PIN_ROTARY_DAT,
    encoderButtonHandler, encoderRotateHandler);

  // Register Our Devices
  OG_Layout* layout1 = new OG_Layout("layout1");
  myGauge->addLayout(layout1);
  layout1->addOutputDevice(new OG_ServoMeter("meter", MYPIN_METER));
  layout1->addOutputDevice(new OG_ChannelSelectionLEDs(myGauge, "leds",
    MYPIN_WHITE_LED_1, MYPIN_WHITE_LED_2, MYPIN_WHITE_LED_3,
    MYPIN_WHITE_LED_4, MYPIN_WHITE_LED_5, MYPIN_WHITE_LED_6));

  // initialize LED digital pin as an output.
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.println("local ip");
  Serial.println(WiFi.localIP());
}

void loop()
{
// for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
// 		// in steps of 1 degree
// 		myservo.write(pos);    // tell servo to go to position in variable 'pos'
// 		delay(15);             // waits 15ms for the servo to reach the position
// 	}
// 	for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
// 		myservo.write(pos);    // tell servo to go to position in variable 'pos'
// 		delay(15);             // waits 15ms for the servo to reach the position
// 	}
//   return;



  if (myGauge->enterConfigModeIfTilted()) {
    return;
  }
  myGauge->inTheLoop();
  delay(5000);
}
