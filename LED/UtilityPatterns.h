#include "Common.h"
#include "Audio.h"
#include "LedCalculations.h"

/*
 * displays a bright band of white at the top and bottom if sound peaks
 */
class SoundPeak {
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
class SoundLevelStatus {
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
class FrameStatus {
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


SoundPeak soundpeak = SoundPeak();
SoundLevelStatus soundlevelstatus = SoundLevelStatus();
FrameStatus framestatus = FrameStatus();

