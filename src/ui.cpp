#include "ui.h"
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <Audio.h>
#include "Scale.h"

typedef void (*renderFunction)(UI* ui, Adafruit_SSD1306* d);
typedef void (*pressFunction)(UI* ui);

// UI_STATE_MAIN

void printScope(UI* ui, Adafruit_SSD1306* d, bool dashed) {
  if(ui->refTunerOscopeTable) {
    int inc = dashed ? 3 : 1;
    for(int i = 0; i < 128; i += inc) {
      int py = 32 - (ui->refTunerOscopeTable[127 - i] >> 10);
      if(py > -1 && py < 64) {
        d->drawPixel(i, py, WHITE);
      }
    }
  }
}

void printArrows(UI* ui, Adafruit_SSD1306* d, int x) {
  int y1 = 10;
  d->drawPixel(x, y1, WHITE);
  d->drawPixel(x + 1, y1 + 1, WHITE);
  d->drawPixel(x + 2, y1 + 2, WHITE);
  d->drawPixel(x - 1, y1 + 1, WHITE);
  d->drawPixel(x - 2, y1 + 2, WHITE);

  int y2 = 64 - 10;
  d->drawPixel(x, y2, WHITE);
  d->drawPixel(x + 1, y2 - 1, WHITE);
  d->drawPixel(x + 2, y2 - 2, WHITE);
  d->drawPixel(x - 1, y2 - 1, WHITE);
  d->drawPixel(x - 2, y2 - 2, WHITE);
}

void printTitle(UI* ui, Adafruit_SSD1306* d, const char* title) {
  d->setTextSize(2);
  int x = (128 - (strlen(title) * 12)) * 0.5;
  int y = (64 - 14) * 0.5;
  d->setTextColor(BLACK);
  d->setCursor(x - 2, y);
  d->println(title);
  d->setCursor(x + 2, y);
  d->println(title);
  d->setCursor(x, y - 2);
  d->println(title);
  d->setCursor(x, y + 2);
  d->println(title);
  d->setTextColor(WHITE);
  d->setCursor(x,y);
  d->println(title);
}

void printOption(UI* ui, Adafruit_SSD1306* d, const char* label) {
  printTitle(ui, d, label);
  printArrows(ui, d, 64);
}

void printOption(UI* ui, Adafruit_SSD1306* d, const char* label, int value, int arrowsX = 64) {
  char str[16];
  sprintf(str, "%s:%d", label, value);
  printTitle(ui, d, str);
  printArrows(ui, d, arrowsX);
}

void printDebug(UI* ui, Adafruit_SSD1306* d) {
  d->print("Ticks: ");
  d->println(ui->ticks);

  d->print("CPU: ");
  d->print(AudioProcessorUsageMax());
  d->println("%");

  d->print("Freq: ");
  d->print(*ui->refTunerFreq);
  d->println(" Hz");

  d->print("Peak: ");
  d->println(*ui->refTunerPeak);

  d->print("Exp1: ");
  d->println(*ui->refExp1Value);

  d->print("Exp2: ");
  d->println(*ui->refExp2Value);
}

void renderMain(UI* ui, Adafruit_SSD1306* d) {
  int cursor = ui->getCursor();
  bool intro = ui->ticksLastChange + 1000 > ui->ticks;

  if(cursor == 0) {
    printScope(ui, d, false);
    if(intro) printTitle(ui, d, "SCOPE");
    return;
  }

  if(cursor == 1) {
    printScope(ui, d, !*ui->refTunerFreqFound);

    if(intro) {
      printTitle(ui, d, "TUNER");
    } else {
      char str[10];
      sprintf(str, "%d\\%d", (*ui->refScale).nearestStep, (*ui->refScale).edo);
      printTitle(ui, d, str);
    }
    return;
  }

  if(cursor == 2) {
    if(intro) printTitle(ui, d, "SLEEP");
    return;
  }

  if(cursor == 3) {
    printDebug(ui, d);
    if(intro) {
      printTitle(ui, d, "DEBUG");
    }
    return;
  }
}

void pressMain(UI* ui) {
  int cursor = ui->getCursor();
  if(cursor == 1) {
    ui->setState(UI::UI_STATE_TUNER);
    return;
  }
}

// UI_STATE_TUNER



void renderTuner(UI* ui, Adafruit_SSD1306* d) {
  int cursor = ui->getCursor();
  printScope(ui, d, true);

  if(cursor == 0) {
    printOption(ui, d, "EDO", (*ui->refScale).edo);
    return;
  }

  if(cursor == 1) {
    printOption(ui, d, "ROOT", (*ui->refScale).rootHz);
    return;
  }

  if(cursor == 2) {
    printOption(ui, d, "DONE");
    return;
  }
}

void pressTuner(UI* ui) {
  int cursor = ui->getCursor();
  if(cursor == 0) {
    ui->setState(UI::UI_STATE_TUNER_EDO);
    return;
  }

  if(cursor == 1) {
    ui->setState(UI::UI_STATE_TUNER_ROOT);
    return;
  }

  ui->setState(UI::UI_STATE_MAIN, true);
}

void pressBackToTuner(UI* ui) {
  ui->setState(UI::UI_STATE_TUNER, true);
}

// UI_STATE_TUNER_EDO

void renderTunerEdo(UI* ui, Adafruit_SSD1306* d) {
  printScope(ui, d, true);
  printOption(ui, d, "EDO", (*ui->refScale).edo, 87);
}

// UI_STATE_TUNER_ROOT

void renderTunerRoot(UI* ui, Adafruit_SSD1306* d) {
  printScope(ui, d, true);
  printOption(ui, d, "ROOT", (*ui->refScale).rootHz, 93);
}

// methods

void UI::setState(int newState, bool back) {
  if(newState != this->activeState) {
    this->activeState = newState;
    this->cursorOffsets[this->activeState] = this->cursor;
    this->ticksLastChange = this->ticks;
    if(back) {
      this->cursorOffsets[this->activeState] -= this->lastCursors[this->activeState];
    }
  }
}

int UI::getCursor() {
  int cursor = this->cursor - this->cursorOffsets[this->activeState];
  int limit = CURSOR_LIMITS[this->activeState];
  if(limit != 0) {
    while(cursor < 0) {
      cursor += limit;
    }
    cursor = cursor % limit;
  }
  return cursor;
}

void UI::setCursor(int cursor) {
  if(cursor == this->cursor) return;

  int state = this->activeState;
  int delta = cursor - this->cursor;
  this->cursor = cursor;
  this->ticksLastChange = this->ticks;
  this->lastCursors[state] = this->getCursor();

  if(state == UI::UI_STATE_TUNER_EDO) {
    (*this->refScale).setEdo((*this->refScale).edo + delta);

  } else if(state == UI::UI_STATE_TUNER_ROOT) {
    (*this->refScale).setRoot((*this->refScale).rootHz + delta);
  }
}

void UI::onPress() {
  static pressFunction pressers[] = {
    pressMain,
    pressTuner,
    pressBackToTuner,
    pressBackToTuner
  };
  int s = this->activeState;
  if(s > -1 && s < 4) pressers[s](this);
}

void UI::update(int ticks) {
  this->ticks = ticks;

  Adafruit_SSD1306* d = this->display;
  d->clearDisplay();
  d->setTextColor(INVERSE);
  d->setCursor(0,0);
  d->setTextSize(1);

  static renderFunction renderers[] = {
    renderMain,
    renderTuner,
    renderTunerEdo,
    renderTunerRoot
  };
  int s = this->activeState;
  if(s > -1 && s < 4) renderers[s](this, d);
  d->display();
}
