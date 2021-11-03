#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Audio.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Encoder.h>
#include "ui.h"
#include "AudioOscilloscope.h"
#include "Scale.h"

//
// audio patching
//

AudioInputI2S i2sInput;
AudioAnalyzeNoteFrequency notefreq;
AudioOscilloscope oscope;

// https://forum.pjrc.com/threads/32252-Different-Range-FFT-Algorithm/page4
AudioAnalyzePeak peakAnalyzer;
AudioConnection patchCord1(i2sInput, 1, notefreq, 0);
AudioConnection patchCord2(i2sInput, 1, peakAnalyzer, 0);
AudioConnection patchCord3(i2sInput, 1, oscope, 0);

// AudioSynthToneSweep tonesweep1;
// AudioOutputI2S i2sOuptut;
// AudioConnection patchCord4(tonesweep1, 0, i2sOuptut, 0);
// AudioConnection patchCord5(tonesweep1, 0, i2sOuptut, 1);

AudioControlSGTL5000 audioAdaptor;

//
// consts
//

#define PIN_OLED_DC        0
#define PIN_OLED_RESET     1
#define PIN_OLED_CS        2
#define PIN_CV_CS          3
#define PIN_ROTARY_1       4
#define PIN_ROTARY_2       5
#define PIN_ROTARY_BUTTON  6
#define PIN_BYPASS_SWITCH  10
#define PIN_SR_CS          8
#define PIN_TAP_BUTTON     17
#define PIN_LED_1          21
#define PIN_LED_2          20
#define EXP_1              A2
#define EXP_2              A1

#define SEND_ROTARY_VALUE  0
#define SEND_ROTARY_BUTTON 1
#define SEND_TAP_BUTTON    2
#define SEND_BYPASS_SWITCH 3
#define SEND_EXP_1         4
#define SEND_EXP_2         5
#define SEND_TOTAL         6

#define SET_CV             0
#define SET_AUDIO_ROUTE    1
#define SET_LED            2
#define SET_OLED           3 // not used yet
#define SET_BB_OLED        7 // not used yet
#define SET_BB_LED_UPPER   8 // not used yet
#define SET_BB_LED_LOWER   9

#define SERIAL_RECEIVE_MAX 10

#define BB_I2C_CHANNEL 8

static const int CVS[] = {5, 6, 8, 7, 1, 4, 2, 3};

static const int LEDS[] = {PIN_LED_1, PIN_LED_2};

static const int LED_BRIGHTNESS_TABLE[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
  0x02, 0x02, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x04, 0x04, 0x04, 0x04, 0x04, 0x05, 0x05, 0x05,
  0x05, 0x06, 0x06, 0x06, 0x07, 0x07, 0x07, 0x08, 0x08, 0x08, 0x09, 0x09, 0x0A, 0x0A, 0x0B, 0x0B,
  0x0C, 0x0C, 0x0D, 0x0D, 0x0E, 0x0F, 0x0F, 0x10, 0x11, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
  0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1F, 0x20, 0x21, 0x23, 0x24, 0x26, 0x27, 0x29, 0x2B, 0x2C,
  0x2E, 0x30, 0x32, 0x34, 0x36, 0x38, 0x3A, 0x3C, 0x3E, 0x40, 0x43, 0x45, 0x47, 0x4A, 0x4C, 0x4F,
  0x51, 0x54, 0x57, 0x59, 0x5C, 0x5F, 0x62, 0x64, 0x67, 0x6A, 0x6D, 0x70, 0x73, 0x76, 0x79, 0x7C,
  0x7F, 0x82, 0x85, 0x88, 0x8B, 0x8E, 0x91, 0x94, 0x97, 0x9A, 0x9C, 0x9F, 0xA2, 0xA5, 0xA7, 0xAA,
  0xAD, 0xAF, 0xB2, 0xB4, 0xB7, 0xB9, 0xBB, 0xBE, 0xC0, 0xC2, 0xC4, 0xC6, 0xC8, 0xCA, 0xCC, 0xCE,
  0xD0, 0xD2, 0xD3, 0xD5, 0xD7, 0xD8, 0xDA, 0xDB, 0xDD, 0xDE, 0xDF, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5,
  0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xED, 0xEE, 0xEF, 0xEF, 0xF0, 0xF1, 0xF1, 0xF2,
  0xF2, 0xF3, 0xF3, 0xF4, 0xF4, 0xF5, 0xF5, 0xF6, 0xF6, 0xF6, 0xF7, 0xF7, 0xF7, 0xF8, 0xF8, 0xF8,
  0xF9, 0xF9, 0xF9, 0xF9, 0xFA, 0xFA, 0xFA, 0xFA, 0xFA, 0xFB, 0xFB, 0xFB, 0xFB, 0xFB, 0xFB, 0xFC,
  0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xFD, 0xFD, 0xFD, 0xFD, 0xFD, 0xFD, 0xFD, 0xFD,
  0xFD, 0xFD, 0xFD, 0xFD, 0xFD, 0xFD, 0xFD, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFF, 0xFF
};

//
// scale
//

Scale scale;

//
// peripherals and UI
//

Adafruit_SSD1306 display(PIN_OLED_DC, PIN_OLED_RESET, PIN_OLED_CS);

Encoder rotary(PIN_ROTARY_1, PIN_ROTARY_2);

UI ui(&display);

//
// state
//

float peak = 0;
float freq = 0;
bool freqFound = false;
int exp1Value;
int exp2Value;

//
// CV
//

void setCV(long chn, long val) {
  int h = (chn << 4) | (val >> 6);
  int l = (val << 2);
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
  digitalWrite(PIN_CV_CS, LOW);
  SPI.transfer(h);
  SPI.transfer(l);
  SPI.endTransaction();
  digitalWrite(PIN_CV_CS, HIGH);
  digitalWrite(PIN_CV_CS, LOW);
}

//
// Shift register (tap + relay)
//

boolean audioOutputPath = 0; // default to off
int activeTaps = 63; // all taps
int shiftRegisterState = activeTaps;

void syncSR() {
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
  digitalWrite(PIN_SR_CS,LOW);
  SPI.transfer(shiftRegisterState);
  SPI.endTransaction();
  digitalWrite(PIN_SR_CS,HIGH);
  digitalWrite(PIN_SR_CS,LOW);
}

// void setSR(int id, boolean value) {
//   if(value) {
//     activeTaps |= (1u << id);
//   } else {
//     activeTaps &= ~(1u << id);
//   }
// }

void setAudioRoute(boolean value) {
  if(value) {
    shiftRegisterState |= 128;
  } else {
    shiftRegisterState &= 127;
  }
  syncSR();
}

//
// LEDs
//

void setLed(int pin, int brightness) {
  analogWrite(pin, LED_BRIGHTNESS_TABLE[brightness]);
}

//
// Button board
//

void sendButtonBoardMessage(const char * data, int len) {
  Wire.beginTransmission(BB_I2C_CHANNEL);
  Wire.write(data, len);
  Wire.endTransmission();
}

//
// Serial send
//

int serialSendMemory[SEND_TOTAL] = {};

void serialSend(int id, int value) {
  if(value == serialSendMemory[id]) return;
  Serial.print(id);
  Serial.print(':');
  Serial.println(value);
  serialSendMemory[id] = value;
}

//
// Serial receive
//
// e.g. 00:1023 - set CV (0) wizard (4) to 5V (1023)
// e.g. 10:1    - toggle audio from teensy
// e.g. 21:128  - set LED (2) number [1] to 128

void serialReceive(const char * data, int len) {
  const char * valueString = data + 3;
  int target = data[0] - '0';
  int subtarget = data[1] - '0';

  switch(target) {
    case SET_CV:
      setCV(CVS[subtarget], atoi(valueString));
      break;

    case SET_AUDIO_ROUTE:
      setAudioRoute(atoi(valueString));
      break;

    case SET_LED:
      setLed(LEDS[subtarget], atoi(valueString));
      break;

    case SET_BB_LED_UPPER:
    case SET_BB_LED_LOWER:
    case SET_BB_OLED:
      sendButtonBoardMessage(data, len);
      break;

    default:
      break;
  }
}

void serialReceiveByte(const byte inByte) {
  static char inputLine[SERIAL_RECEIVE_MAX];
  static unsigned int inputPos = 0;

  switch(inByte) {
    case '\n':
      inputLine[inputPos] = 0;
      serialReceive(inputLine, inputPos + 1);
      inputPos = 0;
      break;

    case '\r':
      break;

    default:
      if(inputPos < (SERIAL_RECEIVE_MAX - 1))
        inputLine[inputPos++] = inByte;
      break;
  }
}

//
// tuner
//


// #define TUNER_BUCKETS_TOTAL 400;
// float tunerBuckets[TUNER_BUCKETS_TOTAL];

// #define TUNER_SCALE_STEPS_MAX 100;
// float tunerScaleStepNames[TUNER_SCALE_STEPS_MAX];
// int tunerScaleSize = 12;
// int tunerScaleRootHz = 440;

// void tunerSetEdo(int edo) {
//   tunerScaleSize = edo;

//   float halfStepRatio = exp2(0.5 / edo);
//   float fullStepRatio = halfStepRatio * halfStepRatio;
//   float currentRatio = 1.0;
//   float nextStepRatio = halfStepRatio;

//   for(int i = 0; i < TUNER_BUCKETS_TOTAL; i++) {
//     if(currentRatio > nextStepRatio) {
//       currentRatio *= fullStepRatio;
//       nextStepRatio *= fullStepRatio;
//     }
//     tunerBuckets[i] = tunerScaleRootHz * currentRatio;
//   }

//   for(int i = 0; i < edo; i++) {
//     tunerScaleStepNames[i] = i;
//   }
// }

// float tunerGetTargetHz(float hz) {
//   while(hz > tunerScaleRootHz * 2.0) {
//     hz *= 0.5;
//   }
//   while(hz < tunerScaleRootHz) {
//     hz *= 2.0;
//   }
//   tunerBuckets[];
// }

//
// Setup
//

void setup() {
  // SPI
  SPI.setMOSI(7);
  SPI.setSCK(14);

  // SPI select
  pinMode(PIN_CV_CS, OUTPUT);
  pinMode(PIN_SR_CS, OUTPUT);
  digitalWrite(PIN_CV_CS, HIGH);
  digitalWrite(PIN_SR_CS, HIGH);

  // LEDs
  pinMode(PIN_LED_1, OUTPUT);
  pinMode(PIN_LED_2, OUTPUT);
  analogWriteFrequency(PIN_LED_1, 30000.0);
  analogWriteFrequency(PIN_LED_2, 30000.0);
  analogWrite(PIN_LED_1, 0);
  analogWrite(PIN_LED_2, 0);

  // buttons
  pinMode(PIN_ROTARY_BUTTON, INPUT_PULLUP);
  pinMode(PIN_TAP_BUTTON, INPUT_PULLUP);
  pinMode(PIN_BYPASS_SWITCH, INPUT_PULLUP);

  // audio
  AudioMemory(90);
  notefreq.begin(0.99);

  audioAdaptor.enable();
  audioAdaptor.inputSelect(AUDIO_INPUT_LINEIN);
  audioAdaptor.micGain(0);
  audioAdaptor.lineInLevel(12);
  audioAdaptor.lineOutLevel(31);
  audioAdaptor.dacVolume(0.9);
  audioAdaptor.muteHeadphone();
  audioAdaptor.adcHighPassFilterDisable();

  // serial
  Serial.begin(115200);
  serialReceiveByte('\n');

  // i2c
  Wire.begin();

  // display
  display.begin(SSD1306_SWITCHCAPVCC);
  display.clearDisplay();
  display.display();

  // ui refs
  ui.refTunerOscopeTable = oscope.table;
  ui.refTunerFreq = &freq;
  ui.refTunerFreqFound = &freqFound;
  ui.refTunerPeak = &peak;
  ui.refScale = &scale;
  ui.refExp1Value = &exp1Value;
  ui.refExp2Value = &exp2Value;

  // route
  setAudioRoute(false);

  // initial CV
  for(int i = 0; i < 8; i++) {
    setCV(CVS[i], 1023);
  }

  // test tone
  // tonesweep1.play(1.0, 500.0, 1000.0, 100.0);
}

unsigned long ticks;
bool rotaryButtonPressedPrev;
int rotaryValue;

//
// loop
//

void loop() {
  ticks += 3;

  while(Serial.available () > 0) {
    serialReceiveByte(Serial.read());
  }

  // once per 8ms = ~125Hz
  if(ticks % 8 == 0) {

    // rotary button pressed
    bool rotaryButtonPressed = digitalRead(PIN_ROTARY_BUTTON) == LOW;
    if(!rotaryButtonPressedPrev && rotaryButtonPressed) {
        ui.onPress();
    } else {
        // rotary position
        int rotaryRead = rotary.read();
        if(rotaryRead < 0) {
            rotaryValue = -((-rotaryRead + 2) / 4);
        } else {
            rotaryValue = (rotaryRead + 1) / 4;
        }
        ui.setCursor(rotaryValue);
    }
    rotaryButtonPressedPrev = rotaryButtonPressed;

    // tap button
    bool tap = digitalRead(PIN_TAP_BUTTON) == LOW;
    if(tap) {
      setLed(PIN_LED_2, 96);
    } else {
      setLed(PIN_LED_2, 0);
    }

    // send serial values
    serialSend(SEND_ROTARY_VALUE, rotaryValue);
    serialSend(SEND_ROTARY_BUTTON, rotaryButtonPressed);
    serialSend(SEND_BYPASS_SWITCH, digitalRead(PIN_BYPASS_SWITCH) == LOW);
    serialSend(SEND_TAP_BUTTON, tap);

    setLed(PIN_LED_1, digitalRead(PIN_BYPASS_SWITCH) == LOW ? 0 : 128);
  }

  // once per 8ms = ~125fps
  if(ticks % 8 == 0) {
    ui.update(ticks);
  }

  // once per 32ms = ~8Hz
  if(ticks % 32 == 0) {
    analogRead(EXP_1);
    exp1Value = analogRead(EXP_1);
    serialSend(SEND_EXP_1, exp1Value);
  }

  // once per 32ms = ~8Hz
  if((ticks + 16) % 32 == 0) {
    analogRead(EXP_2);
    exp2Value = analogRead(EXP_2);
    serialSend(SEND_EXP_2, exp2Value);

    // setCV(CVS[0], read); // dmm
    // setCV(CVS[1], read); // pitchfork
    // setCV(CVS[2], read);
    // setCV(CVS[3], read);
    // setCV(CVS[4], read); // wizard
    // setCV(CVS[5], read); // bigsky
    // setCV(CVS[4], read);
    // setCV(CVS[5], read);
    // setCV(CVS[6], read);
    // setCV(CVS[7], read);
  }

  // once per 4ms = ~250Hz
  if(ticks % 4 == 0) {
    if(peakAnalyzer.available()) {
      peak = peakAnalyzer.read();
    }
  }

  // once per 1024ms = ~1Hz
  if(ticks % 1024 == 0) {
    Serial.print("wire got ");
    Wire.requestFrom(BB_I2C_CHANNEL, 6);
    while (Wire.available()) {
      char c = Wire.read();
      Serial.print(c);
    }
    Serial.println(".");
  }

  // once per 4ms = ~250Hz
  if((ticks + 2) % 4 == 0) {
    freqFound = peak > 0.05 && notefreq.probability() > 0.8;
    if(notefreq.available() && freqFound) {
      freq = notefreq.read();
      scale.calculateNearestPitch(freq);
      oscope.setRate(scale.nearestHz);
    }
  }
}
