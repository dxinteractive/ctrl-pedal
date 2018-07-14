#ifndef SCALE_12TET_H
#define SCALE_12TET_H

#include "./Scale.h"

class Scale12TET : public Scale {
    public:
        static float const* allFrequencies() {
            static float arr[] = {
                440.0,
                466.2,
                493.9,
                523.3,
                554.4,
                587.3,
                622.3,
                659.3,
                698.5,
                740.0,
                784.0,
                830.6,
                880.0
            };
            return arr;
        }
        static char const** allNames() {
            static char const* arr[] = {
                "A",
                "A#",
                "B",
                "C",
                "C#",
                "D",
                "D#",
                "E",
                "F",
                "F#",
                "G",
                "G#",
                "A"
            };
            return arr;
        }

        virtual int steps() {
            return 12;
        }

        virtual float const* frequencies() {
            return Scale12TET::allFrequencies();
        }

        virtual char const** names() {
            return Scale12TET::allNames();
        }
};

#endif
