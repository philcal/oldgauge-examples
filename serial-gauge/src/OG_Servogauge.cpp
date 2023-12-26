/*
 *  This gauge drives a serial gauge that uses a tiny automotive servo.
 *  Board can be ordered from https://www.tindie.com/products/boffin/serial-x27-stepper-motor-gauge/
 *  See https://github.com/snafu-ca/serial-gauge for assembly details and protocol.
 * 
 *  If zeroing issues, this might help:
 *  https://github.com/clearwater/SwitecX25/issues/4
 * 
 *  To debug the serial connection, and by forwarding from the serial monitor to the device, see:
 *  https://icircuit.net/arduino-esp32-hardware-serial2-example/3181
 * 
 *  Pinout reference:
 *  https://lastminuteengineers.com/esp32-pinout-reference/
 */
#include <Arduino.h>
#include "./Oldgauge.h"
#include <ESP32Servo.h>
#include "./OG_Servogauge.h"


int OG_SERIALGAUGE_MIN = 100;
int OG_SERIALGAUGE_MAX = 600;


OG_Servogauge::OG_Servogauge(String name, String deviceChar = "A") : OG_Device(name) {
  // Serial.print("OG_ServoMeter::OG_Servogauge()");
  // _name = name;
  _deviceChar = deviceChar;


  // 	// Allow allocation of all timers
	// ESP32PWM::allocateTimer(0);
	// ESP32PWM::allocateTimer(1);
	// ESP32PWM::allocateTimer(2);
	// ESP32PWM::allocateTimer(3);
	// _myservo.setPeriodHertz(50);    // standard 50 hz servo
	// _myservo.attach(_servoPin, 500, 2400); // attaches the servo on pin 18 to the servo object
	// using default min/max of 1000us and 2000us
	// different servos may require different min/max settings
	// for an accurate 0 to 180 sweep
}

String OG_Servogauge::device() {
  return "OG_Servogauge";
}

JSONVar OG_Servogauge::toJSONVar() {
  JSONVar me;
  me["d"] = "Serialgauge";
  me["v"]["v1"]["type"] = "int";
  me["v"]["v1"]["samples"] = 1;
  // me["g1"]["min"] = 0;
  // me["g1"]["max"] = 255;
  return me;
}

// // Remove this.
// void OG_Servogauge::updateFromServer(JSONVar data) {
//   // Serial.printf("OG_Servogauge::updateFromServer(%s)\n", _name.c_str());
//   return;
// }


void OG_Servogauge::show(Oldgauge* cluster, String channelName, OG_Layout* layout) {
  // Serial.println("");
  // Serial.printf("OG_Servogauge::show(%s)\n", _name.c_str());

  // Serial.print("data=");
  // Serial.println(data);

  // Get my one data value
  JSONVar channelData = cluster->getChannelData(channelName);
  JSONVar myData = channelData["outputs"][_name];
  // JSONVar keys = myData.keys();
  // Serial.println("");
  // Serial.print("myData=");
  // Serial.println(myData);
  // myData={"v1":{"category":"dbCluster","label0":"Database Cluster","label1":"fgt-trtools-aurora","label2":"CPUUtilization",
  // "minutes":10,"lower":0,"upper":100,"unit":"Percent",
  // "series":[{"t":"2023-11-05T14:36:00.000Z","v":13.19,"min":12.77,"max":13.67}]},
  // "v2":{"category":"dbCluster","label0":"Database Cluster","label1":"trsyduat2db","label2":"CPUUtilization","minutes":10,"lower":0,"upper":100,"unit":"Percent","series":[{"t":"2023-11-05T14:36:00.000Z","v":14.54,"min":14.08,"max":15.53}]}}

  // Lets just show the first data value.
  JSONVar v1 = myData["v1"];
  // Serial.println("");
  // Serial.print("v1=");
  // Serial.println(v1);
  String label = v1["label0"];
  String style = "regular";
  // String style = firstMetric["style"];

  int value = 0;
  int lower = v1["lower"];
  int upper = v1["upper"];
  // Serial.print("upper=");
  // Serial.println(upper);
  // Serial.print("lower=");
  // Serial.println(lower);

  JSONVar series = v1["series"];
  int num = series.length();
  if (num < 1) {
    // Not much we can do
    Serial.print("Device ");
    Serial.print(_name);
    Serial.println(": no data");
    return;
  } else {
    JSONVar firstMetric = series[series.length() - 1];
    value = firstMetric["v"];
    // int min = firstMetric["min"];
    // int max = firstMetric["max"];
  }




  // Serial.print("min=");
  // Serial.println(min);
  // Serial.print("max=");
  // Serial.println(max);

  Serial.println("SHOWING '" + label + "' AS VALUE " + String(value) + " WITH STYLE " + style);

  // Convert from the data's value, min and max, to 0 - 180 degrees.
  int range = upper - lower;
  // Serial.print("range=");
  // Serial.println(range);
  int offset = value - lower;
  // Serial.print("offset=");
  // Serial.println(offset);
  int ratio = (1000 * offset) / range; // Stay with integer arithmetic
  // Serial.print("ratio=");
  // Serial.println(ratio);

  int rangeOfMotion = OG_SERIALGAUGE_MAX - OG_SERIALGAUGE_MIN;
  // Serial.print("rangeOfMotion=");
  // Serial.println(rangeOfMotion);
  int position = OG_SERIALGAUGE_MIN + ((ratio * rangeOfMotion) / 1000);
  // Serial.print("position=");
  // Serial.println(position);
  Serial2.println("A=" + String(position));
}

void OG_Servogauge::eventHandler(Oldgauge* cluster, bool isActiveDevice, ClusterEvent mode, const char *data) {
  Serial.println("OG_Servogauge::handleEvent");
  Serial.print("mode=");
  Serial.println(mode);

  switch (mode) {
    case OG_MODE_CONFIG:
    {
      // Needle left
      Serial2.println("A=300");
      break;
    }
    case OG_MODE_PAIRING:
    {
      // Vertical needle
      Serial2.println("A=474");
      break;
    }
    default:
      break;
  }
}
