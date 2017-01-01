#Christmas Tree
A library for controlling a christmas tree made of ws2812b individually addressable RGB led modules.

This project was made with two arduinos, an Uno connected to a microphone and light sensor, that feeds ambient audio amplitude and ambient light brightness to the second, a Mega, that generates patterns and outputs data to the LEDs.

The principal of this project was to create an interactive display that reacts to sound. It responds to the ambient sound level, displays more active patterns when loud, more subtle patterns when quiet, and goes to sleep when very quiet. It also controls the overall brightness to match the ambient lighting.

###Block diagram of set-up.
```
                     +-----------------+       +-----------+
 Mic                 |  9v supply      |       | 5v supply |
  O=                 +-----------------+       +-----------+
   |                   ||           ||               ||
+-----+   +5    +---------+       +---------+        ||
|     |---------|         |       |         |        ||
|     |         |         D<-----<D         D>------|||
| Amp |-------->A         |       |         |       |+|
|     |   GND   |         D>----->Serial    |       | |
|     |---------|         |       |         |       |+|
+-----+         |         |       |         |       | |
                |   Uno   |       |   Mega  |       |+|
+-----+   +5    |         |       |         |       | |
|     |---------|  Sensor |       |   LED   |        .
|     |         |  Board  |       |  Board  |        .
| LDR |-------->A         |       |         |        .
|     |   GND   |         |       |         |
|     |---------|         |       |         |
+-----+         +---------+       +---------+
```
The LED board operates at 30 frames a second. At the start of each cycle sends a signal to the Sensor board by pulling a signal pin high.

The sensor board continually reads audo level and calclulates the maximum difference between high and low readings, approximating peak amplitude.
When the input signal pin goes high, it takes a light reading, and sends the peak volume and light level via SoftwareSerial to the LED board.

The LED board reads the data from the Sensor board, computes a frame of data for the leds, and outputs that data to the LEDs. It waits the remainder of the cycle time.

###Analog Circuit
![Analog Circuit](/Analog.PNG)
