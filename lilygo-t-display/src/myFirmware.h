#include <Arduino.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager
#include <Oldgauge.h>
#include "myFirmware.h"

#ifdef ESP32
  #include <SPIFFS.h>
#endif

// #include "esp-30pin.h"
#include "esp-38pin.h"



// Config
extern bool shouldSaveConfig;
extern char param_passphrase[50];
extern char param_pin[10];
extern char param_clusterID[128];
void rememberToSaveConfig();
void saveConfigIfNecessary();
void loadConfig();
void saveConfig();

// Captive Portal
extern WiFiManager wm;
extern WiFiManagerParameter passphrase_text_box;
extern WiFiManagerParameter pin_text_box;
extern WiFiManagerParameter clusterID_text_box;
void initialiseCaptivePortal();
bool enterConfigModeIfTilted();

// Rotary Encoder
void setupRotaryEncoder();
int getCurrentChannel();

// Config button
// void setupConfigButton();

// class myRotaryEncoder {
//   public:
//     myRotaryEncoder(int buttonPin, int clockPin, int dataPin);

//   private:
//     int pinSw;
//     int pinClk;
//     int pinDt;
//     void IRAM_ATTR encoderButtonCallbackZ();
// };

