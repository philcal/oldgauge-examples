#include <Arduino_JSON.h>
#include <Oldgauge.h>

// https://www.aliexpress.us/item/3256802288539966.html?gatewayAdapt=glo2usa4itemAdapt

/*  References:
 *    https://www.lilygo.cc/products/t5-v2-3-1
 *    https://github.com/Xinyuan-LilyGO/LilyGo-T5-Epaper-Series
 * 
 *  Pinouts:
 *    /resources/T5-V2_16.png
 *    /resources/T5-V2_18.png
 * 
 *  https://github.com/ZinggJM/GxEPD
 */

#define MYPIN_PAIRING_LED -1 // Blue = connect
#define MYPIN_ERROR_LED -1 // Red = error

// On the T_Display, we'll use the two buttons beside the USB-C connector
// See https://www.lilygo.cc/products/lilygo®-ttgo-t-display-1-14-inch-lcd-esp32-control-board
// #define MYPIN_MENU_BUTTON  0
// #define MYPIN_MENUITEM_BUTTON 35
// #define MYPIN_CHANNEL_DOWN 2
#define MYPIN_ENTER_CONFIG_MODE GPIO_NUM_12
// #define MYPIN_ENTER_CONFIG_MODE GPIO_NUM_2

#define MYPIN_CHANNEL_UP      GPIO_NUM_27
#define MYPIN_CHANNEL_DOWN    GPIO_NUM_39 // same as second button
#define MYPIN_METER1          GPIO_NUM_13 // CPU
#define MYPIN_METER2          GPIO_NUM_14 // Memory
#define MYPIN_METER3          GPIO_NUM_15 // Database

// #define MYPIN_CHANNEL_UP      -1
// #define MYPIN_CHANNEL_DOWN    -1
// #define MYPIN_METER1          -1
// #define MYPIN_METER2          -1
// #define MYPIN_METER3          -1

#define MYCHANNEL_METER1      0
#define MYCHANNEL_METER2      1
#define MYCHANNEL_METER3      2


class OG_T_Display_T5 : public OG_Device {
  public:
    OG_T_Display_T5(String name);
    JSONVar toJSONVar() override;
    void show(Oldgauge*, String channelName, OG_Layout*) override;
    // void displayGauges(Oldgauge*, String channelName, OG_Layout*);
    void displayChannel(Oldgauge* cluster);
    void displayData(Oldgauge* cluster);
    String device() override;
    void eventHandler(Oldgauge* cluster, bool isActiveDevice, ClusterEvent mode, const char *data = null) override;

  private:
    // TFT_eSPI* _tft;
    int _v1 = 0;
    int _v2 = 0;
    bool _v1Updated = false;
    bool _v2Updated = false;

    int _currentMode = OG_MODE_UNKNOWN;
    bool _displayingMenu = false;
};
