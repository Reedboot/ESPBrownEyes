// Define if you wish to debug memory usage.  Only works on T4.x
//#define DEBUG_MEMORY

#include <SPI.h>
#include <esp_system.h>

#include "config.h"
#include "util/logging.h"

/// INITIALIZATION -- runs once at startup ----------------------------------
void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 2000);
  delay(200);
  DumpMemoryInfo();
  Serial.println("Init");
  Serial.flush();
  randomSeed(esp_random());

  SPI.begin(eyeInfo[0].sck, -1, eyeInfo[0].mosi);
  initEyes(true, true, true);
}

/// MAIN LOOP -- runs continuously after setup() ----------------------------
void loop() {
  static uint32_t framesRendered{};
  static uint32_t fpsWindowStartMs = millis();
  if (eyes->renderFrame()) {
    framesRendered++;
  }
  const uint32_t now = millis();
  if (now - fpsWindowStartMs >= 1000) {
    Serial.print("FPS: ");
    Serial.println(framesRendered);
    framesRendered = 0;
    fpsWindowStartMs = now;
  }
}
