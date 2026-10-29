#pragma once

#include "eyes/eyes.h"

// Enable the eye(s) you want to #include -- these are large graphics tables for various eyes:
#include "eyes/240x240/brown.h"

#include "eyes/EyeController.h"

#include "displays/GC9A01A_Display.h"

// A list of all the different eye definitions we want to use
EyeDefinition eyeDefinition{brown::eye};

// DISPLAY HARDWARE SETTINGS (screen type & connections) -------------------

// CS, DC, MOSI, SCK, reset, rotation, mirror for the connected left display.
GC9A01A_Config eyeInfo[] = {
  // CS DC MOSI SCK RST ROT MIRROR
  {5, 27, 23, 18, 25, 0, true},
};

/// Start conservatively; increase only after confirming stable output on both displays.
constexpr uint32_t SPI_SPEED{40'000'000};

EyeController<1, GC9A01A_Display> *eyes{};

void initEyes(bool autoMove, bool autoBlink, bool autoPupils) {
  // Create the displays and eye controller
  auto display = new GC9A01A_Display(eyeInfo[0], SPI_SPEED);
  const DisplayDefinition<GC9A01A_Display> left{display, eyeDefinition};
  eyes = new EyeController<1, GC9A01A_Display>({left}, autoMove, autoBlink, autoPupils);
}
