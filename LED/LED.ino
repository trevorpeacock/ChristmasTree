#include <FastLED.h>
#include "PatternManager.h"

//Limits maximum power draw to the specified number of amps.
float MAX_POWER_AMPS = 0;

//WARNING: this will disable MAX_POWER_AMPS limit and run leds as hard as possible.
//WARNING: this will disable MAX_POWER_AMPS limit and run leds as hard as possible.
//WARNING: this will disable MAX_POWER_AMPS limit and run leds as hard as possible.
#define LOADTEST false

#define LED_DATA_DPIN 2
#define FRAME_SIGNAL_DPIN 3


void setup() {
  //Debugging
  Serial.begin(9600);
  //Initialise FastLED library
  FastLED.addLeds<NEOPIXEL, LED_DATA_DPIN>(leds, NUM_LEDS);
  if(!LOADTEST)
    if(MAX_POWER_AMPS>0)
      FastLED.setMaxPowerInVoltsAndMilliamps(5,MAX_POWER_AMPS*1000);
  //Comms from Sensor board
  if(SOUND_SENSOR || LIGHT_SENSOR) Serial3.begin(9600);
  //Frame signal to Sensor Board
  pinMode(FRAME_SIGNAL_DPIN, OUTPUT);
  //Reset frame signal and clear serial buffer
  digitalWrite(FRAME_SIGNAL_DPIN, LOW);
  delay(5);
  if(SOUND_SENSOR || LIGHT_SENSOR) while(Serial3.available()) Serial3.read();
  //Clear led buffer
  for(int i=0; i<NUM_LEDS; i++) {
    leds[i]=CRGB::Black;
  }
  //Setup managers
  //Determines which set of patterns to display based on audio levels
  levelmanager.setup();
  //Pattern that reacts directly to audio volume
  if(SOUND_SENSOR) soundpeak.setup();
  //debug pattern, displays the current pattern level
  if(SOUND_SENSOR) soundlevelstatus.setup();
  //debug pattern, flickers indicating program is running
  framestatus.setup();
}

//timer that tracks how long frame calculations take
long frametime=0;
//Target time for a frame
const int FRAME_TIME=33;  // 30 frames/sec
//tracks recent sound level
int lastlevel = -1;

unsigned int lightlevel = 0;
unsigned int audiolevel = 0;

void loop() {
  //sends average time to calculate a frame, once a second
  if(DEBUG && (framenumber%30==0 and framenumber > 0)) {
    Serial.println("Average frame time " + String(float(frametime)/30) + "ms.");
    Serial.println("light: " + String(lightlevel) + " audio:" + String(audiolevel));
    frametime = 0;
  }
  framenumber ++;
  //Start a timer
  WaitFor t = WaitFor(FRAME_TIME);

  if(LOADTEST) {
    if(framenumber==1) loadtest.setup();
    if(!Serial.available()) {
      loadtest.update(leds);
    } else {
      //if we receive something, stop the test.
      blank.update(leds);
    }
    if(framenumber%30==0) {
      Serial.println("Power: " + String(0.001 * calculate_unscaled_power_mW(leds, NUM_LEDS)) + "W");
      frametime = 0;
    }
    FastLED.show();
    while(t.wait());
    return;
  }
  
  if(SOUND_SENSOR || LIGHT_SENSOR) {
    //Signal Sensor board to send data, and wait for 2 bytes
    while(Serial3.available()) Serial3.read();
    digitalWrite(FRAME_SIGNAL_DPIN, HIGH);
    while(Serial3.available()<3) continue;
    while(Serial3.available() && Serial3.read()!=42) continue;
    while(Serial3.available()<2) continue;
    digitalWrite(FRAME_SIGNAL_DPIN, LOW);
    //Read data and split into 
    lightlevel = Serial3.read();
    //Serial.print(lightlevel);
    //Serial.print(" ");
    audiolevel = (lightlevel & 3) << 8;
    lightlevel = lightlevel >> 2;
    audiolevel = audiolevel | Serial3.read();
    if(SOUND_SENSOR) {
      //update sound level model
      soundlevel.update(audiolevel);
      int level = soundlevel.getLevel();
    
      if(lastlevel != soundlevel.getLevel()) {
        //if sound level has changed inform levelmanager
        lastlevel = soundlevel.getLevel();
        levelmanager.newlevel(lastlevel);
      }
    }
  }

  frametime += t.timeRemaining();
  //generate new frame data
  levelmanager.update();
  frametime -= t.timeRemaining();

  //small indicator of sound level and frame status for testing
  if(SOUND_SENSOR && DEBUG) soundlevelstatus.update();
  if(DEBUG) framestatus.update();

  //peak indicator
  if(SOUND_SENSOR && !DEMO) soundpeak.update();

  //set overall brightness baseed on ambient light levels
  if(LIGHT_SENSOR && !DEMO)
    FastLED.setBrightness(map(constrain(lightlevel, 5, 40), 1, 40, 0, 255));
  //FastLED.setBrightness(64);
  //Display pattern
  FastLED.show();
  //Send alert if calculations took too long
  if(t.timeRemaining()<0)
    Serial.println("CYCLE TOOK " + String(-t.timeRemaining()) + "ms TOO LONG");
  //Wait out the rest of the frame
  while(t.wait()) {
  }
}
