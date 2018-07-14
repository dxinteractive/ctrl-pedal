#ifndef SCALE_H
#define SCALE_H

class Scale {
    public:
        virtual int steps() = 0;
        virtual float const* frequencies() = 0;
        virtual char const** names() = 0;
};

#endif
