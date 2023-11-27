/*
 *
 * Display:
 * https://www.lilygo.cc/products/lilygo%C2%AE-ttgo-t-display-1-14-inch-lcd-esp32-control-board
 * 
 * For sprites see:
 * https://github.com/Bodmer/TFT_eSPI/blob/master/examples/Sprite/Animated_dial/Animated_dial.ino
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
#include <Oldgauge.h>
#include <TFT_eSPI.h>
#include "OG_TTGO_T_Display.h"
#include "./splash.h"


// int BG_COLOR = 0xf7db;
// int fgColor = 0x8430;
// int fgColor2 = TFT_BLUE;

#define DISPLAY_WIDTH 135
#define DISPLAY_HEIGHT 240
#define POLL_X DISPLAY_WIDTH - 5
#define POLL_Y DISPLAY_HEIGHT - 5
#define DEFAULT_BG TFT_WHITE
#define HEADER_COLOR TFT_WHITE
#define HEADER_BGCOLOR TFT_BLUE

TFT_eSPI tft2 = TFT_eSPI(DISPLAY_WIDTH, DISPLAY_HEIGHT); // Invoke custom library
TFT_eSprite img = TFT_eSprite(&tft2);

#define MY_EVENT_MENU_BUTTON 1000
// #define MY_EVENT_CLOSEMENU 1001
bool og_t_display_displayingMenu = false;
int og_t_display_currentMode = OG_MODE_UNKNOWN;


// OG_T_Display::OG_T_Display(String name, TFT_eSPI* tft) : OG_Device(name) {
OG_T_Display::OG_T_Display(String name) : OG_Device(name) {
  // Serial.print("OG_T_Display::OG_T_Display(");
  // Serial.print(name);
  // Serial.println();
  _name = name;
  // _selector = selector;
  _tft = &tft2;

  _tft->init();
  _tft->fontHeight(1);
  // _tft->setRotation(1);
  _tft->fillScreen(TFT_WHITE);

  // img.createSprite(DISPLAY_HEIGHT, DISPLAY_WIDTH);
  img.createSprite(DISPLAY_WIDTH, DISPLAY_HEIGHT);
  // img.setRotation(1);
  img.setTextDatum(MC_DATUM);
  img.setSwapBytes(true);

  img.setFreeFont(&FreeSansBold9pt7b);
}

String OG_T_Display::device() {
  return "OG_T_Display";
}


JSONVar OG_T_Display::toJSONVar() {
  /*
    Exampe: {
            "type": "int",
            "samples": 1
          }
  */
  JSONVar me;
  me["v1"]["type"] = "int";
  me["v1"]["samples"] = 1;
  me["v2"]["type"] = "int";
  me["v2"]["samples"] = 1;
  return me;
}


void drawGauge(int middleX, int middleY, int width, String label, int value, int upper, int lower)
{

  // Serial.print("middleX=");
  // Serial.println(middleX);
  // Serial.print("middleY=");
  // Serial.println(middleY);
  int r = width / 2;
  // Serial.print("r=");
  // Serial.println(r);
  // int x = middleX - r;
  // int y = middleY + r;
  int r2 = r - 5;
  int angleMin = 55;
  int angleMax = 305;
  // Serial.print("x=");
  // Serial.println(x);
  // Serial.print("y=");
  // Serial.println(y);

  // int percent = ((value - lower) * 100) / (upper - lower);
  int percent = (value * 100) / 255;
  // Serial.print("percent=");
  // Serial.println(percent);
  int angle = angleMin + ((angleMax - angleMin) * percent) / 100;
  // Serial.print("angle=");
  // Serial.println(angle);
  // img.drawSmoothArc(x, y, r, r2, angleMin, angle, TFT_BLUE, TFT_RED, true);
  img.drawSmoothArc(middleX, middleY, r, r2, angleMin, angleMax, TFT_LIGHTGREY, TFT_RED, true);
  img.drawSmoothArc(middleX, middleY, r, r2, angleMin, angle, TFT_BLUE, TFT_RED, true);

  img.setTextDatum(CC_DATUM);
  // img.setTextColor(TFT_BLUE, BG_COLOR, false);
  img.setTextColor(TFT_BLUE, DEFAULT_BG, false);
  img.drawString(String(value), middleX, middleY + 2, 4);

  img.setTextColor(TFT_BLACK, DEFAULT_BG, false);
  img.drawString(label, middleX, middleY + r - 10, 2);

  // img.setTextColor(TFT_RED, TFT_GREENYELLOW, false);
  // img.drawString("1", 115, 20, 1);
  // img.drawString("2", 115, 40, 2);
  // img.drawString("3", 115, 60, 3);
  // img.drawString("4", 115, 80, 4);
  // img.drawString("5", 115, 100, 5);
  // img.drawString("6", 115, 120, 6);
  // img.drawString("7", 115, 140, 7);

}


void OG_T_Display::displayData(Oldgauge* cluster) {
  // String channelNameZ, OG_Layout* layoutZ) {
  Serial.printf("OG_T_Display::displayData(%s)\n", _name.c_str());
  // Serial.printf("  channel=%d\n", _selector->currentChannel());

  String channelName = cluster->_preferredChannel;
  JSONVar channelData = cluster->getChannelData(channelName);
  // String layoutName = 

  JSONVar myData = channelData["outputs"][_name];

  img.fillScreen(TFT_WHITE);

  // Draw the channel label
  String heading = channelName;
  JSONVar labelObj = channelData["label"];
  if (JSON.typeof(labelObj) == "string") {
    String s = String((const char *) labelObj);
    if (s != "") {
      heading = s;
    }
  }
  img.fillRect(0, 0, DISPLAY_WIDTH, 35, HEADER_BGCOLOR);
  img.setTextDatum(TC_DATUM);
  img.setTextColor(HEADER_COLOR, HEADER_BGCOLOR, true);
  img.drawString(heading, DISPLAY_WIDTH / 2, 7, 4);

  // Lets show the first data value.
  JSONVar v1 = myData["v1"];
  {
    String label = v1["label0"];
    String style = "regular";
    int value = 0;
    // int lower = v1["lower"];
    // int upper = v1["upper"];
    int v1middleX = DISPLAY_WIDTH / 2;
    int v1middleY = 95;
    // int v1middleY = 85;
    JSONVar series = v1["series"];
    int num = series.length();
    if (num < 1) {
      // Not much we can do
      img.setTextDatum(CC_DATUM);
      img.setTextColor(TFT_BLUE, DEFAULT_BG, false);
      img.drawString("no data", v1middleX, v1middleY + 2, 4);
    } else {
      JSONVar firstMetric = series[series.length() - 1];
      value = firstMetric["v"];
      // int min = firstMetric["min"];
      // int max = firstMetric["max"];
      int upper = firstMetric["upper"];
      int lower = firstMetric["lower"];

      int width = 75;
      drawGauge(v1middleX, v1middleY, width, label, value, upper, lower);
    }
  }

  // Lets show the second data value.
  int v2middleX = DISPLAY_WIDTH / 2;
  int v2middleY = 190;
  // int v2middleY = 170;
  JSONVar v2 = myData["v2"];
  {
    String label = v2["label0"];
    String style = "regular";
    int value = 0;
    // int lower = v2["lower"];
    // int upper = v2["upper"];
    JSONVar series = v2["series"];
    int num = series.length();
    if (num < 1) {
      // Not much we can do
      img.setTextDatum(CC_DATUM);
      img.setTextColor(TFT_BLUE, DEFAULT_BG, false);
      img.drawString("no data", v2middleX, v2middleY + 2, 4);
    } else {
      JSONVar firstMetric = series[series.length() - 1];
      value = firstMetric["v"];
      // int min = firstMetric["min"];
      // int max = firstMetric["max"];
      int lower = firstMetric["lower"];
      int upper = firstMetric["upper"];

      int width = 75;
      drawGauge(v2middleX, v2middleY, width, label, value, upper, lower);
    }
  }//- v2

      // img.drawString("Cluster: " + String(cluster->_clusterId), 5, DISPLAY_HEIGHT-60, 2);
      // img.drawString(String(cluster->_pin), DISPLAY_WIDTH/2, DISPLAY_HEIGHT-25, 1);
      // img.drawString(String(cluster->_passphrase), DISPLAY_WIDTH/2, DISPLAY_HEIGHT-10, 1);
  img.pushSprite(0, 0);


  // JSONVar channelLabel = data["label"];
  // _tft->setTextDatum(TL_DATUM);
  // _tft->setTextColor(fgColor, BG_COLOR, true);
  // // int channel = cluster->getSelectedChannel();
  // // _tft->drawString(String(channel), middle + 10, 15, 4);
  // String s = channelLabel;
  // _tft->drawString(s, 10, 15, 4);

  // JSONVar myData = data["data"];
  // JSONVar keys = myData.keys();

  // // Lets just show the first data value.
  // if (keys.length() >= 1) {
  //   JSONVar metric = myData[keys[0]];
  //   String label = metric["label"];
  //   String style = metric["style"];
  //   int value = metric["value"];

  //   _tft->setTextDatum(TR_DATUM);
  //   _tft->setTextColor(fgColor, BG_COLOR, true);
  //   _tft->drawString(label + ":", middle, 55, 4);
  //   // _tft->setTextColor(_v2Updated ? TFT_BLUE : TFT_ORANGE, BG_COLOR, true);

  //   _tft->setTextDatum(TR_DATUM);
  //   _tft->setTextColor(TFT_BLUE, BG_COLOR, true);
  //   _tft->drawString("   " + String(value), _tft->getViewportWidth() - 25, 55, 4);
  //   // Serial.println("SHOWING '" + label + "' AS VALUE " + String(value) + " WITH STYLE " + style);
  // }

  // if (keys.length() >= 2) {
  //   JSONVar metric = myData[keys[1]];
  //   String label = metric["label"];
  //   String style = metric["style"];
  //   int value = metric["value"];

  //   _tft->setTextDatum(TR_DATUM);
  //   _tft->setTextColor(fgColor, BG_COLOR, true);
  //   _tft->drawString(label + ":", middle, 90, 4);

  //   _tft->setTextDatum(TR_DATUM);
    
  //   _tft->setTextColor(TFT_BLUE, BG_COLOR, true);
  //   _tft->drawString("   " + String(value), _tft->getViewportWidth() - 25, 90, 4);
  //   // Serial.println("SHOWING '" + label + "' AS VALUE " + String(value) + " WITH STYLE " + style);
  // }

    // // _tft->drawString("channel:", middle, 15, 4);  //string,start x,start y, font weight {1;2;4;6;7;8}
    // _tft->drawString(label, middle, 50, 4);

    // // _tft->drawString("pin:", middle2, 85, 2);
    // // _tft->drawString("mac addr:", middle2, 100, 2);
    // // _tft->drawString("clusterId:", middle2, 115, 2);

    // _tft->setTextDatum(TL_DATUM);
    // _tft->setTextColor(fgColor2, BG_COLOR, true);
    // // _tft->setTextColor(fgColor, BG_COLOR, true);
    // // _tft->setTextColor(TFT_BLACK, BG_COLOR, true);
    // // _tft->drawString("187261  ", middle2 + 5, 85, 2);
    // // _tft->drawString("0a:22:e7:9c:22:eb  ", middle2 + 5, 100, 2);
    // // _tft->drawString("6b6081f3-962f-47aa-af38-64589d72aab5  ", middle2 + 5, 115, 2);

    // _tft->setTextColor(_v1Updated ? TFT_BLUE : TFT_ORANGE, BG_COLOR, true);
    // _tft->drawString(String(value) + "      ", middle + 10, 50, 4);
}



void OG_T_Display::show(Oldgauge* cluster, String channelName, OG_Layout* layout) {
  // Serial.printf("OG_T_Display::show(%s)\n", _name.c_str());
  // // Serial.printf("  channel=%d\n", _selector->currentChannel());
  JSONVar channelData = cluster->getChannelData(channelName);
  JSONVar myData = channelData["outputs"][_name];

  img.fillScreen(TFT_WHITE);


  // Draw the channel label
  String heading = channelName;
  JSONVar labelObj = channelData["label"];
  if (JSON.typeof(labelObj) == "string") {
    String s = String((const char *) labelObj);
    if (s != "") {
      heading = s;
    }
  }
  img.fillRect(0, 0, DISPLAY_WIDTH, 35, HEADER_BGCOLOR);
  img.setTextDatum(TC_DATUM);
  img.setTextColor(HEADER_COLOR, HEADER_BGCOLOR, true);
  img.drawString(heading, DISPLAY_WIDTH / 2, 7, 4);

  // Lets show the first data value.
  JSONVar v1 = myData["v1"];
  {
    String label = v1["label0"];
    String style = "regular";
    int value = 0;
    // int lower = v1["lower"];
    // int upper = v1["upper"];
    int v1middleX = DISPLAY_WIDTH / 2;
    int v1middleY = 95;
    // int v1middleY = 85;
    JSONVar series = v1["series"];
    int num = series.length();
    if (num < 1) {
      // Not much we can do
      img.setTextDatum(CC_DATUM);
      img.setTextColor(TFT_BLUE, DEFAULT_BG, false);
      img.drawString("no data", v1middleX, v1middleY + 2, 4);
    } else {
      JSONVar firstMetric = series[series.length() - 1];
      value = firstMetric["v"];
      // int min = firstMetric["min"];
      // int max = firstMetric["max"];
      int upper = firstMetric["upper"];
      int lower = firstMetric["lower"];

      int width = 75;
      drawGauge(v1middleX, v1middleY, width, label, value, upper, lower);
    }
  }

  // Lets show the second data value.
  int v2middleX = DISPLAY_WIDTH / 2;
  int v2middleY = 190;
  // int v2middleY = 170;
  JSONVar v2 = myData["v2"];
  {
    String label = v2["label0"];
    String style = "regular";
    int value = 0;
    // int lower = v2["lower"];
    // int upper = v2["upper"];
    JSONVar series = v2["series"];
    int num = series.length();
    if (num < 1) {
      // Not much we can do
      img.setTextDatum(CC_DATUM);
      img.setTextColor(TFT_BLUE, DEFAULT_BG, false);
      img.drawString("no data", v2middleX, v2middleY + 2, 4);
    } else {
      JSONVar firstMetric = series[series.length() - 1];
      value = firstMetric["v"];
      // int min = firstMetric["min"];
      // int max = firstMetric["max"];
      int lower = firstMetric["lower"];
      int upper = firstMetric["upper"];

      int width = 75;
      drawGauge(v2middleX, v2middleY, width, label, value, upper, lower);
    }
  }//- v2

      // img.drawString("Cluster: " + String(cluster->_clusterId), 5, DISPLAY_HEIGHT-60, 2);
      // img.drawString(String(cluster->_pin), DISPLAY_WIDTH/2, DISPLAY_HEIGHT-25, 1);
      // img.drawString(String(cluster->_passphrase), DISPLAY_WIDTH/2, DISPLAY_HEIGHT-10, 1);
  img.pushSprite(0, 0);



  // JSONVar channelLabel = data["label"];
  // _tft->setTextDatum(TL_DATUM);
  // _tft->setTextColor(fgColor, BG_COLOR, true);
  // // int channel = cluster->getSelectedChannel();
  // // _tft->drawString(String(channel), middle + 10, 15, 4);
  // String s = channelLabel;
  // _tft->drawString(s, 10, 15, 4);


  // JSONVar myData = data["data"];
  // JSONVar keys = myData.keys();

  // // Lets just show the first data value.
  // if (keys.length() >= 1) {
  //   JSONVar metric = myData[keys[0]];
  //   String label = metric["label"];
  //   String style = metric["style"];
  //   int value = metric["value"];

  //   _tft->setTextDatum(TR_DATUM);
  //   _tft->setTextColor(fgColor, BG_COLOR, true);
  //   _tft->drawString(label + ":", middle, 55, 4);
  //   // _tft->setTextColor(_v2Updated ? TFT_BLUE : TFT_ORANGE, BG_COLOR, true);

  //   _tft->setTextDatum(TR_DATUM);
  //   _tft->setTextColor(TFT_BLUE, BG_COLOR, true);
  //   _tft->drawString("   " + String(value), _tft->getViewportWidth() - 25, 55, 4);
  //   // Serial.println("SHOWING '" + label + "' AS VALUE " + String(value) + " WITH STYLE " + style);
  // }

  // if (keys.length() >= 2) {
  //   JSONVar metric = myData[keys[1]];
  //   String label = metric["label"];
  //   String style = metric["style"];
  //   int value = metric["value"];

  //   _tft->setTextDatum(TR_DATUM);
  //   _tft->setTextColor(fgColor, BG_COLOR, true);
  //   _tft->drawString(label + ":", middle, 90, 4);

  //   _tft->setTextDatum(TR_DATUM);
    
  //   _tft->setTextColor(TFT_BLUE, BG_COLOR, true);
  //   _tft->drawString("   " + String(value), _tft->getViewportWidth() - 25, 90, 4);
  //   // Serial.println("SHOWING '" + label + "' AS VALUE " + String(value) + " WITH STYLE " + style);
  // }




    // // _tft->drawString("channel:", middle, 15, 4);  //string,start x,start y, font weight {1;2;4;6;7;8}
    // _tft->drawString(label, middle, 50, 4);

    // // _tft->drawString("pin:", middle2, 85, 2);
    // // _tft->drawString("mac addr:", middle2, 100, 2);
    // // _tft->drawString("clusterId:", middle2, 115, 2);

    // _tft->setTextDatum(TL_DATUM);
    // _tft->setTextColor(fgColor2, BG_COLOR, true);
    // // _tft->setTextColor(fgColor, BG_COLOR, true);
    // // _tft->setTextColor(TFT_BLACK, BG_COLOR, true);
    // // _tft->drawString("187261  ", middle2 + 5, 85, 2);
    // // _tft->drawString("0a:22:e7:9c:22:eb  ", middle2 + 5, 100, 2);
    // // _tft->drawString("6b6081f3-962f-47aa-af38-64589d72aab5  ", middle2 + 5, 115, 2);

    // _tft->setTextColor(_v1Updated ? TFT_BLUE : TFT_ORANGE, BG_COLOR, true);
    // _tft->drawString(String(value) + "      ", middle + 10, 50, 4);


}

void drawMenu(Oldgauge* cluster) {
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
}

void OG_T_Display::eventHandler(Oldgauge* cluster, bool isActiveDevice, ClusterEvent event, const char *data) {
  if (event == OG_EVENT_PAUSE || event == OG_EVENT_POLLING) {
    return;
  }
  Serial.println("");
  Serial.println("");
  Serial.println("");
  Serial.println("");
  Serial.print("OG_T_Display::eventHandler(");
  Serial.print("event=");
  Serial.print(event);
  Serial.println(")");
  Serial.print("_currentMode=");
  Serial.println(_currentMode);

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
      img.fillScreen(DEFAULT_BG);
      img.setTextDatum(CC_DATUM);
      img.setTextColor(TFT_BLACK, DEFAULT_BG, true);
      img.drawString("Oldgauge", DISPLAY_WIDTH / 2, 40, 1);
      img.drawString("initializing...", DISPLAY_WIDTH / 2, 80, 2);
      img.pushImage(0, 120, 135, 120, splashImage);
      img.pushSprite(0, 0);
      _currentMode = newMode;
      delay(2000);
      break;

    case OG_MODE_CONNECTING:
      img.fillScreen(DEFAULT_BG);
      img.setTextDatum(CC_DATUM);
      img.setTextColor(TFT_BLACK, DEFAULT_BG, true);
      img.drawString("Connecting", DISPLAY_WIDTH / 2, 35, 1);
      img.drawString("to", DISPLAY_WIDTH / 2, 60, 1);
      img.drawString("wifi router", DISPLAY_WIDTH / 2, 85, 1);
      img.pushImage(0, 120, 135, 120, splashImage);
      img.pushSprite(0, 0);
      _currentMode = newMode;
      break;

    case OG_MODE_CONFIG:
      img.fillScreen(TFT_BLUE);
      img.setTextDatum(CC_DATUM);
      img.setTextColor(TFT_WHITE, TFT_BLUE, true);
      img.drawString("Config mode", DISPLAY_WIDTH / 2, 30, 1);
      img.drawString("Connect to the wifi", DISPLAY_WIDTH / 2, 70, 2);
      img.drawString("access point named", DISPLAY_WIDTH / 2, 90, 2);
      img.drawString(cluster->captivePortalName(), DISPLAY_WIDTH / 2, 120, 1);
      img.drawString("and enter your", DISPLAY_WIDTH / 2, 150, 2);
      img.drawString("router credentials.", DISPLAY_WIDTH / 2, 170, 2);
      img.drawString("https://oldgauge.com", DISPLAY_WIDTH / 2, 225, 2);
      img.pushSprite(0, 0);
      _currentMode = newMode;
      break;

    case OG_MODE_READY:
      img.fillScreen(DEFAULT_BG);
      img.setTextDatum(CC_DATUM);
      img.setTextColor(TFT_BLACK, DEFAULT_BG, true);
      img.drawString("Getting data", DISPLAY_WIDTH / 2, 60, 1);
      img.pushImage(0, 120, 135, 120, splashImage);
      img.pushSprite(0, 0);
      img.pushSprite(0, 0);
      _currentMode = newMode;
      break;

    case OG_MODE_PAIRING:
      img.fillScreen(TFT_BLUE);
      img.setTextDatum(CC_DATUM);
      img.setTextColor(TFT_WHITE, TFT_BLUE, true);
      img.drawString("Pairing", DISPLAY_WIDTH / 2, 30, 1);
      img.drawString("with", DISPLAY_WIDTH / 2, 60, 1);
      img.drawString("server...", DISPLAY_WIDTH / 2, 90, 1);
      img.pushSprite(0, 0);
      _currentMode = newMode;
      break;

    case OG_EVENT_POLLING:
      // Serial.println("OG_MODE_POLLING OG_MODE_POLLING OG_MODE_POLLING");
      _tft->fillCircle(POLL_X, POLL_Y, 2, TFT_RED);
      break;

    case OG_MODE_HAVEDATA:
      // Display the data, if this is the active layout
      if (isActiveDevice) {
        displayData(cluster);
      }
      _currentMode = newMode;
      break;

    case OG_EVENT_PAUSE:
      // Serial.println("PAUSE PAUSE PAUSE");
      _tft->fillCircle(POLL_X, POLL_Y, 2, TFT_WHITE);
      break;

    case OG_MODE_ERROR:
    case OG_MODE_ERROR_NOT200:
    case OG_MODE_ERROR_BADDATA:
    case OG_MODE_ERROR_INVALID_JSON:
      img.fillScreen(TFT_RED);
      img.setTextDatum(CC_DATUM);
      img.setTextColor(TFT_WHITE, TFT_RED, true);
      img.drawString("ERROR", DISPLAY_WIDTH / 2, 30, 1);
      // img.drawString(String(cluster->getErrorStatus()), DISPLAY_WIDTH / 2, 60, 1);
      img.drawString(cluster->getErrorMsg(), DISPLAY_WIDTH / 2, 90, 2);
      img.pushSprite(0, 0);
      _currentMode = newMode;
      break;
  
    case OG_MODE_ERROR_BAD_PAIRING:
      img.fillScreen(TFT_RED);
      img.setTextDatum(CC_DATUM);
      img.setTextColor(TFT_WHITE, TFT_RED, true);
      img.drawString("Pairing Error", DISPLAY_WIDTH / 2, 30, 1);
      img.drawString("Unable to pair.", DISPLAY_WIDTH / 2, 70, 2);
      img.drawString("The PIN my have", DISPLAY_WIDTH / 2, 100, 2);
      img.drawString("expired, or your", DISPLAY_WIDTH / 2, 120, 2);
      img.drawString("passphrase may", DISPLAY_WIDTH / 2, 140, 2);
      img.drawString("be incorrect.", DISPLAY_WIDTH / 2, 160, 2);
      img.pushSprite(0, 0);
      _currentMode = newMode;
      break;

    // default:
    //   Serial.println("Do nothing for this mode");
    //   break;
  }
  Serial.print("Afterwards, _currentMode=");
  Serial.println(_currentMode);
}
