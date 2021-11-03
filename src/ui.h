#ifndef UI_H
#define UI_H

#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <Audio.h>
#include "Scale.h"

#define UI_STATES_TOTAL 4

class UI {
    public:
        UI(Adafruit_SSD1306* display) {
            this->display = display;
        }

        //
        // static
        //

        static const int UI_STATE_MAIN = 0;
        static const int UI_STATE_TUNER = 1;
        static const int UI_STATE_TUNER_EDO = 2;
        static const int UI_STATE_TUNER_ROOT = 3;

        //
        // public vars
        //

        const int CURSOR_LIMITS[UI_STATES_TOTAL] = {4, 3, 0, 0};
        int ticks;
        int ticksLastChange;

        //
        // refs
        //

        volatile int16_t* refTunerOscopeTable;
        float* refTunerFreq;
        bool* refTunerFreqFound;
        float* refTunerPeak;
        Scale* refScale;
        int* refExp1Value;
        int* refExp2Value;

        //
        // methods
        //

        void setState(int newState, bool back = false);
        void setCursor(int cursor);
        int getCursor();
        void onPress();
        void update(int ticks);

    private:
        Adafruit_SSD1306* display;
        int cursor = 0;
        int cursorOffsets[UI_STATES_TOTAL];
        int lastCursors[UI_STATES_TOTAL];
        int activeState = 0;
};

#endif
