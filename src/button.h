#pragma once
#include <Arduino.h>

#include "debounce.h"

class Button {
private:
    uint8_t pin;
    uint8_t mode;
    bool invert;

    unsigned long doublePressTime;
    unsigned long holdTime;

    Debounce db;
    bool reading;
    bool prevReading;
    uint8_t state;

    unsigned long timer;

    void (*callbackOnPress)();
    void (*callbackOnDoublePress)();
    void (*callbackOnHold)();

public:
    Button(uint8_t pin, uint8_t mode, bool invert = false, unsigned long doublePressTime = 300,
           unsigned long holdTime = 1000);

    void update();
    void onPress(void (*callback)());
    void onDoublePress(void (*callback)());
    void onHold(void (*callback)());
};
