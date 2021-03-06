﻿# Christmas Tree
A library for controlling a christmas tree made of ws2812b individually addressable RGB led modules.

The principal of this project was to create an interactive display that reacts to sound. It responds to the ambient sound level, displays more active patterns when loud, more subtle patterns when quiet, and goes to sleep when very quiet. It also controls the overall brightness to match the ambient lighting.

## Operation
This project was made with two arduinos, an Uno connected to a microphone and light sensor, that feeds ambient audio amplitude and ambient light brightness to the second, a Mega, that generates patterns and outputs data to the LEDs.

### Block diagram of set-up
```
                     ╔═════════════════╗       ╔═══════════╗
    Mic              ║  9v supply      ║       ║ 5v supply ║
   ╔═O               ╚═╤╤═══════════╤╤═╝       ╚════╤═╤════╝
   ║                   ││           ││    ┌─────────┤ │
╔══╩══╗   +5    ╔══════╧╧═╗       ╔═╧╧════╧═╗ GND   │ │
║     ╟─────────╢         ║       ║         ║       │ │
║     ║         ║         D<─────<D         D>──R───│┐│
║ Amp ╟────────>A         ║       ║         ║       ┢┷┪
║     ║         ║         D>─────>Serial    ║       ┃ ┃ LED
║     ╟─────────╢         ║       ║         ║       ┃o┃ Strip
╚═════╝   GND   ║         ║       ║         ║       ┃ ┃
                ║   Uno   ║       ║   Mega  ║       ┃o┃
╔═════╗   +5    ║         ║       ║         ║       ┃ ┃
║     ╟─────────╢  Sensor ║       ║   LED   ║       ┃o┃
║     ║         ║  Board  ║       ║  Board  ║       ┃ ┃
║ LDR ╟────────>A         ║       ║         ║       ┇ ┇
║     ║         ║         ║       ║         ║       ┇ ┇
║     ╟─────────╢         ║       ║         ║       ┋ ┋
╚═════╝   GND   ╚═════════╝       ╚═════════╝
```

The LED board operates at 30 frames a second. At the start of each cycle sends a signal to the Sensor board by pulling a signal pin high.

The sensor board continually reads audo level and calclulates the maximum difference between high and low readings, approximating peak amplitude.
When the input signal pin goes high, it takes a light reading, and sends the peak volume and light level via SoftwareSerial to the LED board.

The LED board reads the data from the Sensor board, computes a frame of data for the leds, and outputs that data to the LEDs. It waits the remainder of the cycle time.

### Analog Circuit
![Analog Circuit](/Sensor.png)

## To Build
### Components

Core components

* Arduino with 2 analog inputs and 2 digital pins
* Arduino running at 16MHz, with a spare hardware serial interface, and digital pin, with 30KB program memory and 8KB RAM
* 16m of 30LED/m RGB addressable ws2812b leds (can be varied), cut into 1m lengths
* A 9v power supply (or pair of) to power the arduino boards, connectors as required
* A 5v power supply, approx 120Watts (30A)
* 370 Ohm resistor

Power components

* 2 30A+ Bus Bars
* 4 10A fuses in holders
* Terminal block, 8x10A
* Approx 20m of power cable (2 conductor), minimum 2A
* 4m hookup wire
* 16x 1000uF electrolytic capacitors

Analog Circuit Components

* Electret Mic
* Light Dependant Resistor
* Components as shown in schematic (LM324 op-amp, capacitors, resistors)
* Breadboard, stripboard/veroboard or perfboard to construct circuit

For mounting

* 1.5m x 20mm dowel
* 2 x 550mm x 6mm threaded rod, 2x 150mm x 6mm threaded rod
* 8-16x matching nuts
* 8m x 2mm wire (5x coathangers)
* Flower pot, bricks, cable ties
* approx. 1m x 2m of thin paper or white cloth.

Helpful components

* 2x USB A-B cables to program arduinos
* USB cables and hub, to program both boards
* 5m usb active extention cable, to program from a distance

### Power schematic
I ran multiple wires from the high current supply, to bus bars, from the positive bus bar, through fuses into terminal blocks. A 1000uF capacitor was soldered directly to the end of each 1m strip, and a 1m length of power cable. The cable ran to the power supply, positive to a terminal block, the negative to the bus bar.

A 370 Ohm resistor is connected between the arduino and data in of the first LED strip.
```
╔════╗   +5   ╔═════╗ Fuses               ╔══════╗
║    ╟────────╢ 5v  ╟──o~o────────────────╢ Term ║
║    ║   +5   ║ BUS ║             ╔══════╗╚═╤╤╤╤═╝
║ 5v ╟────────╢     ╟──o~o────────╢ Term ║  ││││
║    ║   +5   ║     ╟──o~o──┄┄┄   ╚═╤╤╤╤═╝  ││││
║ S  ╟────────╢     ╟──o~o──┄┄┄     ││││    ││││
║ u  ║        ╚═════╝               ││││    ││││
║ p  ║   GND  ╔═════╗               ││││    ││││
║ p  ╟────────╢ GND ╟──────────┬┬┬┐ ││││    ││││
║ l  ║   GND  ║ BUS ╟─────┬┬┬┐ ││││ ││││    ││││
║ y  ╟────────╢     ╟─┄┄┄ ┆┆┆┆ ┆┆┆┆ ┆┆┆┆    ┆┆┆┆
║    ║   GND  ║     ╟┄┄┄     GND         5v
║    ╟────────╢     ║        LEDs       LEDs
╚════╝        ╚═════╝
```

### Wiring of led strips
```
            ┌────────────────┐                ┌────────┄┄┄┄
        ╔═╧═╧═╧═╗        ╔═╧═╧═╧═╗        ╔═╧═╧═╧═╗
        ║ ┌───┐ ║        ║ ┌───┐ ║        ║ ┌───┐ ║
        ║ │ O │ ║        ║ │ O │ ║        ║ │ O │ ║    ....
        ║ └───┘ ║        ║ └───┘ ║        ║ └───┘ ║
        ┇       ┇        ┇       ┇        ┇       ┇
        ┇       ┇        ┇       ┇        ┇       ┇
        ║ ┌───┐ ║        ║ ┌───┐ ║        ║ ┌───┐ ║
        ║ │ O │ ║        ║ │ O │ ║        ║ │ O │ ║    ....
        ║ └───┘ ║        ║ └───┘ ║        ║ └───┘ ║
        ║ ┌───┐ ║        ║ ┌───┐ ║        ║ ┌───┐ ║
        ║ │ O │ ║        ║ │ O │ ║        ║ │ O │ ║    ....
        ║ └───┘ ║        ║ └───┘ ║        ║ └───┘ ║
        ╚═╤═╤═╤═╝        ╚═╤═╤═╤═╝        ╚═╤═╤═╤═╝
    ┌─────│─┘ │            │ └─│────────────│─┘ │  
   ┌┴┐    │   │            │   │            │   │  
   │R│    ├─┨┠┤            ├─┨┠┤            ├─┨┠┤      ....
   └┬┘    │cap│            │cap│            │cap│  
To  ┆     └┐┌─┘            └┐┌─┘            └┐┌─┘  
Arduino    ││               ││               ││   
           ┆┆               ┆┆               ┆┆   
        To Power         To Power         To Power
```
Data entered at the bottom of the first strip. Each alternate strip was installed upside down, the power always entering from the bottom. Data was dasied alternately at the top and bottom of each pair of strips.

### Physical construction
Securely cable tie the dowel to some bricks, place the bricks in the flower pot.

Drill a pair of 5.5mm holes through the dowel, 90deg apart at the top of the dowel. Lock the rod in the chuck of a cordless drill (lock two nuts against eacher on the thread if necessary) and thread half way it into the holes. Repeat with the longer rods 980mm below the top rods.

Wrap the wire around each rod, from the end of one to the end of the next (about 30mm in) forming two circles, one at the top and one at the bottom. Use nuts to position loops. Repeat at the end of the rods. Inner circles will hold leds, outer will hold paper/cloth covering.

Fix each led strip vertically around the inner wire loop, we found thin strips of gaffers tape to be the simplest and most effective method.

## Code Summary
```
Sensor   ╔══════╗ audiolevel ╔══════════════╗
Board ───>      >────────────> SoundReactor ║
         ║ Main <────────────< (soundlevel) ║
         ║ Loop ║ soundlevel ║ (Audio.h)    ║
         ║      ║            ╚══════════════╝
         ║      ║
         ║      ║ soundlevel ╔══════════════════╗  patternid ╔══════════════════╗
         ║      >────────────> LevelManager     >────────────> PatternManager   ║ pattern ╔═══════════╗
         ║      ║            ║ PatternManager.h ║            ║ PatternManager.h <─────────< Pattern   ║ 
         ║      ║            ║                  ║            ║                  ║         ║ Pattern.h ║
         ║      ║            ║                  ║            ║                  ║         ╚═══════════╝
         ║      ║  pattern   ║                  ║   pattern  ║                  ║ pattern ╔═══════════╗
         ║      <────────────<                  <────────────<                  <─────────< Pattern   ║
         ║      ║            ╚══════════════════╝            ║                  ║         ╚═══════════╝
         ║      ║  pattern   ╔═══════════════════════╗       ║                  ║               :
         ║      <────────────< Utility/Debug Pattern ║       ║                  ║               :
         ║      ║            ║ UtilityPatterns.h     ║       ╚══════════════════╝               .
         ║      ║            ╚═══════════════════════╝
         ║      ║  pattern   ╔═══════════════════════╗
         ║      <────────────< Utility/Debug Pattern ║
         ║      ║            ╚═══════════════════════╝
         ║      ║  pattern   ╔═════════╗
         ║      >────────────> FastLED ║
         ╚══════╝            ╚═════════╝
```
Every frame the main loop retrieves audio and light levels from the sensor board. It updates the SoundReactor model, which returns a discreet level, which is passed to LevelManager.

LevelManager contains a pattern set for each soundlevel, and manages PatternManager to rotate patterns periodically.

PatternManager invokes the specific pattern, which in turn renders a frame. PatternManager manages crossfading patterns, calling a second Pattern if necessary.

The main loop calls several utility or debug patterns as required, before passing the rendered frame to FastLED.

## Resources
* [Photos](https://www.flickr.com/photos/trevorpeacock/tags/ledchristmastree2016/)
* [Video](https://youtu.be/KjJf4GW_VQg)

## Future work

* Come up with some more interesting subtle patterns
