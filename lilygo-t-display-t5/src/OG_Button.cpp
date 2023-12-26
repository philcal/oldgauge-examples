#include <Oldgauge.h>


#define DEBOUNCE_TIME  50 // the debounce time in millisecond, increase this time if it still chatters

void IRAM_ATTR isr(void* param) {
  // Do not use Serial in here.
  OG_Button* button = (OG_Button *) param;
  button->checkButton();
}

OG_Button::OG_Button(int pin, int longPressDuration) {
Serial.println("YARP 2a");
  _PIN = pin;
Serial.println("YARP 2b");
  if (_PIN < 0) return; // disabled
Serial.println("YARP 2c");
  _longPressDuration = longPressDuration;
Serial.println("YARP 2d");
  pinMode(pin, INPUT_PULLUP);
Serial.println("YARP 2e");
  attachInterruptArg(digitalPinToInterrupt(pin), isr, this, CHANGE);
Serial.println("YARP 2f");
};


void IRAM_ATTR OG_Button::checkButton() {
  // Note - do not use Serial in here, it gets called by interrupt handlers.
  if (_PIN < 0) return; // disabled
  long now = millis();
  if (now < _ignoreTill) {
    return;
  }

  // read the state of the switch/button:
  int state = digitalRead(_PIN);
  if (state == LOW && _currentState == LOW) {

    // Button remains pressed down
    if (_buttonDownTime > 0) {
      int timePressed = now - _buttonDownTime;
      if (_longPressDuration > 0 && timePressed> _longPressDuration) {
        // Long press. No need to wait till the expiry time.
        _pressed = OG_LONG_PRESS;
        _buttonDownTime = 0; // prevent double triggering.
      }
    }
  } else if (state == LOW && _currentState == HIGH) {

    // Button has been pressed down
    _buttonDownTime = now;
    _ignoreTill = now + DEBOUNCE_TIME;
  } else if (state == HIGH && _currentState == LOW) {

    // Button has been released
    _pressDuration = now - _buttonDownTime;
    if (_buttonDownTime > 0 ) {
      int timePressed = now - _buttonDownTime;
      if (_longPressDuration > 0 && timePressed > _longPressDuration) {
        _pressed = OG_LONG_PRESS;
      } else {
        _pressed = OG_SHORT_PRESS;
      }
    }
  }
  _currentState = state;
}

OG_ButtonStatus OG_Button::hasBeenPressed() {
  if (_PIN < 0) return OG_NOT_PRESSED; // disabled

  OG_ButtonStatus rv = _pressed;
  _pressed = OG_NOT_PRESSED;
  return rv;
}
