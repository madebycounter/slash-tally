#include "button.h"

Button::Button(uint8_t pin, uint8_t mode, bool invert, unsigned long doublePressTime,
               unsigned long holdTime)
    : pin(pin),
      mode(mode),
      invert(invert),
      doublePressTime(doublePressTime),
      holdTime(holdTime),
      db(10),
      reading(false),
      prevReading(false),
      state(0),
      callbackOnPress(nullptr),
      callbackOnDoublePress(nullptr),
      callbackOnHold(nullptr) {
    pinMode(this->pin, mode);
}

void Button::update() {
    int raw = digitalRead(this->pin);
    this->db.update(this->invert ? !raw : raw);

    bool reading = this->db.state();
    bool buttonDown = reading != this->prevReading && reading;
    bool buttonUp = reading != this->prevReading && !reading;

    // STATE: idle
    if (this->state == 0) {
        if (buttonDown) {
            this->state = 1;
            this->timer = millis();
        }
    }

    // STATE: button down
    else if (this->state == 1) {
        if (buttonUp) {
            this->state = 2;
            // this->timer = millis();
        }

        else if (millis() - this->timer > this->holdTime) {
            this->state = 0;
            if (this->callbackOnHold) this->callbackOnHold();
        }
    }

    // STATE: pressed once
    else if (this->state == 2) {
        if (buttonDown && millis() - this->timer < this->doublePressTime) {
            this->state = 0;
            if (this->callbackOnDoublePress) this->callbackOnDoublePress();
        }

        else if (millis() - this->timer > this->doublePressTime) {
            this->state = 0;
            if (this->callbackOnPress) this->callbackOnPress();
        }
    }

    this->prevReading = reading;
}

void Button::onPress(void (*callback)()) {
    this->callbackOnPress = callback;
}

void Button::onDoublePress(void (*callback)()) {
    this->callbackOnDoublePress = callback;
}

void Button::onHold(void (*callback)()) {
    this->callbackOnHold = callback;
}
