#include "ui.h"

uint8_t BM_CONNECTED[UI_NUM_LEDS] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

uint8_t BM_ARROW_UP[UI_NUM_LEDS] = {0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1,
                                    1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

uint8_t BM_ARROW_DOWN[UI_NUM_LEDS] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1,
                                      1, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 0};

uint8_t BM_ARROW_LEFT[UI_NUM_LEDS] = {0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1,
                                      0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0};

uint8_t BM_ARROW_RIGHT[UI_NUM_LEDS] = {0, 0, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1,
                                       1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0};

uint8_t BM_REPEAT[UI_NUM_LEDS] = {0, 1, 1, 0, 0, 0, 1, 0, 1, 0, 0, 1, 1,
                                  0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0};

uint8_t BM_NEGATIVE[UI_NUM_LEDS] = {1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1,
                                    0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1};

uint8_t BM_POSITIVE[UI_NUM_LEDS] = {0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1,
                                    1, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 0};

uint8_t BM_SOLID[UI_NUM_LEDS] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

uint8_t BM_CENTER[UI_NUM_LEDS] = {0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1,
                                  1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0};

CRGB leds[UI_NUM_LEDS];
long signal_counter = -99999;
long heartbeat_counter = -99999;
bool waiting_first_heartbeat = true;
SignalType signal_type = UI_SIGNAL_SOLID;

void set_rgb_bitmap(CRGB col, uint8_t bitmap[UI_NUM_LEDS], bool invert = false, bool clear = true) {
    if (clear) {
        FastLED.clear();
    }

    for (int i = 0; i < UI_NUM_LEDS; i++) {
        if (invert ? !bitmap[i] : bitmap[i]) {
            leds[i] = col;
        }
    }
}

void set_rgb_solid(CRGB col) {
    fill_solid(leds, UI_NUM_LEDS, col);
}

void set_rgb_dithered(CRGB col) {
    for (int i = 0; i < UI_NUM_LEDS; i+=2) {
        leds[i] = col;
    }
}

void set_rgb_off() {
    set_rgb_solid(CRGB::Black);
}

void set_rgb_idle() {
    set_rgb_bitmap(CRGB::Blue, BM_CONNECTED);
}

void ui_init() {
    FastLED.addLeds<WS2812B, 27, GRB>(leds, UI_NUM_LEDS);
}

void ui_set_brightness(int brightness) {
    FastLED.setBrightness(brightness);
}

void ui_spinner(CRGB col) {
    static const uint8_t ring[] = {6, 7, 8, 13, 18, 17, 16, 11};
    static const uint8_t RING_LEN = 8;
    static const uint8_t TRAIL_LEN = 5;

    FastLED.clear();

    int head = (millis() / 100) % RING_LEN;

    for (int i = 0; i < TRAIL_LEN; i++) {
        int idx = (head - i + RING_LEN) % RING_LEN;
        uint8_t brightness = 255 - (i * (255 / TRAIL_LEN));
        leds[ring[idx]] = CRGB(
            scale8(col.r, brightness),
            scale8(col.g, brightness),
            scale8(col.b, brightness)
        );
    }

    FastLED.show();
}

void ui_signal(SignalType type) {
    signal_counter = millis();
    signal_type = type;
}

void ui_heartbeat() {
    heartbeat_counter = millis();
    waiting_first_heartbeat = false;
}

void ui_update(String program, String preview, String camera, bool transitioning, bool access_point) {
    if (waiting_first_heartbeat) {
        ui_spinner(access_point ? CRGB::Green : CRGB::Red);
        return;
    }

    bool onScreen = program == camera || preview == camera;
    bool onAir = program == camera;

    if (millis() - signal_counter < 1200) {
        if (((millis() - signal_counter) / 100) % 2 == 0) {
            set_rgb_off();
        } else {
            if (signal_type == UI_SIGNAL_ARROW_UP) {
                set_rgb_bitmap(CRGB::Purple, BM_ARROW_UP);
            } else if (signal_type == UI_SIGNAL_ARROW_DOWN) {
                set_rgb_bitmap(CRGB::Purple, BM_ARROW_DOWN);
            } else if (signal_type == UI_SIGNAL_ARROW_LEFT) {
                set_rgb_bitmap(CRGB::Purple, BM_ARROW_LEFT);
            } else if (signal_type == UI_SIGNAL_ARROW_RIGHT) {
                set_rgb_bitmap(CRGB::Purple, BM_ARROW_RIGHT);
            } else if (signal_type == UI_SIGNAL_REPEAT) {
                set_rgb_bitmap(CRGB::Purple, BM_REPEAT);
            } else if (signal_type == UI_SIGNAL_POSITIVE) {
                set_rgb_bitmap(CRGB::Purple, BM_POSITIVE);
            } else if (signal_type == UI_SIGNAL_NEGATIVE) {
                set_rgb_bitmap(CRGB::Purple, BM_NEGATIVE);
            } else {
                set_rgb_bitmap(CRGB::Purple, BM_SOLID);
            }
        }
    } else if (onScreen && transitioning) {
        set_rgb_off();

        if (int(millis() / 100.0) % 2 == 0) {
            set_rgb_dithered(CRGB::Red);
        }
    } else if (onAir) {
        set_rgb_solid(CRGB::Red);
    } else if (onScreen) {
        set_rgb_solid(CRGB::Green);
    } else {
        set_rgb_off();
    }

    CRGB heartbeat_color = access_point ? CRGB::Red : CRGB::Blue;

    if (millis() - heartbeat_counter < UI_HEARTBEAT_TOLERANCE) {
        leds[UI_NUM_LEDS - 1] = heartbeat_color;
    } else {
        if ((millis() / 100) % 2 == 0) {
            leds[UI_NUM_LEDS - 1] = heartbeat_color;
        } else {
            leds[UI_NUM_LEDS - 1] = CRGB::Black;
        }
    }

    FastLED.show();
}