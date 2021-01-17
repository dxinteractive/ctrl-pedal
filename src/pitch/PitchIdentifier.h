#ifndef PITCHIDENTIFIER_H
#define PITCHIDENTIFIER_H

#include "./Scale.h"

class PitchIdentifier {
    public:
        PitchIdentifier(Scale* scale) { _scale = scale; }

        Scale* scale() const { return _scale; }
        void identify(float freq);
        float pitch() const { return _pitch; }
        int step() const { return _step; }
        int pitchToOctave(int pitch);
        char const* pitchToNote(int pitch);

    private:
        Scale* _scale;
        float _pitch;
        int _step;
};

#endif
