# ESP32BrownEyes

### Introduction

Animated eyes for 240x240 GC9A01 displays, currently targeting a Freenove ESP32-WROOM board.

This project builds on code and ideas from:
  - https://github.com/adafruit/Uncanny_Eyes
  - https://github.com/adafruit/Adafruit_Learning_System_Guides/tree/main/M4_Eyes
  - https://github.com/adafruit/Adafruit-GC9A01A

### Building and Running

This project is developed with PlatformIO and (optionally) CLion.

Build the firmware with `pio run -e eyes` and upload it with `pio run -e eyes -t upload`.
The configured board is `esp32dev`, compatible with the Freenove ESP32-WROOM-32.

### Display Pinout

Both displays share the SPI clock and data lines. The left display is currently configured in firmware;
the right-display pins below are for a second display and are not currently enabled.

| Display signal | ESP32 GPIO | Connection |
|---|---:|---|
| SCL / SCK (both) | 18 | SPI clock |
| SDA / MOSI (both) | 23 | SPI data; green wire on the current left display |
| Left CS | 5 | Left display chip select |
| Left DC / A0 | 27 | Left display data/command |
| Left RST | 25 | Reset, shared with right display if connected |
| Right CS | 32 | Optional right display chip select |
| Right DC / A0 | 26 | Optional right display data/command |
| VCC (both) | 3.3 V | Display power |
| GND (both) | GND | Common ground with ESP32 |

Wire colors other than the confirmed green SDA/MOSI lead are not specified; follow the GPIO numbers.
Runtime FPS is printed to the serial monitor at 115200 baud.

The eye graphics are compiled into the firmware, so this project does not need a filesystem image.
The configured `huge_app` partition fits the firmware but does not provide an OTA update slot.

### Creating Your Own Animated Eyes
Each type of eye is defined with a config.eye JSON file specifying various parameters of the eye, pupil, iris and
sclera, plus (optional) bitmaps that define the extents of the upper and lower eyelids, along with any textures for the
iris and sclera.

Detailed documentation for config.eye is coming, but in the meantime, if you want to try creating your own eyes
then copying and editing existing eyes in the `resources/eyes/240x240` directory is a good
starting point. Note that colors are currently only able to be specified as 5:6:5 RGB decimal
or hex values. Try [this](http://greekgeeks.net/#maker-tools_convertColor) for help in creating
your own 5:6:5 color codes.

Once you have an eye definition that you want to try, you need to generate code from it for
use in your firmware. For this you will need a recent version of [Python](https://www.python.org/) installed, as well as [numpy](https://numpy.org/)
and the imaging library [Pillow](https://python-pillow.org/). You can install these two libraries with:
```shell
pip install numpy
pip install Pillow
```

To generate the data files for a single eye, run the following from the `resources/eyes/240x240` directory:
```shell
python tablegen.py ../../../src/eyes/graphics/240x240 path/to/your/config.eye
```
To generate _all_ eyes, run the `genall.py` command shown below. Note that you may want to make sure the output directory is empty first, so you don't end
up with redundant data files in there if your eye's configurations have changed.
```shell
python genall.py ../../../src/eyes/graphics/240x240
```

To use your newly created eye, include it with `#include "path/to/eyename.h"` and access it in your
code using `eyename::eye`, or with `eyename::left` and `eyename::right` if your eye has different parameters
for the left and right eyes.
