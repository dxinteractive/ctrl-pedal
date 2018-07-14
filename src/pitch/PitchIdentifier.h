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
        int octave() const { return _octave; }
        int centsOffset() const { return _centsOffset; }
        char const* note() const { return _note; }
        float log() { return _log; }

    private:
        Scale* _scale;
        float _pitch;
        int _step;
        int _octave;
        int _centsOffset;
        char const* _note;
        float _log;
};

#endif
