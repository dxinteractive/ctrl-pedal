#include "PitchIdentifier.h"

void PitchIdentifier::identify(float freq) {
    const float* frequencies = _scale->frequencies();
    float first = frequencies[0];
    int steps = _scale->steps();

    int octaveShift = 5;
    while(freq < first) {
        freq *= 2.0;
        octaveShift--;
    }
    while(freq >= first * 2.0) {
        freq *= 0.5;
        octaveShift++;
    }

    int step = steps - 1;
    while(freq < frequencies[step]) step--;

    float lower = frequencies[step];
    float higher = frequencies[step + 1];
    float remainder = (freq - lower) / (higher - lower);

    _pitch = ((float)step) + remainder;

    if(remainder > 0.5) {
        step++;
        remainder -= 1.0;
    }

    _step = step;
    _octave = octaveShift;
    _centsOffset = (int)(remainder * 100.0);
    _note = _scale->names()[_step];
}
