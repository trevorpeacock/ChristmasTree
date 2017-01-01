#include <SoftwareSerial.h>
#include "Common.h"

#define FRAME_SIGNAL_DPIN 12
#define SOFTWARE_SERIAL_DPIN 13
#define AUDIO_APIN 0
#define LIGHT_SENSOR_APIN 1

SoftwareSerial mySerial(0, SOFTWARE_SERIAL_DPIN); // dont care about read pin, we never read.

MinMax minmax;

void setup() {
  //Debugging
  Serial.begin(250000);
  //Comm to LED board
  mySerial.begin(250000);
  //Initialise the audio levels
  minmax = MinMax();
}

bool signalpin = false;

void loop() {
  int signalpinval = digitalRead(FRAME_SIGNAL_DPIN);
  //Check if signal pin goes high
  if(signalpinval==HIGH && signalpin!=signalpinval) {
    //Send data to LED board

    //Fetch audio and read light data
    unsigned int audiolevel = minmax.getRange();
    unsigned int lightlevel = analogRead(LIGHT_SENSOR_APIN);
    /*  Arrange data into two bytes
     *  llllllaa aaaaaaaa
     *  6 bits (0-63) for light, 10 bits (0-1023) for audio */
    lightlevel = (lightlevel / 16) << 10;
    audiolevel |= lightlevel;
    //send two bytes to LED board
    mySerial.write(byte(audiolevel>>8));
    mySerial.write(byte(audiolevel&255));
    //reset audio levels
    minmax = MinMax();
  } else {
    //Spend 1ms reading in audio data
    WaitFor timer = WaitFor(1);
    while(timer.wait()) {
      minmax.update(analogRead(AUDIO_APIN));
    }
  }
  signalpin = signalpinval;
}

