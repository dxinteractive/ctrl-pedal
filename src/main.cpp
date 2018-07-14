#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Audio.h>
#include <Encoder.h>

#include "pitch/Scale12TET.h"
#include "pitch/PitchIdentifier.h"

AudioInputI2S i2s1;
AudioAnalyzeNoteFrequency notefreq; // AUDIO_GUITARTUNER_BLOCKS = 9
AudioAnalyzePeak peak;
AudioConnection patchCord1(i2s1, 1, notefreq, 0);
AudioConnection patchCord2(i2s1, 1, peak, 0);

AudioControlSGTL5000 audioAdaptor;

#define PIN_OLED_DC        0
#define PIN_OLED_RESET     1
#define PIN_OLED_CS        2
#define PIN_CV_CS          3
#define PIN_ROTARY_1       4
#define PIN_ROTARY_2       5
#define PIN_BUTTON_ROTARY  6
#define PIN_SWITCH_BYPASS  10
#define PIN_SR_CS          8
#define PIN_BUTTON_TAP     17
#define PIN_LED_1          20
#define PIN_LED_2          21

Adafruit_SSD1306 display(PIN_OLED_DC, PIN_OLED_RESET, PIN_OLED_CS);

// Encoder rotary(PIN_ROTARY_1, PIN_ROTARY_2);

// void setCV(long chn, long val)
// {
//   int h = chn << 4 | val >> 6;
//   int l = val << 2;
//   digitalWrite(PIN_CV_CS, LOW);
//   SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));
//   SPI.transfer(h);
//   SPI.transfer(l);
//   SPI.endTransaction();
//   digitalWrite(PIN_CV_CS, HIGH);
// }

// void setSR(int value)
// {
//   digitalWrite(PIN_SR_CS,LOW);
//   SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));
//   SPI.transfer(value);
//   SPI.endTransaction();
//   digitalWrite(PIN_SR_CS,HIGH);
// }

// tuner
Scale12TET scale12TET;
PitchIdentifier pitchIdentifier(&scale12TET);

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
    digitalWrite(PIN_LED_1, LOW);
    digitalWrite(PIN_LED_2, LOW);

    // buttons
    pinMode(PIN_BUTTON_ROTARY, INPUT);
    pinMode(PIN_BUTTON_TAP, INPUT);
    pinMode(PIN_SWITCH_BYPASS, INPUT_PULLUP);

    // audio
    AudioMemory(90);

    audioAdaptor.enable();
    audioAdaptor.inputSelect(AUDIO_INPUT_LINEIN);
    audioAdaptor.micGain(0);
    audioAdaptor.lineInLevel(12);
    audioAdaptor.lineOutLevel(31);
    audioAdaptor.dacVolume(0.9);
    audioAdaptor.muteHeadphone();
    audioAdaptor.adcHighPassFilterDisable();

    notefreq.begin(0.15);

    // display
    display.begin(SSD1306_SWITCHCAPVCC);
    display.display();
}

// int cv = 0;
// int cvDir = 1;
int count = 0;
// int delayTime = 10;

void loop() {
    count++;

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.println("Hello");
    // display.println(count);

    // display.println(rotary.read());
    // display.setCursor(0,45);

    // int delayMulti = 4;
    // if(digitalRead(PIN_BUTTON_ROTARY) == LOW) {
    //   display.println("Press!");
    //   delayMulti = 1;
    // }
    // if(digitalRead(PIN_SWITCH_BYPASS) == LOW) {
    //   display.println("Bypass!");
    //   delayMulti = 1;
    // }
    // if(digitalRead(PIN_BUTTON_TAP) == LOW) {
    //   display.println("Tap!");
    //   delayMulti = 1;
    // }



    // cv += (16 * cvDir);
    // if(cv > 1023)
    // {
    //   cvDir = -1;
    //   cv = 1023;
    // } else if(cv < 0) {
    //   cvDir = 1;
    //   cv = 0;
    // }

    // for(int j = 0; j < 8; j++)
    //   setCV(j, cv);

    // setSR(count >> 5);

    // analogWrite(PIN_LED_1, (count << 2) & 255);
    // analogWrite(PIN_LED_2, 255 - ((count << 2) & 255));

    // delay(delayTime * delayMulti);

    // if(peak.available()) {
    //     display.println(peak.read());
    // }

    if(notefreq.available()) {
        float freq = notefreq.read();
        pitchIdentifier.identify(freq);
        display.println(freq);
        display.println(pitchIdentifier.pitch());
        display.setTextSize(3);
        display.println(pitchIdentifier.note());
        display.setTextSize(1);
        display.println(pitchIdentifier.octave());
        display.println(pitchIdentifier.centsOffset());
    }

    // int cpuusage = AudioProcessorUsageMax();
    // display.print("CPU: ");
    // display.println(cpuusage);

    display.display();
    delay(25);
}
