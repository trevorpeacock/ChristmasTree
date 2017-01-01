#include <FastLED.h>
#include "PatternManager.h"

#define DEBUG false

#define LED_DATA_DPIN 2
#define FRAME_SIGNAL_DPIN 3

void setup() {
  //Debugging
  Serial.begin(250000);
  //Initialise FastLED library
  FastLED.addLeds<NEOPIXEL, LED_DATA_DPIN>(leds, NUM_LEDS);
  FastLED.setMaxPowerInVoltsAndMilliamps(5,5000); 
  //Comms from Sensor board
  Serial3.begin(250000);
  //Frame signal to Sensor Board
  pinMode(FRAME_SIGNAL_DPIN, OUTPUT);
  //Reset frame signal and clear serial buffer
  digitalWrite(FRAME_SIGNAL_DPIN, LOW);
  delay(5);
  while(Serial3.available()) Serial3.read();
  //Clear led buffer
  for(int i=0; i<NUM_LEDS; i++) {
    leds[i]=CRGB::Black;
  }
  //Setup managers
  //Determines which set of patterns to display based on audio levels
  levelmanager.setup();
  //Pattern that reacts directly to audio volume
  soundpeak.setup();
  //debug pattern, displays the current pattern level
  soundlevelstatus.setup();
  //debug pattern, flickers indicating program is running
  framestatus.setup();
}

//timer that tracks how long frame calculations take
long frametime=0;
//Target time for a frame
const int FRAME_TIME=33;  // 30 frames/sec
//tracks recent sound level
int lastlevel = -1;

void loop() {
  //sends average time to calculate a frame, once a second
  if(framenumber%30==0 and framenumber > 0) {
    Serial.println("Average frame time " + String(float(frametime)/30) + "ms.");
    frametime = 0;
  }
  framenumber ++;
  //Start a timer
  WaitFor t = WaitFor(FRAME_TIME);

  //Signal Sensor board to send data, and wait for 2 bytes
  digitalWrite(FRAME_SIGNAL_DPIN, HIGH);
  while(Serial3.available()<2) continue;
  digitalWrite(FRAME_SIGNAL_DPIN, LOW);
  //Read data and split into 
  unsigned int lightlevel = Serial3.read();
  unsigned int audiolevel = (lightlevel & 3) << 8;
  lightlevel = lightlevel >> 2;
  audiolevel = audiolevel | Serial3.read();

  //update sound level model
  soundlevel.update(r);
  int level = soundlevel.getLevel();

  if(lastlevel != soundlevel.getLevel()) {
    //if sound level has changed inform levelmanager
    lastlevel = soundlevel.getLevel();
    levelmanager.newlevel(lastlevel);
  }
  frametime += t.timeRemaining();
  //generate new frame data
  levelmanager.update();
  frametime -= t.timeRemaining();

  //small indicator of sound level and frame status for testing
  if(DEBUG) soundlevelstatus.update();
  if(DEBUG) framestatus.update();

  //peak indicator
  soundpeak.update();

  //set overall brightness baseed on ambient light levels
  FastLED.setBrightness(map(constrain(lightlevel, 5, 40), 2, 40, 0, 255));
  //Display pattern
  FastLED.show();
  //Send alert if calculations took too long
  if(t.timeRemaining()<0)
    Serial.println("CYCLE TOOK " + String(-t.timeRemaining()) + "ms TOO LONG");
  //If we have any time left, refresh leds. Probably never happens.
  while(t.timeRemaining()>16) {
    FastLED.show();
  }
  //Wait out the rest of the frame
  while(t.wait()) {
  }
}

