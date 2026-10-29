#pragma once

#include <Adafruit_GC9A01A.h>

#include "Display.h"

typedef struct {
  int8_t cs;            // Chip select pin.
  int8_t dc;            // DC pin.
  int8_t mosi;          // MOSI pin.
  int8_t sck;           // SCK pin.
  int8_t rst;           // reset pin, or -1 to disable reset.
  uint8_t rotation;     // The rotation value for the display (0-3).
  bool mirror;          // Mirror the display in the X direction.
} GC9A01A_Config;

class GC9A01A_Display : public Display<GC9A01A_Display> {
private:
  Adafruit_GC9A01A* display;
  uint16_t* pixels{};
  uint8_t* compactPixels{};
  int16_t dirtyLeft{240};
  int16_t dirtyTop{240};
  int16_t dirtyRight{-1};
  int16_t dirtyBottom{-1};
  int displayNum;

public:
  /// Creates a generic wrapper for a 240x240 GC9A01A round display screen.
  /// \param config the screen's configuration.
  /// \param spiSpeed the speed of the SPI bus. For maximum performance, set this as high as you can get
  /// away with. It will depend on the displays themselves, wire lengths, shielding/interference etc. My
  /// setup works up to about 90,000,000. At 100,000,000 I start seeing corruption on the displays.
  GC9A01A_Display(const GC9A01A_Config &config, const uint32_t spiSpeed = 30'000'000);

  virtual ~GC9A01A_Display();

  void drawPixel(int16_t x, int16_t y, uint16_t color565);

  void drawFastVLine(int16_t x, int16_t y, int16_t height, uint16_t color565);

  void update();

  bool isAvailable() const;

private:
  void markDirty(int16_t x, int16_t y);
};
