#include "debounce.h"

Debounce::Debounce(unsigned long delay)
    : delay(delay), debouncedState(false), lastState(false), lastBounce(0) {
}

void Debounce::update(bool reading) {
    if (reading != this->lastState) {
        this->lastBounce = millis();
    }

    if ((millis() - this->lastBounce) > this->delay) {
        if (reading != this->debouncedState) {
            this->debouncedState = reading;
        }
    }

    this->lastState = reading;
}

bool Debounce::state() {
    return this->debouncedState;
}