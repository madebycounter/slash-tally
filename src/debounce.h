#pragma once
#include <Arduino.h>

class Debounce {
private:
    bool debouncedState;
    bool lastState;
    unsigned long lastBounce;
    unsigned long delay;

public:
    Debounce(unsigned long delay);

    void update(bool reading);
    bool state();
};
