#include <FastLED.h>
#include "PatternManager.h"



int lastlevel = -1;

void setup() {
  FastLED.addLeds<NEOPIXEL, LED_DATA_PIN>(leds, NUM_LEDS);
  FastLED.setMaxPowerInVoltsAndMilliamps(5,5000); 
  Serial.begin(250000);
  Serial3.begin(250000);
  pinMode(3, OUTPUT);
  while(Serial3.available()) Serial3.read();
  digitalWrite(3, LOW);
  delay(5);
  for(int i=0; i<NUM_LEDS; i++) {
    leds[i]=CRGB::Black;
  }
  levelmanager.setup();
  soundpeak.setup();
  soundlevelstatus.setup();
  framestatus.setup();
}

long frametime=0;
const int FRAME_TIME=33;

void loop() {
  if(framenumber%30==0) {
    Serial.println("Average frame time " + String(float(frametime)/30) + "ms.");
    frametime = 0;
  }
  framenumber ++;
  WaitFor t = WaitFor(FRAME_TIME);

  digitalWrite(3, HIGH);
  while(Serial3.available()<2) continue;
  digitalWrite(3, LOW);
  unsigned int l = Serial3.read();
  unsigned int r = (l & 3) << 8;
  l = l >> 2;
  r = r | Serial3.read();
  //Serial.println(l*4);
  // l is brightness from 0 to 255
  // r us loudness from 0 to 1023

  soundlevel.update(r);
  int level = soundlevel.getLevel();
  //Serial.println(level);

  if(lastlevel != soundlevel.getLevel()) {
    lastlevel = soundlevel.getLevel();
    //Serial.println(lastlevel);
    levelmanager.newlevel(lastlevel);
  }
  /*if(framenumber%(10*30)==0) {
    if((framenumber/(10*30))%2==0) {
      patternmanager.transition(2);
    } else {
      patternmanager.transition(3);
    }
  }*/
  frametime += t.timeRemaining();
  levelmanager.update();
  frametime -= t.timeRemaining();

  //small indicator of sound level for testing
  //soundlevelstatus.update();
  //peak indicator
  soundpeak.update();

  //framestatus.update();

  FastLED.setBrightness(map(constrain(l, 5, 40), 2, 40, 0, 255));
  //Serial.println(String(map(constrain(l, 5, 40), 2, 40, 0, 255)) + " " + String(l));
  //Serial.println(String(float(calculate_unscaled_power_mW(leds, NUM_LEDS))/5/1000)+"A");
  FastLED.show();
  //int displaycount=0;
  if(t.timeRemaining()<0)
    Serial.println("CYCLE TOOK " + String(-t.timeRemaining()) + "ms TOO LONG");
  while(t.timeRemaining()>16) {
    //displaycount++;
    FastLED.show();
  }
  while(t.wait()) {
  }
}
