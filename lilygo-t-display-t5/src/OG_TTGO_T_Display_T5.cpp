/*
 *
 * Display:
 *  https://www.lilygo.cc/products/t5-v2-3-1
 * 
 * Partial Updates:
 *  https://www.hackster.io/galoebn/e-paper-display-using-partial-updates-a8af20
 *
 * Converting images for LCD:
 *  https://www.youtube.com/watch?v=OPaCF-XJhqc
 * 
 */
#include <Arduino.h>
#include <Oldgauge.h>
#include "OG_TTGO_T_Display_T5.h"
#include <GxEPD2_BW.h>
#include <GxEPD2_3C.h>
#include <GxEPD2_4C.h>
#include <GxEPD2_7C.h>
#include <Fonts/FreeMonoBold9pt7b.h>
// #include "./splash.h"


// int BG_COLOR = 0xf7db;
// int fgColor = 0x8430;
// int fgColor2 = TFT_BLUE;

#define DISPLAY_WIDTH 135
#define DISPLAY_HEIGHT 240
#define POLL_X DISPLAY_WIDTH - 5
#define POLL_Y DISPLAY_HEIGHT - 5
#define DEFAULT_BG TFT_WHITE
#define HEADER_HEIGHT 19
#define HEADER_COLOR TFT_WHITE
#define HEADER_BGCOLOR TFT_BLUE

#define MSG_PADDING 10
#define MSG_X MSG_PADDING
#define MSG_Y MSG_PADDING
#define MSG_WIDTH (display.width() - (2 * MSG_PADDING))
#define MSG_HEIGHT (display.height() - (2 * MSG_PADDING))

// TFT_eSPI tft2 = TFT_eSPI(DISPLAY_WIDTH, DISPLAY_HEIGHT); // Invoke custom library
// TFT_eSprite img = TFT_eSprite(&tft2);
// extern GxEPD2_BW<GxEPD2_213_BN, GxEPD2_213_BN::HEIGHT> display;

GxEPD2_BW<GxEPD2_213_BN, GxEPD2_213_BN::HEIGHT> display(GxEPD2_213_BN(/*CS=5*/ SS, /*DC=*/ 17, /*RST=*/ 16, /*BUSY=*/ 4)); // DEPG0213BN 122x250, SSD1680, TTGO T5 V2.4.1, V2.3.1

#define MY_EVENT_MENU_BUTTON 1000
// #define MY_EVENT_CLOSEMENU 1001
bool og_t_display_displayingMenu = false;
int og_t_display_currentMode = OG_MODE_UNKNOWN;


// OG_T_Display_T5::OG_T_Display_T5(String name, show(* tft) : OG_Device(name) {
OG_T_Display_T5::OG_T_Display_T5(String name) : OG_Device(name) {
  // Serial.print("OG_T_Display_T5::OG_T_Display_T5(");
  // Serial.print(name);
  // Serial.println();
  _name = name;
  // _selector = selector;

  display.init(115200, true, 2, false);
  display.setRotation(1);



  // _tft = &tft2;

  // _tft->init();
  // _tft->fontHeight(1);
  // // _tft->setRotation(1);
  // _tft->fillScreen(TFT_WHITE);

  // // img.createSprite(DISPLAY_HEIGHT, DISPLAY_WIDTH);
  // img.createSprite(DISPLAY_WIDTH, DISPLAY_HEIGHT);
  // // img.setRotation(1);
  // img.setTextDatum(MC_DATUM);
  // img.setSwapBytes(true);

  // img.setFreeFont(&FreeSansBold9pt7b);
}

String OG_T_Display_T5::device() {
  return "OG_T_Display_T5";
}


JSONVar OG_T_Display_T5::toJSONVar() {
  /*
    Example:
    {
      "d": "A nice display",
      "v": {
        "dial": { "type": "int", "samples": 1 }
      }
    }
  */
  JSONVar me;
  me["d"] = "Two Dials";
  me["v"]["v1"]["type"] = "int";
  me["v"]["v1"]["samples"] = 1;
  me["v"]["v2"]["type"] = "int";
  me["v"]["v2"]["samples"] = 1;
  return me;
}


void OG_T_Display_T5::displayChannel(Oldgauge* cluster) {
  String channelName = cluster->_preferredChannel;
  JSONVar channelData = cluster->getChannelData(channelName);

  // Partial updates:
  // See https://forum.arduino.cc/t/e-paper-partial-update-i-dont-know-how/527555/6
  display.setPartialWindow(0, 0, display.width(), HEADER_HEIGHT);
  display.firstPage();
  do {
    // Draw a dark bar across the top
    display.fillScreen(GxEPD_BLACK);

    // // Draw the channel label
    String heading = channelName;
    JSONVar labelObj = channelData["label"];
    if (JSON.typeof(labelObj) == "string") {
      String s = String((const char *) labelObj);
      if (s != "") {
        heading = s;
      }
    }
    display.setFont(&FreeMonoBold9pt7b);
    if (display.epd2.WIDTH < 104) display.setFont(0);
    display.setTextColor(GxEPD_WHITE);
    display.setCursor(25, 16);
    display.print(heading);
  } while (display.nextPage());
}


void drawGauge(int index, String label, int value, int upper, int lower)
{
  // Serial.println("index=" + String(index) +", value="+String(value)+", lower="+String(lower)+", upper="+String(upper));
  float ratio = (float)(value - lower) / (float)(upper - lower);
  // Serial.print("ratio=");
  // Serial.println(ratio);

  int barwidth = 80;
  int barwidth2 = ratio * barwidth;
  // Serial.print("barwidth2=");
  // Serial.println(barwidth2);
  
  int top = 45 + 35 * index;
  int left = 45;

  display.setPartialWindow(0, top, display.width(), 20);
  // // display.setPartialWindow(0, HEADER_HEIGHT, display.width(), display.height() - HEADER_HEIGHT);
  display.firstPage();
  do {
    display.fillScreen(GxEPD_WHITE);

    if (label != "-") {
      display.setTextColor(GxEPD_BLACK);
      display.setCursor(7, top + 15);
      display.print(String(value));

      display.drawRect(left, top, barwidth, 20, GxEPD_BLACK);
      display.fillRect(left, top, barwidth2, 20, GxEPD_BLACK);

      display.setTextColor(GxEPD_BLACK);
      display.setCursor(left + barwidth + 10, top + 15);
      display.print(label);
    }
  }
  while (display.nextPage());

}


void OG_T_Display_T5::displayData(Oldgauge* cluster) {
  // Serial.printf("OG_T_Display_T5::displayData(%s)\n", _name.c_str());
  // Serial.printf("  channel=%d\n", _selector->currentChannel());


  // Partial updates:
  // See https://forum.arduino.cc/t/e-paper-partial-update-i-dont-know-how/527555/6
  String channelName = cluster->_preferredChannel;
  JSONVar channelData = cluster->getChannelData(channelName);
  // String layoutName = 

  JSONVar myData = channelData["outputs"][_name];
  // Serial.print("myData=");
  // Serial.println(myData);

  displayChannel(cluster);

  // Serial.print("display.width()=");
  // Serial.println(display.width());
  // Serial.print("display.height()=");
  // Serial.println(display.height());

  // display.setPartialWindow(0, 0, display.width(), 19);
  // display.firstPage();
  // do {
  //   // Draw a dark bar across the top
  //   display.fillScreen(GxEPD_BLACK);

  //   // // Draw the channel label
  //   String heading = channelName;
  //   JSONVar labelObj = channelData["label"];
  //   if (JSON.typeof(labelObj) == "string") {
  //     String s = String((const char *) labelObj);
  //     if (s != "") {
  //       heading = s;
  //     }
  //   }
  //   display.setFont(&FreeMonoBold9pt7b);
  //   if (display.epd2.WIDTH < 104) display.setFont(0);
  //   display.setTextColor(GxEPD_WHITE);
  //   display.setCursor(25, 16);
  //   display.print(heading);
  // } while (display.nextPage());

  // display.setPartialWindow(0, top, display.width(), 25);
  // display.setPartialWindow(0, HEADER_HEIGHT + 2, display.width(), display.height() - (HEADER_HEIGHT + 2));
  // Serial.println("Here we go");
  // display.firstPage();
  // do {

    // Lets show the first data value.
    JSONVar v1 = myData["v1"];
    if (JSON.typeof(v1) != "undefined")
    {
      String label = v1["label0"];
      int upper = v1["upper"];
      int lower = v1["lower"];
      JSONVar series = v1["series"];
      int num = series.length();
      if (num < 1) {
        // Not much we can do
        drawGauge(0, "-", 0, 0, 0);
      } else {
        JSONVar firstMetric = series[series.length() - 1];
        int value = firstMetric["v"];
        drawGauge(0, label, value, upper, lower);
      }
    } else {
      drawGauge(0, "-", 0, 0, 0);
    }

    JSONVar v2 = myData["v2"];
    if (JSON.typeof(v2) != "undefined")
    {
      String label = v2["label0"];
      int upper = v2["upper"];
      int lower = v2["lower"];
      JSONVar series = v2["series"];
      int num = series.length();
      if (num < 1) {
        // Not much we can do
        drawGauge(1, "-", 0, 0, 0);
      } else {
        JSONVar firstMetric = series[series.length() - 1];
        int value = firstMetric["v"];
        drawGauge(1, label, value, upper, lower);
      }
    } else {
      drawGauge(1, "-", 0, 0, 0);
    }
  // }
  // while (display.nextPage());
}


void OG_T_Display_T5::show(Oldgauge* cluster, String channelName, OG_Layout* layout) {
}


void drawMenu(Oldgauge* cluster) {
#ifdef NOTYET
  // Display a nice menu
  Serial.println("Display a nice menu");
  img.fillScreen(TFT_WHITE);
  img.setTextDatum(TL_DATUM);
  img.setTextColor(TFT_DARKGREY, TFT_WHITE, true);
  img.drawString("Setup", 20, 30, 1);
  img.setTextColor(TFT_BLACK, TFT_WHITE, true);
  // img.drawString(">", 5, 60, 1);
  // img.drawString("Channel 1", 20, 60, 1);
  // img.drawString("Channel 2", 25, 90, 1);
  // img.drawString("Channel 3", 25, 120, 1);
  // img.drawString("Channel 4", 25, 150, 1);

  String name = cluster->_preferredChannel;
  Serial.print("_preferredChannel=");
  Serial.println(name);

  int numChannels = cluster->numChannels();
  // Serial.print("numChannels=");
  // Serial.println(numChannels);

  for (int i = 0; i < numChannels; i++) {
    String name = cluster->getChannelName(i);
    // Serial.print("name=");
    // Serial.println(name);
    img.drawString(name, 15, 60 + (i * 30), 1);
    if (name == cluster->_preferredChannel) {
      img.drawString(">", 3, 60 + (i * 30), 1);
    }
  }
  img.pushSprite(0, 0);
  // delay(5000);
#endif
}

void OG_T_Display_T5::eventHandler(Oldgauge* cluster, bool isActiveDevice, ClusterEvent event, const char *data) {
  if (event == OG_EVENT_PAUSE || event == OG_EVENT_POLLING) {
    return;
  }
  // Serial.println("");
  // Serial.println("");
  // Serial.println("");
  // Serial.println("");
  // Serial.print("OG_T_Display_T5::eventHandler(");
  // Serial.print("event=");
  // Serial.print(event);
  // Serial.println(")");
  // Serial.print("_currentMode=");
  // Serial.println(_currentMode);

  int newMode = event;
  if (event == MY_EVENT_MENU_BUTTON) {
    if (og_t_display_displayingMenu) {
      Serial.println("----- Leave menu mode");
      og_t_display_displayingMenu = false;
      cluster->resumePolling(true);
      Serial.print("Returning to mode ");
      Serial.println(_currentMode);
      Serial.println();
      newMode = _currentMode;
      delay(5000);
    } else {
      Serial.println("----- Enter menu mode");
      og_t_display_displayingMenu = true;
      cluster->pausePolling();
      Serial.print("Entering with mode as ");
      Serial.println(_currentMode);
      Serial.println();
      delay(5000);
    }
  }

  if (og_t_display_displayingMenu) {
      drawMenu(cluster);
      return;
  }

  // Handle events (which don't change the mode)
  // if (event == OG_EVENT_PAUSE) {
  //   return;
  // }
  // if (event == OG_EVENT_POLLING) {
  //   return;
  // }

  switch (newMode) {
    case OG_MODE_STARTUP:
      {
        String message = "Initializing...";
        display.setFont(&FreeMonoBold9pt7b);
        if (display.epd2.WIDTH < 104) display.setFont(0);
        display.setTextColor(GxEPD_BLACK);
        int16_t tbx, tby; uint16_t tbw, tbh;
        display.getTextBounds(message, 0, 0, &tbx, &tby, &tbw, &tbh);
        // center bounding box by transposition of origin:
        uint16_t x = ((display.width() - tbw) / 2) - tbx;
        uint16_t y = ((display.height() - tbh) / 2) - tby;
        display.setFullWindow();
        display.firstPage();
        do
        {
          // display.setPartialWindow(MSG_X, MSG_Y, MSG_WIDTH, MSG_HEIGHT);
          display.fillScreen(GxEPD_WHITE);
          display.setCursor(x, y);
          display.print(message);
        }
        while (display.nextPage());
        _currentMode = newMode;
        delay(2000);
      }
      break;

    case OG_MODE_CONNECTING:
    {
        display.setFont(&FreeMonoBold9pt7b);
        if (display.epd2.WIDTH < 104) display.setFont(0);
        display.setTextColor(GxEPD_BLACK);
        int16_t tbx, tby; uint16_t tbw, tbh;

        String message1 = "Connecting";
        display.getTextBounds(message1, 0, 0, &tbx, &tby, &tbw, &tbh);
        // center bounding box by transposition of origin:
        uint16_t x1 = ((display.width() - tbw) / 2) - tbx;
        uint16_t y1 = ((display.height() - tbh) / 2) + tby;


        String message2 = "to";
        display.getTextBounds(message2, 0, 0, &tbx, &tby, &tbw, &tbh);
        // center bounding box by transposition of origin:
        uint16_t x2 = ((display.width() - tbw) / 2) - tbx;
        uint16_t y2 = ((display.height() - tbh) / 2) - tby;


        String message3 = "wifi router";
        display.getTextBounds(message3, 0, 0, &tbx, &tby, &tbw, &tbh);
        // center bounding box by transposition of origin:
        uint16_t x3 = ((display.width() - tbw) / 2) - tbx;
        uint16_t y3 = ((display.height() - tbh) / 2) - 3*tby;

        display.setPartialWindow(MSG_X, MSG_Y, MSG_WIDTH, MSG_HEIGHT);
        display.firstPage();
        do
        {
          // Y axis is from bottom
          display.fillScreen(GxEPD_WHITE);
          display.setCursor(x1, y1);
          display.print(message1);
          display.setCursor(x2, y2);
          display.print(message2);
          display.setCursor(x3, y3);
          display.print(message3);
        }
        while (display.nextPage());
    }
      // img.fillScreen(DEFAULT_BG);
      // img.setTextDatum(CC_DATUM);
      // img.setTextColor(TFT_BLACK, DEFAULT_BG, true);
      // img.drawString("Connecting", DISPLAY_WIDTH / 2, 35, 1);
      // img.drawString("to", DISPLAY_WIDTH / 2, 60, 1);
      // img.drawString("wifi router", DISPLAY_WIDTH / 2, 85, 1);
      // img.pushImage(0, 120, 135, 120, splashImage);
      // img.pushSprite(0, 0);
      _currentMode = newMode;
      break;

    case OG_MODE_CONFIG:
      {
        String message = "Config mode";
        display.setFont(&FreeMonoBold9pt7b);
        if (display.epd2.WIDTH < 104) display.setFont(0);
        display.setTextColor(GxEPD_BLACK);
        int16_t tbx, tby; uint16_t tbw, tbh;
        display.getTextBounds(message, 0, 0, &tbx, &tby, &tbw, &tbh);
        // center bounding box by transposition of origin:
        uint16_t x = ((display.width() - tbw) / 2) - tbx;
        uint16_t y = ((display.height() - tbh) / 2) - tby;
        display.setPartialWindow(MSG_X, MSG_Y, MSG_WIDTH, MSG_HEIGHT);
        display.firstPage();
        do
        {
          display.fillScreen(GxEPD_WHITE);
          display.setCursor(x, y);
          display.print(message);
        }
        while (display.nextPage());
        _currentMode = newMode;
        delay(2000);
      }
      // img.fillScreen(TFT_BLUE);
      // img.setTextDatum(CC_DATUM);
      // img.setTextColor(TFT_WHITE, TFT_BLUE, true);
      // img.drawString("Config mode", DISPLAY_WIDTH / 2, 30, 1);
      // img.drawString("Connect to the wifi", DISPLAY_WIDTH / 2, 70, 2);
      // img.drawString("access point named", DISPLAY_WIDTH / 2, 90, 2);
      // img.drawString(cluster->captivePortalName(), DISPLAY_WIDTH / 2, 120, 1);
      // img.drawString("and enter your", DISPLAY_WIDTH / 2, 150, 2);
      // img.drawString("router credentials.", DISPLAY_WIDTH / 2, 170, 2);
      // img.drawString("https://oldgauge.com", DISPLAY_WIDTH / 2, 225, 2);
      // img.pushSprite(0, 0);
      _currentMode = newMode;
      break;

    case OG_MODE_READY:
      {
        String message = "Getting data";
        display.setFont(&FreeMonoBold9pt7b);
        if (display.epd2.WIDTH < 104) display.setFont(0);
        display.setTextColor(GxEPD_BLACK);
        int16_t tbx, tby; uint16_t tbw, tbh;
        display.getTextBounds(message, 0, 0, &tbx, &tby, &tbw, &tbh);
        // center bounding box by transposition of origin:
        uint16_t x = ((display.width() - tbw) / 2) - tbx;
        uint16_t y = ((display.height() - tbh) / 2) - tby;
        display.setPartialWindow(MSG_X, MSG_Y, MSG_WIDTH, MSG_HEIGHT);
        display.firstPage();
        do
        {
          display.fillScreen(GxEPD_WHITE);
          display.setCursor(x, y);
          display.print(message);
        }
        while (display.nextPage());
        _currentMode = newMode;
        delay(2000);
      }
      // img.fillScreen(DEFAULT_BG);
      // img.setTextDatum(CC_DATUM);
      // img.setTextColor(TFT_BLACK, DEFAULT_BG, true);
      // img.drawString("Getting data", DISPLAY_WIDTH / 2, 60, 1);
      // img.pushImage(0, 120, 135, 120, splashImage);
      // img.pushSprite(0, 0);
      // img.pushSprite(0, 0);
      _currentMode = newMode;
      break;

    case OG_MODE_PAIRING:
      {
        String message = "Pairing";
        display.setFont(&FreeMonoBold9pt7b);
        if (display.epd2.WIDTH < 104) display.setFont(0);
        display.setTextColor(GxEPD_BLACK);
        int16_t tbx, tby; uint16_t tbw, tbh;
        display.getTextBounds(message, 0, 0, &tbx, &tby, &tbw, &tbh);
        // center bounding box by transposition of origin:
        uint16_t x = ((display.width() - tbw) / 2) - tbx;
        uint16_t y = ((display.height() - tbh) / 2) - tby;
        display.setPartialWindow(MSG_X, MSG_Y, MSG_WIDTH, MSG_HEIGHT);
        display.firstPage();
        do
        {
          display.fillScreen(GxEPD_WHITE);
          display.setCursor(x, y);
          display.print(message);
        }
        while (display.nextPage());
        _currentMode = newMode;
        delay(2000);
      }
      // img.fillScreen(TFT_BLUE);
      // img.setTextDatum(CC_DATUM);
      // img.setTextColor(TFT_WHITE, TFT_BLUE, true);
      // img.drawString("Pairing", DISPLAY_WIDTH / 2, 30, 1);
      // img.drawString("with", DISPLAY_WIDTH / 2, 60, 1);
      // img.drawString("server...", DISPLAY_WIDTH / 2, 90, 1);
      // img.pushSprite(0, 0);
      _currentMode = newMode;
      break;

    case OG_EVENT_POLLING:
      // Serial.println("OG_MODE_POLLING OG_MODE_POLLING OG_MODE_POLLING");
      // _tft->fillCircle(POLL_X, POLL_Y, 2, TFT_RED);
      break;

    case OG_EVENT_CHANNEL_CHANGED:
      Serial.println("channel change event");
      displayChannel(cluster);
      drawGauge(0, "-", 0, 0, 0);
      drawGauge(1, "-", 0, 0, 0);
      break;

    case OG_MODE_HAVEDATA:
      // Display the data, if this is the active layout
      if (isActiveDevice) {
        // Serial.println("HAVE DATA!!!");
        displayData(cluster);
      }
      _currentMode = newMode;
      break;

    case OG_EVENT_PAUSE:
      // Serial.println("PAUSE PAUSE PAUSE");
      // _tft->fillCircle(POLL_X, POLL_Y, 2, TFT_WHITE);
      break;

    case OG_MODE_ERROR:
    case OG_MODE_ERROR_NOT200:
    case OG_MODE_ERROR_BADDATA:
    case OG_MODE_ERROR_INVALID_JSON:
      {
        String message = "Error";
        display.setFont(&FreeMonoBold9pt7b);
        if (display.epd2.WIDTH < 104) display.setFont(0);
        display.setTextColor(GxEPD_BLACK);
        int16_t tbx, tby; uint16_t tbw, tbh;
        display.getTextBounds(message, 0, 0, &tbx, &tby, &tbw, &tbh);
        // center bounding box by transposition of origin:
        uint16_t x = ((display.width() - tbw) / 2) - tbx;
        uint16_t y = ((display.height() - tbh) / 2) - tby;
        display.setFullWindow();
        display.firstPage();
        do
        {
          display.fillScreen(GxEPD_WHITE);
          display.setCursor(x, y);
          display.print(message);
        }
        while (display.nextPage());
        _currentMode = newMode;
        delay(2000);
      }
      // img.fillScreen(TFT_RED);
      // img.setTextDatum(CC_DATUM);
      // img.setTextColor(TFT_WHITE, TFT_RED, true);
      // img.drawString("ERROR", DISPLAY_WIDTH / 2, 30, 1);
      // // img.drawString(String(cluster->getErrorStatus()), DISPLAY_WIDTH / 2, 60, 1);
      // img.drawString(cluster->getErrorMsg(), DISPLAY_WIDTH / 2, 90, 2);
      // img.pushSprite(0, 0);
      _currentMode = newMode;
      break;
  
    case OG_MODE_ERROR_BAD_PAIRING:
      {
        String message = "Bad pairing";
        display.setFont(&FreeMonoBold9pt7b);
        if (display.epd2.WIDTH < 104) display.setFont(0);
        display.setTextColor(GxEPD_BLACK);
        int16_t tbx, tby; uint16_t tbw, tbh;
        display.getTextBounds(message, 0, 0, &tbx, &tby, &tbw, &tbh);
        // center bounding box by transposition of origin:
        uint16_t x = ((display.width() - tbw) / 2) - tbx;
        uint16_t y = ((display.height() - tbh) / 2) - tby;
        display.setFullWindow();
        display.firstPage();
        do
        {
          display.fillScreen(GxEPD_WHITE);
          display.setCursor(x, y);
          display.print(message);
        }
        while (display.nextPage());
        _currentMode = newMode;
        delay(2000);
      }
      // img.fillScreen(TFT_RED);
      // img.setTextDatum(CC_DATUM);
      // img.setTextColor(TFT_WHITE, TFT_RED, true);
      // img.drawString("Pairing Error", DISPLAY_WIDTH / 2, 30, 1);
      // img.drawString("Unable to pair.", DISPLAY_WIDTH / 2, 70, 2);
      // img.drawString("The PIN my have", DISPLAY_WIDTH / 2, 100, 2);
      // img.drawString("expired, or your", DISPLAY_WIDTH / 2, 120, 2);
      // img.drawString("passphrase may", DISPLAY_WIDTH / 2, 140, 2);
      // img.drawString("be incorrect.", DISPLAY_WIDTH / 2, 160, 2);
      // img.pushSprite(0, 0);
      _currentMode = newMode;
      break;

    // default:
    //   Serial.println("Do nothing for this mode");
    //   break;
  }
  // Serial.print("Afterwards, _currentMode=");
  // Serial.println(_currentMode);
}
