/*
 * Functional or debug patterns that overlay a rendered pattern.
 * These are called from the main loop.
 */
#include "Common.h"
#include "Audio.h"
#include "LedCalculations.h"

/*
 * base class, by default blanks all LEDs
 */
class Pattern {
  public:
    //This is called everytime the pattern starts to be used
    virtual void setup() {
    }
    //Called every frame. Pattern should be supplied to provided buffer
    virtual void update(CRGB ledbuffer[]) {
      for (int i = 0; i < NUM_LEDS; i++) {
        ledbuffer[i] = CRGB::Black;
      }
    }

    virtual void copy_to_star(CRGB ledbuffer[], int pos) {
      int diff = NUM_LEDS_TREE+pos;
      for (int i = NUM_LEDS_TREE; i < NUM_LEDS; i++) {
        ledbuffer[i] = ledbuffer[i-diff];
      }
    }
};

/*
 * displays a bright band of white at the top and bottom if sound peaks
 */
class SoundPeak: public Pattern {

  int frames;
  const int maxframes = 7;
  
  public: SoundPeak() {
    frames = 0;
  }
  
  void setup() {
  }

  void update() {
    CRGB color ;
    //a sound peak occurs if current volume is twice the average volume
    if(soundlevel.getLastVolume()>soundlevel.getAudioLevel()*2) {
      frames=maxframes;
      color = CHSV(random8(), 255, 255);
      for(int i=0; i<50; i++) {
        leds[random(NUM_LEDS)]=color;
      }
    }
    if(frames>0) {
      color = CHSV(0,0,255*frames*frames/(maxframes*maxframes));
      for(int i = 0; i<ROWS; i++) {
        leds[LEDS_PER_ROW * i]=color;
        leds[LEDS_PER_ROW * i+29]=color;
      }
      frames--;
    }
  }

};

/*
 * displays a number of red dots representing the audiolevel
 */
class SoundLevelStatus: public Pattern {
  public: SoundLevelStatus() {
  }
  
  void setup() {
  }

  void update() {
    CRGB color;
    switch (soundlevel.getLevel()) {
      case 0:
        color=CRGB::Red; break;
      case 1:
        color=CRGB::Green; break;
      case 2:
        color=CRGB::Blue; break;
    }    
    for(int i = 1; i<soundlevel.getLevel()+2; i++) {
      leds[ledid(i,0)]=color;
    }
  }

};

/*
 * blinks a led on or off each frame
 */
class FrameStatus: public Pattern {
  public: FrameStatus() {
  }
  
  void setup() {
  }

  void update() {
    if(framenumber%2==1) {
      leds[0] = CHSV( 0, 0, 64);
    } else {
      leds[0] = CRGB::Black;
    }
  }

};

/*
 * Tests the power supply
 */
class LoadTest: public Pattern {

  public:
    LoadTest() {
    }

    virtual void setup() {
    }

    virtual void update(CRGB ledbuffer[]) {
      Pattern::update(ledbuffer);
      int framestep = 3;
      int frame = framenumber/framestep;
      int phase1 = NUM_LEDS*framestep;
      int phase2 = phase1 + 30*60;
      int phase3 = phase2 + 30*5;
      if(framenumber<phase1) {
        //Slowly increase power, turning on one led at a time
        int start = NUM_LEDS - (frame % int(NUM_LEDS))-1;
        for(int i=NUM_LEDS-1; i>=start; i--) {
          ledbuffer[i]=CRGB::White;
        }
      } else if(framenumber<phase2) {
        //Blink all leds on and off every 10 seconds
        if(((frame-NUM_LEDS)/10/10) % 2==0) {
          for(int i=0; i<NUM_LEDS; i++) {
            ledbuffer[i]=CRGB::White;
          }
        }
      } else if(framenumber<phase3) {
        //Blink all leds on and off every frame
        if(framenumber % 2==0) {
          for(int i=0; i<NUM_LEDS; i++) {
            ledbuffer[i]=CRGB::White;
          }
        }
      } else {
        //Keep leds on
        for(int i=0; i<NUM_LEDS; i++) {
          ledbuffer[i]=CRGB::White;
        }
      }
    }
};

SoundPeak soundpeak = SoundPeak();
SoundLevelStatus soundlevelstatus = SoundLevelStatus();
FrameStatus framestatus = FrameStatus();
LoadTest loadtest = LoadTest();
