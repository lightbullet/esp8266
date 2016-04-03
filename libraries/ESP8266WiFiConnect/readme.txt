This is an WifiConnect library for ESP8266 Arduino

Installation
--------------------------------------------------------------------------------

To install this library, just place this entire folder as a subfolder in your
Arduino/lib/targets/libraries folder.

When installed, this library should look like:

Arduino/lib/targets/libraries/ESP8266WifiConnect                        (this library's folder)
Arduino/lib/targets/libraries/ESP8266WifiConnect/ESP8266WifiConnect.cpp (the library implementation file)
Arduino/lib/targets/libraries/ESP8266WifiConnect/ESP8266WifiConnect.h   (the library description file)
Arduino/lib/targets/libraries/ESP8266WifiConnect/keywords.txt           (the syntax coloring file)
Arduino/lib/targets/libraries/ESP8266WifiConnect/examples               (the examples in the "open" menu)
Arduino/lib/targets/libraries/ESP8266WifiConnect/readme.txt             (this file)

Building
--------------------------------------------------------------------------------

After this library is installed, you just have to start the Arduino application.
You may see a few warning messages as it's built.

To use this library in a sketch, go to the Sketch | Import Library menu and
select ESP8266WifiConnect.  This will add a corresponding line to the top of your sketch:
#include <ESP8266WifiConnect.h>

To stop using this library, delete that line from your sketch.

Geeky information:
After a successful build of this library, a new file named "ESP8266WifiConnect.o" will appear
in "Arduino/lib/targets/libraries/ESP8266WifiConnect". This file is the built/compiled library
code.

If you choose to modify the code for this library (i.e. "ESP8266WifiConnect.cpp" or "ESP8266WifiConnect.h"),
then you must first 'unbuild' this library by deleting the "ESP8266WifiConnect.o" file. The
new "ESP8266WifiConnect.o" with your code will appear after the next press of "verify"

