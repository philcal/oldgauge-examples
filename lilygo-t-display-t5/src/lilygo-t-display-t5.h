// #include <TFT_eSPI.h>
// #include <Arduino_JSON.h>
#include <ESP32Servo.h>
#include <Oldgauge.h>

// Note these are not the pin order on the development board, but
// rather the pins on the chip (as shown by the labels on the board).
// See https://lastminuteengineers.com/esp32-pinout-reference/
// Pull up pins: https://linuxhint.com/esp32-pull-up-pins/

#define MYPIN_ENTER_CONFIG_MODE -1 // The down button
#define MYPIN_PAIRING_LED -1 // Blue = connect
#define MYPIN_ERROR_LED -1 // Red = error

// On the T_Display, we'll use the two buttons beside the USB-C connector
// See https://www.lilygo.cc/products/lilygo®-ttgo-t-display-1-14-inch-lcd-esp32-control-board
#define MYPIN_MENU_BUTTON  0
#define MYPIN_MENUITEM_BUTTON 35

// class OG_T_Display_T5 : public OG_Device {
//   public:
//     // OG_T_Display_T5(String name, TFT_eSPI* tft);
//     OG_T_Display_T5(String name);
//     JSONVar toJSONVar() override;
//     // void updateFromServer(JSONVar data);
//     // void inTheLoop(Oldgauge* cluster);
//     void show(Oldgauge*, String channelName, OG_Layout*) override;
//     // void displayData(Oldgauge*, String channelName, OG_Layout*);
//     void displayData(Oldgauge*);
//     String device() override;
//     void eventHandler(Oldgauge* cluster, bool isActiveDevice, ClusterEvent mode, const char *data = null) override;

//   private:
//     TFT_eSPI* _tft;
//     int _v1 = 0;
//     int _v2 = 0;
//     bool _v1Updated = false;
//     bool _v2Updated = false;

//     int _currentMode = OG_MODE_UNKNOWN;
//     bool _displayingMenu = false;
// };
