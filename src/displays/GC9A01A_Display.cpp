#include "GC9A01A_Display.h"
#include <algorithm>
#include <array>
#include <cstdlib>

Adafruit_GC9A01A *createDisplay(const GC9A01A_Config &config) {
  if (config.cs >= 0) {
    pinMode(config.cs, OUTPUT);
    digitalWrite(config.cs, HIGH);
  }
  return new Adafruit_GC9A01A(config.cs, config.dc, config.rst);
}

GC9A01A_Display::GC9A01A_Display(const GC9A01A_Config &config, uint32_t spiSpeed) :
  display(createDisplay(config)),
  pixels(static_cast<uint16_t *>(malloc(240 * 240 * sizeof(uint16_t)))) {
  static size_t displayNum{};
  Serial.print(F("Init GC9A01A display #"));
  Serial.print(displayNum);
  Serial.print(F(": rotate="));
  Serial.print(config.rotation);
  Serial.print(F(", mirror="));
  Serial.println(config.mirror);
  display->begin(spiSpeed);
  display->setRotation(config.rotation);
  if (config.mirror) {
    const std::array<uint8_t, 4> mirrorTFT{0x08, 0x20, 0x40, 0xE0};
    display->sendCommand(GC9A01A_MADCTL, &mirrorTFT.at(config.rotation & 3), 1);
  }

  if (pixels == nullptr) {
    compactPixels = static_cast<uint8_t *>(malloc(240 * 240));
    if (compactPixels == nullptr) {
      Serial.println(F("Failed to allocate display frame buffer"));
    } else {
      memset(compactPixels, 0, 240 * 240);
      Serial.println(F("Using compact RGB332 frame buffer"));
      dirtyLeft = 0;
      dirtyTop = 0;
      dirtyRight = 239;
      dirtyBottom = 239;
    }
  } else {
    std::fill(pixels, pixels + 240 * 240, GC9A01A_BLACK);
    dirtyLeft = 0;
    dirtyTop = 0;
    dirtyRight = 239;
    dirtyBottom = 239;
  }
  Serial.println(F("Success"));
  this->displayNum = displayNum;
  displayNum++;
}

GC9A01A_Display::~GC9A01A_Display() {
  free(pixels);
  free(compactPixels);
  delete display;
}

void GC9A01A_Display::drawPixel(int16_t x, int16_t y, uint16_t color565) {
  if ((pixels == nullptr && compactPixels == nullptr) || x < 0 || x >= 240 || y < 0 || y >= 240) {
    return;
  }
  if (pixels != nullptr) {
    pixels[y * 240 + x] = color565;
  } else {
    compactPixels[y * 240 + x] = ((color565 >> 8) & 0xE0) | ((color565 >> 6) & 0x1C) | ((color565 >> 3) & 0x03);
  }
  markDirty(x, y);
}

void GC9A01A_Display::drawFastVLine(int16_t x, int16_t y, int16_t height, uint16_t color565) {
  if ((pixels == nullptr && compactPixels == nullptr) || x < 0 || x >= 240 || height <= 0) {
    return;
  }
  const int16_t startY = y < 0 ? 0 : y;
  const int16_t unclippedEndY = y + height;
  const int16_t endY = unclippedEndY > 240 ? 240 : unclippedEndY;
  for (int16_t currentY = startY; currentY < endY; currentY++) {
    const int32_t index = currentY * 240 + x;
    if (pixels != nullptr) {
      pixels[index] = color565;
    } else {
      compactPixels[index] = ((color565 >> 8) & 0xE0) | ((color565 >> 6) & 0x1C) | ((color565 >> 3) & 0x03);
    }
  }
  if (startY < endY) {
    markDirty(x, startY);
    markDirty(x, endY - 1);
  }
}

void GC9A01A_Display::update() {
  if ((pixels == nullptr && compactPixels == nullptr) || dirtyRight < dirtyLeft || dirtyBottom < dirtyTop) {
    return;
  }

  const uint16_t width = dirtyRight - dirtyLeft + 1;
  const uint16_t height = dirtyBottom - dirtyTop + 1;
  display->startWrite();
  display->setAddrWindow(dirtyLeft, dirtyTop, width, height);
  if (pixels != nullptr) {
    for (int16_t y = dirtyTop; y <= dirtyBottom; y++) {
      display->writePixels(pixels + y * 240 + dirtyLeft, width);
    }
  } else {
    uint16_t row[240];
    for (int16_t y = dirtyTop; y <= dirtyBottom; y++) {
      for (uint16_t x = 0; x < width; x++) {
        const uint8_t color = compactPixels[y * 240 + dirtyLeft + x];
        const uint16_t red = (color >> 5) & 0x07;
        const uint16_t green = (color >> 2) & 0x07;
        const uint16_t blue = color & 0x03;
        row[x] = ((red << 2) | (red >> 1)) << 11 |
                 ((green << 3) | green) << 5 |
                 ((blue << 3) | (blue << 1) | (blue >> 1));
      }
      display->writePixels(row, width);
    }
  }
  display->endWrite();
  dirtyLeft = 240;
  dirtyTop = 240;
  dirtyRight = -1;
  dirtyBottom = -1;
}

bool GC9A01A_Display::isAvailable() const {
  return pixels != nullptr || compactPixels != nullptr;
}

void GC9A01A_Display::markDirty(int16_t x, int16_t y) {
  dirtyLeft = std::min(dirtyLeft, x);
  dirtyTop = std::min(dirtyTop, y);
  dirtyRight = std::max(dirtyRight, x);
  dirtyBottom = std::max(dirtyBottom, y);
}
