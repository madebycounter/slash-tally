#pragma once

#include <Arduino.h>
#include <FastLED.h>

#define UI_NUM_LEDS 25

enum SignalType {
    UI_SIGNAL_SOLID,
    UI_SIGNAL_ARROW_DOWN,
    UI_SIGNAL_ARROW_UP,
    UI_SIGNAL_ARROW_LEFT,
    UI_SIGNAL_ARROW_RIGHT,
    UI_SIGNAL_REPEAT,
    UI_SIGNAL_NEGATIVE,
    UI_SIGNAL_POSITIVE
};

void ui_init();
void ui_set_brightness(int brightness);
void ui_signal(SignalType type);
void ui_update(String program, String preview, String camera, bool transitioning);
