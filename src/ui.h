#pragma once

#include <Arduino.h>
#include <FastLED.h>

#define UI_NUM_LEDS 25
#define UI_HEARTBEAT_TOLERANCE 2000

#define UI_BITRATE_CRITICAL 1
#define UI_BITRATE_BAD 2000
#define UI_BITRATE_OK 4000
#define UI_BITRATE_GOOD 6000

#define UI_FRAMERATE_CRITICAL 25
#define UI_FRAMERATE_BAD 27
#define UI_FRAMERATE_OK 28
#define UI_FRAMERATE_GOOD 29

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
void ui_spinner(CRGB col);
void ui_signal(SignalType type);
void ui_heartbeat();
void ui_update(String program, String preview, String camera, bool transitioning, bool access_point);
