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
};

/*
 * displays a bright band of white at the top and bottom if sound peaks
 */
class SoundPeak: public Pattern {
  public: SoundPeak() {
  }
  
  void setup() {
  }

  void update() {
    //a sound peak occurs if current volume is twice the average volume
    if(soundlevel.getLastVolume()>soundlevel.getAudioLevel()*2) {
      for(int i = 0; i<ROWS; i++) {
        leds[LEDS_PER_ROW * i]=CRGB::White;
        leds[LEDS_PER_ROW * i+29]=CRGB::White;
      }
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
    for(int i = 0; i<soundlevel.getLevel()+1; i++) {
      leds[i]=CRGB::Red;
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
      leds[59] = CHSV( 0, 0, 64);
    } else {
      leds[59] = CRGB::Black;
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

