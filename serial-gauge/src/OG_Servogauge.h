#include <Arduino.h>
#include <Arduino_JSON.h>
// #include <ESP32Servo.h>
// #include <WiFiManager.h>
#include <Oldgauge.h>


#ifndef _Servogauge_h_
#define _Servogauge_h_


class OG_Servogauge: public OG_Device {
  public:
    OG_Servogauge(String name, String deviceChar);
    JSONVar toJSONVar() override;
    void show(Oldgauge*, String channelName, OG_Layout*) override;
    String device() override;
    void eventHandler(Oldgauge* cluster, bool isActiveDevice, ClusterEvent mode, const char *data = null) override;

  private:
    String _deviceChar = "A";
    int _v1 = 0;
    bool _v1Updated = false;
    // bool _v2Updated = false;
    // int _servoPin;

    // Servo object to control a servo
    // 16 servo objects can be created on the ESP32
    // Servo _myservo;
};

#endif /* _Servogauge_h_ */
