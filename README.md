#Christmas Tree
A library for controlling a christmas tree made of ws2812b individually addressable RGB led modules.

This project was made with two arduinos, an Uno connected to a microphone and light sensor, that feeds ambient audio amplitude and ambient light brightness to the second, a Mega, that generates patterns and outputs data to the LEDs.

Block diagram of set-up.
```
                +-----------------+      +-----------+
 Mic            |  9v supply      |      | 5v supply |
  O=            +-----------------+      +-----------+
   |              ||           ||              ||
+-----+ +5  +---------+     +---------+        ||
|     |-----|         |     |         |        ||
|     |     |        D<-----<D        |-------|||
| Amp |----->A        |     |         |       |+|
|     | GND |        D>----->Serial   |       | |
|     |-----|         |     |         |       |+|
+-----+     |         |     |         |       | |
            |   Uno   |     |   Mega  |       |+|
+-----+ +5  |         |     |         |       | |
|     |-----|         |     |         |        .
|     |     |         |     |         |        .
| LDR |----->A        |     |         |        .
|     | GND |         |     |         |
|     |-----|         |     |         |
+-----+     +---------+     +---------+
```