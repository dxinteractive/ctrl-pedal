#ifndef AUDIO_OSCILLOSCOPE_H
#define AUDIO_OSCILLOSCOPE_H

#include <Audio.h>

const int OSCOPE_TABLE_SIZE = 128;
const float OSCOPE_BASE_HZ = AUDIO_SAMPLE_RATE / OSCOPE_TABLE_SIZE;

class AudioOscilloscope : public AudioStream {
  public:
    AudioOscilloscope(void): AudioStream(1, inputQueueArray) {}

    volatile int16_t table[OSCOPE_TABLE_SIZE];

    void update(void) {
      audio_block_t *block;
      block = receiveReadOnly();
      if (!block) return;

      while(readIndex < OSCOPE_TABLE_SIZE) {
        table[writeIndex] = block->data[(int)readIndex];
        readIndex += readRate;
        writeIndex++;
        if(writeIndex >= OSCOPE_TABLE_SIZE) {
          writeIndex = 0;
        }
      }
      readIndex -= OSCOPE_TABLE_SIZE;

      transmit(block);
      release(block);
    }

    void setRate(float freq) {
      if(freq > 0.0) {
        float newReadRate = fmod(OSCOPE_BASE_HZ / freq * 2.0, OSCOPE_TABLE_SIZE);
        __disable_irq();
        readRate = newReadRate;
        __enable_irq();
      }
    }

  private:
    audio_block_t *inputQueueArray[1];
    int writeIndex = 0;
    float readIndex = 0.0;
    float readRate = 1.0;
};

# endif
