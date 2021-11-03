#ifndef SCALE_H
#define SCALE_H

class Scale {
  public:
    Scale() {
      updateScaleTable();
    }

    static const int PITCHES_TOTAL = 54;

    int edo = 12;
    float rootHz = 440.0;

    int nearestStep = 0;
    int nearestHz = 0;

    void setEdo(int edo) {
      if(edo < 1) {
        edo = 1;
      } else if(edo > PITCHES_TOTAL - 2) {
        edo = PITCHES_TOTAL - 1;
      }
      this->edo = edo;
      updateScaleTable();
    }

    void setRoot(int hz) {
      if(hz < 220) {
        hz = 220;
      } else if(hz > 660) {
        hz = 660;
      }
      this->rootHz = hz;
      updateScaleTable();
    }

    void updateScaleTable() {
      for(int i = 0; i < this->edo + 1; i++) {
        float hz = this->rootHz * pow(2, i * 1.0 / this->edo);
        float maxHz = this->rootHz * pow(2, ((i * 2.0) + 1.0) / (this->edo * 2.0));
        this->pitches[i] = hz;
        this->maxPitches[i] = maxHz;
      }
    }

    void calculateNearestPitch(float hz) {
      float octaveReducedHz = hz;
      for(int i = 0; i < 5; i++) {
        if(octaveReducedHz < this->rootHz) {
          octaveReducedHz *= 2.0;
        }
        if(octaveReducedHz >= this->rootHz * 2.0) {
          octaveReducedHz *= 0.5;
        }
      }

      this->nearestStep = 0;
      for(int i = 0; i < this->edo; i++) {
        float maxHz = this->maxPitches[i];
        if(octaveReducedHz < maxHz) {
          this->nearestStep = i % this->edo;
          break;
        }
      }

      float nearestHz = this->pitches[this->nearestStep];
      for(int i = 0; i < 5; i++) {
        if(nearestHz > hz * 1.5) {
          nearestHz *= 0.5;
        }
        if(nearestHz < hz * 0.75) {
          nearestHz *= 2.0;
        }
      }
      this->nearestHz = nearestHz;
    }

  private:
    float pitches[Scale::PITCHES_TOTAL];
    float maxPitches[Scale::PITCHES_TOTAL];
};

#endif
