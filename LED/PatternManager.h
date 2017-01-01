#include "Patterns.h"
#define DEMO false

//Lists all available patterns
namespace PATTERNS {
  enum PATTERN {
    BLANK,
    ORNAMENTS,
    CHASE1,
    CHASE2,
    FLASHROW,
    FLASHRING,
    RADIO,
    EYE,
    FIRE,
    BOUNCINGBALL,
    LOUDNESS,
    FALLINGSTAR,
    SPARKLE,
    WIGGLE,
    DIAGONAL,
    FIREWORKS
  };
};

//Initialise all patterns used
Blank blank = Blank();
Ornaments ornaments = Ornaments();
Chase1 chase1 = Chase1();
Chase2 chase2 = Chase2();
FlashRow flashrow = FlashRow();
FlashRing flashring = FlashRing();
ChristmasRadio christmasradio = ChristmasRadio();
Eye eye = Eye();
Fire fire = Fire();
BouncingBall bouncingball = BouncingBall();
Loudness loudness = Loudness();
FallingStar fallingstar = FallingStar();
Sparkle sparkle = Sparkle();
Wiggle wiggle = Wiggle();
Diagonal diagonal = Diagonal();
Fireworks fireworks = Fireworks();

// given a pattern id, return that model
// eg. getPattern(PATTERNS::BLANK) returns blank object.
Pattern* getPattern(int p) {
  switch (p) {
    case PATTERNS::BLANK:
      return &blank;
    case PATTERNS::ORNAMENTS:
      return &ornaments;
    case PATTERNS::CHASE1:
      return &chase1;
    case PATTERNS::CHASE2:
      return &chase2;
    case PATTERNS::FLASHROW:
      return &flashrow;
    case PATTERNS::FLASHRING:
      return &flashring;
    case PATTERNS::RADIO:
      return &christmasradio;
    case PATTERNS::EYE:
      return &eye;
    case PATTERNS::FIRE:
      return &fire;
    case PATTERNS::BOUNCINGBALL:
      return &bouncingball;
    case PATTERNS::LOUDNESS:
      return &loudness;
    case PATTERNS::FALLINGSTAR:
      return &fallingstar;
    case PATTERNS::SPARKLE:
      return &sparkle;
    case PATTERNS::WIGGLE:
      return &wiggle;
    case PATTERNS::DIAGONAL:
      return &diagonal;
    case PATTERNS::FIREWORKS:
      return &fireworks;
  }
}

class PatternManager {
    //Number of frames during which both patterns should be cross-faded
    const int TRANSITION_FRAMES = 75;

    //Pattern currently being used
    int currentpattern = 0;
    //the next pattern to use, or 0 if not in transition
    int nextpattern = 0;
    //frame counter for the transition period
    int transition_status = 0;
    CRGB spare[NUM_LEDS];

  public: PatternManager() {
    }


    void setup() {
    }

    void update() {
      //Debug/testing. Manually set single pattern to use.
      if (false) {
        currentpattern = PATTERNS::DIAGONAL;
        if(framenumber==1) getPattern(currentpattern)->setup();
        getPattern(currentpattern)->update(leds);
        return;
      }
      //Check if we are transitioning
      if (nextpattern) {
        if (transition_status == 0) {
          //Setup pattern on first frame
          getPattern(nextpattern)->setup();
        }
        if (transition_status < TRANSITION_FRAMES) {
          transition_status ++;
        } else {
          //Transition complete
          currentpattern = nextpattern;
          nextpattern = 0;
          transition_status = 0;
        }
      }
      //Run pattern
      getPattern(currentpattern)->update(leds);
      //If we are transitioning crossfade the new pattern
      if (nextpattern) {
        //Fade the current framebuffer
        int fade_percent = 255 * transition_status / TRANSITION_FRAMES;
        fade_percent = sin8((fade_percent / 2 + 64) % 256);
        for (int i = 0; i < NUM_LEDS; i++) {
          leds[i].nscale8(fade_percent);
        }
        //Call new patter, writing into a spare buffer
        getPattern(nextpattern)->update(spare);
        //Mix pattern into main buffer
        for (int i = 0; i < NUM_LEDS; i++) {
          spare[i].nscale8(255 - fade_percent);
          leds[i] += spare[i];
        }
      }
    }

    //Start transition into new pattern
    void transition(int pattern) {
      nextpattern = pattern;
      transition_status = 0;
    }

};


//Manages which pattern is displayed, specified by newlevel(int)
class LevelManager {

    //how long (in frames) each pattern should be displayed
    const int FRAMES_PER_PATTERN = 30*60;
    PatternManager patternmanager = PatternManager();
    //which set of patterns are in use. Corresponds to audio level.
    int patternset;
    //frame counter since level was changed
    int framessincelevelchange;
    //current pattern in use
    PATTERNS::PATTERN currentpattern;

  public: LevelManager() {
    }


    void setup() {
      //initialise pattern manager
      patternmanager.setup();
      //default to pattern set 1
      patternset = 1;
      framessincelevelchange = 0;
    }

    void update() {
      if(DEMO) {
        const int frames_per_pattern = 30*15;
        if(framenumber==1) framenumber = frames_per_pattern - 5*30;
        long frame = framenumber-1;
        if(framenumber%frames_per_pattern==0) {
          int patternid = (framenumber/ frames_per_pattern) - 1;
          PATTERNS::PATTERN pattern;
          switch (patternid % 9) {
            case -1:
              pattern = PATTERNS::BLANK; break;
            case 0:
              pattern = PATTERNS::ORNAMENTS; break;
            case 1:
              pattern = PATTERNS::FALLINGSTAR; break;
            case 2:
              pattern = PATTERNS::DIAGONAL; break;
            case 3:
              pattern = PATTERNS::RADIO; break;
            case 4:
              pattern = PATTERNS::FIRE; break;
            case 5:
              pattern = PATTERNS::CHASE1; break;
            case 6:
              pattern = PATTERNS::BOUNCINGBALL; break;
            case 7:
              pattern = PATTERNS::SPARKLE; break;
            case 8:
              pattern = PATTERNS::FIREWORKS; break;
          };
          patternmanager.transition(pattern);
        }
        patternmanager.update();
        return;
      }
      //patterns are displayed for a defined amount of time, each pattern in the patternset displayed in turn
      //check if its time to change pattern within the patternset
      if(framessincelevelchange % int(FRAMES_PER_PATTERN)==0) {
        transition();
      }
      patternmanager.update();
      framessincelevelchange++;
    }

    void transition() {
      //determine which pattern should be in use
      PATTERNS::PATTERN newpattern = getnewpattern();
      //if its different to the current pattern, update the patternmanager
      if(newpattern!=currentpattern) {
        patternmanager.transition(newpattern);
        currentpattern=newpattern;
      }
    }
    
    PATTERNS::PATTERN getnewpattern() {
      //each pattern is plated depending on how long its been since levelchange
      int patterncount = framessincelevelchange / FRAMES_PER_PATTERN;
      switch (patternset) {
        case 0:
          switch (patterncount % 1) {
            case 0:
              return PATTERNS::EYE;
          }; break;
        case 1:
          switch (patterncount % 3) {
            case 0:
              return PATTERNS::ORNAMENTS;
            case 1:
              return PATTERNS::FALLINGSTAR;
            case 2:
              return PATTERNS::DIAGONAL;
          }; break;
        case 2:
          switch (patterncount % 7) {
            case 0:
              return PATTERNS::RADIO;
            case 1:
              return PATTERNS::FIRE;
            case 2:
              return PATTERNS::CHASE1;
            case 3:
              return PATTERNS::BOUNCINGBALL;
            case 4:
              return PATTERNS::LOUDNESS;
            case 5:
              return PATTERNS::SPARKLE;
            case 6:
              return PATTERNS::FIREWORKS;
          }; break;
      }
    }

    //use new patternset
    void newlevel(int level) {
      patternset = level;
      framessincelevelchange = 0;
    }

};

LevelManager levelmanager = LevelManager();

