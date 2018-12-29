/*
 * A library of pretty patterns for the tree to display
 */
#include "UtilityPatterns.h"

/*
 * Blank (black) pattern
 */
class Blank: public Pattern {
  public: Blank() {
    }

    void setup() {
    }

    void update(CRGB ledbuffer[]) {
      Pattern::update(ledbuffer);
    }
};

/*
 * displays a "Christmas Tree", a green background, with slowly twinkling "ornaments"
 */
class Ornaments: public Pattern {

    //the number of ornaments
    const static int NO_ORNAMENTS = 32;
    //how long an ornament should be displayed, from off, fading in, and fading out
    const static int BLINK_DURATION = 120; //frames
    const CHSV background_colour = CHSV( 96, 255, 64);

    //the colour of each ornament
    CHSV ornament_colour[NO_ORNAMENTS];
    //led position of each ornament
    int ornament_position[NO_ORNAMENTS];
    //a frame counter used to determine the brightness of each ornament
    int ornament_brightness[NO_ORNAMENTS];
    //RGB representation of background colour
    CRGB background_colour_rgb = background_colour;

  public:
    Ornaments() {
    }

    //Return true if an ornament already exists at t.
    bool testposition(int t) {
      for (int i = 0; i < NO_ORNAMENTS; i++) {
        if (ornament_position[i] == t) return true;
      }
      return false;
    }

    //Randomise values for a new ornament.
    void randomise(int i) {
      ornament_colour[i] = CHSV( random8(), 255, 64);
      int pos;
      while (testposition(pos = random(NUM_LEDS)));
      ornament_position[i] = pos;
    }

    //randomise all ornaments. Setting brightness negative causes a delay before initial display of ornaments
    virtual void setup() {
      for (int i = 0; i < NO_ORNAMENTS; i++) {
        ornament_brightness[i] = -random(BLINK_DURATION);
        randomise(i);
      }
    }

    virtual void update(CRGB ledbuffer[]) {
      //set background
      for (int i = 0; i < NUM_LEDS; i++) {
        ledbuffer[i] = background_colour_rgb;
      }
      for (int i = 0; i < NO_ORNAMENTS; i++) {
        //if brightness negative, dont show it yet
        if (ornament_brightness[i] < 0) {
          ornament_brightness[i] += 1;
          continue;
        }
        //increment brightness
        ornament_brightness[i] = (ornament_brightness[i] + 1) % BLINK_DURATION;
        //use a sin curve to transition ornament brightness
        int transition = sin8((map(ornament_brightness[i], 0, BLINK_DURATION - 1, 0, 255) + 192) % 256);
        CHSV b = background_colour;
        //fade led from background colour
        ornament_colour[i].value = transition;
        //generate ornament colour, with appropriate brightness
        b.value = (b.value * 255 - b.value * transition) / 256;
        //fade the two together
        ledbuffer[ornament_position[i]] = b + ornament_colour[i];
        if (ornament_brightness[i] == 0) {
          //if brightness has been reset, generate at a new location
          randomise(i);
        }
      }
    }
};

/*
 * Shows random shooting stars, moving vertically down the tree, fading in then out again.
 * A fading tail streaks behind the star.
 */
class FallingStar: public Pattern {

    //The number of shooting starts scheduled at any time
    const static int NO_ORNAMENTS = 10;
    //possible number of frames to wait between shoots
    const static int DELAY = 30 * 5; //frames
    //how many frames the star should spend falling
    const static int FALL_DURATION = 20; //frames
    //how long the streak should last
    const static int FADE_DURATION = 30; //frames
    //the minimum start height of each star
    const static int MIN_HEIGHT = 10;

    //the start frame for the star
    int ornament_startframe[NO_ORNAMENTS];
    //the row to display the star on
    int ornament_row[NO_ORNAMENTS];
    //the start height 
    int ornament_height[NO_ORNAMENTS];
    //fade the brightness up to this value
    int ornament_max_brightness[NO_ORNAMENTS];
    //star colour
    int ornament_hue[NO_ORNAMENTS];
    //star saturation
    int ornament_sat[NO_ORNAMENTS];

    const static int PULSE_TIME = 256;

  public:
    FallingStar() {
    }

    void randomise(int i) {
      //set star to shoot some time in the future
      ornament_startframe[i] = framenumber + random(DELAY);
      //on a random row
      ornament_row[i] = random(ROWS);
      //at a random height
      ornament_height[i] = MIN_HEIGHT + random(LEDS_PER_ROW);
      //maximum brightness
      ornament_max_brightness[i] = 128 + random8(127);
      //red-yellow-orange colour
      ornament_hue[i] = random8(64);
      //mostly saturated
      ornament_sat[i] = 128 + random8(127);
    }

    virtual void setup() {
      for (int i = 0; i < NO_ORNAMENTS; i++) {
        randomise(i);
      }
    }

    virtual void update(CRGB ledbuffer[]) {
      //blank canvas
      Pattern::update(ledbuffer);
      for (int i = 0; i < NO_ORNAMENTS; i++) {
        //if start time in the future, dont display anything
        if (ornament_startframe[i] > framenumber) {
          continue;
        }
        //for this star, how much time should it spend falling. Dependant on maxumum brighness
        int fall = FALL_DURATION * ornament_max_brightness[i] / 255;
        //for this star, how much time should it spend fading
        int fade = FADE_DURATION * ornament_max_brightness[i] / 255;

        //total number of frames
        int animationframes = fall + fade;
        //which frame are we up to
        int ornamentframe = framenumber - ornament_startframe[i];
        int framesremaining = animationframes - ornamentframe;

        if (!(framesremaining > 0)) {
          //Were done, reset star
          randomise(i);
          continue;
        }
        //draw the tail. we count pixels from 0 to how many frames have passed (up to fall frames)
        for (int j = 0; j < min(ornamentframe, fall) - 1; j++) {
          int height = ornament_height[i] - j;
          //brightess of tail starts as a sin curve
          unsigned int bright = sin8((map(j, 0, fall - 1, 0, 255) + 192) % 256);
          //dim the tail depending on max brightness
          bright = bright * ornament_max_brightness[i] / 255;
          //we want to fade the led depending on how long it has been displayed
          int fade_led = max(0, fade - ornamentframe + j + 1);
          bright = bright * fade_led / fade;
          setLed(ledbuffer, ornament_row[i], height, CHSV(ornament_hue[i], ornament_sat[i], bright));
        }
        if (ornamentframe < fall) {
          //display the star, its brightness determined by a sin curve and the max brightness
          int height = ornament_height[i] - ornamentframe;
          unsigned int bright = sin8((map(ornamentframe, 0, fall - 1, 0, 255) + 192) % 256);
          bright = bright * ornament_max_brightness[i] / 255;
          //the star has lower saturation, making it whiter and brighter
          setLed(ledbuffer, ornament_row[i], height, CHSV(ornament_hue[i], ornament_sat[i] / 2, bright));
        }
      }
      int sat = map(sin8((framenumber%PULSE_TIME)*256/PULSE_TIME), 0,255, 128, 0);
      int bri = map(sin8((framenumber%PULSE_TIME)*256/PULSE_TIME), 0,255, 128, 192);
      for (int i = NUM_LEDS_TREE; i < NUM_LEDS; i++) {
        ledbuffer[i]=CHSV(32, sat, bri);
      }
    }
    void setLed(CRGB ledbuffer[], int row, int height, CRGB colour) {
      //we dont display anything if its height is out of range
      if (!(row >= 0 && row < ROWS && height >= 0 & height < LEDS_PER_ROW)) return;
      ledbuffer[ledid(row, height)] = colour;
    }
};


/*
 * red green and blue dots chace alternately up and down each strip.
 */
class Chase1: public Pattern {
    int position;

  public:
    Chase1() {
    }

    virtual void setup() {
      position = 0;
    }

    virtual void update(CRGB ledbuffer[]) {
      Pattern::update(ledbuffer);
      for (int i = 0; i < ROWS; i++) {
        ledbuffer[(position + LEDS_PER_ROW * i + 0) % int(NUM_LEDS_TREE)] = CRGB::Red;
        ledbuffer[(position + LEDS_PER_ROW * i + 1) % int(NUM_LEDS_TREE)] = CRGB::Green;
        ledbuffer[(position + LEDS_PER_ROW * i + 2) % int(NUM_LEDS_TREE)] = CRGB::Blue;
      }
      position = (position + 1) % LEDS_PER_ROW;
    }

};

/*
 * single led chases around the bottom row, then around the second, and so on up the tree.
 */
class Chase2: public Pattern {

    int position;

  public: Chase2() {
    }

    virtual void setup() {
      position = 0;
    }

    virtual void update(CRGB ledbuffer[]) {
      Pattern::update(ledbuffer);
      position = (position + 1) % int(NUM_LEDS_TREE);
      ledbuffer[ledid(position % ROWS, position / ROWS)] = CRGB::White;
    }

};

/*
 * randomly light up a single vertical "row"
 */
class FlashRow: public Pattern {

    //the number of frames before the strip moves
    const int FRAMES_CYCLE = 12;
    //the number of frames in the cycle the strip is on for
    const int FRAMES_ON = 12;
    int row;
    CHSV colour;

  public:
    FlashRow() {
    }

    virtual void setup() {
    }

    virtual void update(CRGB ledbuffer[]) {
      Pattern::update(ledbuffer);
      if (framenumber % FRAMES_CYCLE == 0) {
        row = random(ROWS);
        colour = CHSV( random8(), random8(), 64);
      }
      if (framenumber % FRAMES_CYCLE < FRAMES_ON) {
        for (int i = 0; i < LEDS_PER_ROW; i++) {
          ledbuffer[ledid(row, i)] = colour;
        }
      }
    }

};

/*
 * randomly light up a single ring around the tree
 */
class FlashRing: public Pattern {

    //the number of frames before the ring moves
    const int FRAMES_CYCLE = 12;
    //the number of frames in the cycle the ring is on for
    const int FRAMES_ON = 12;
    int row;
    CHSV colour;

  public:
    FlashRing() {
    }

    virtual void setup() {
    }

    virtual void update(CRGB ledbuffer[]) {
      Pattern::update(ledbuffer);
      if (framenumber % FRAMES_CYCLE == 0) {
        row = random(LEDS_PER_ROW);
        colour = CHSV( random8(), random8(), 255);
      }
      if (framenumber % FRAMES_CYCLE < FRAMES_ON) {
        for (int i = 0; i < ROWS; i++) {
          ledbuffer[ledid(i, row)] = colour;
        }
      }
    }

};

/*
 * alternating green and red rings chase up the tree
 */
class ChristmasRadio: public Pattern {

    //how many rings
    const int RING_NUMBER = 5;
    // distance between each ring
    const int RING_SPACER = 2;
    //number of frames each state is displayed for before moving the rings
    const int RING_SPEED = 2;

    int ROW_COUNT = 0;

    const int LEDS_IN_SEQUENCE = LEDS_PER_ROW+8;

  public:
    ChristmasRadio() {
    }

    virtual void setup() {
    }

    virtual void update(CRGB ledbuffer[]) {
      Pattern::update(ledbuffer);
      CRGB RING_COLOUR;
      for (int RING = 0; RING < RING_NUMBER; RING++) {
        if (RING % 2 == 0) {
          RING_COLOUR = CRGB::Green;
        } else {
          RING_COLOUR = CRGB::Red;
        }
        setring(ledbuffer, (ROW_COUNT + RING * RING_SPACER) % LEDS_IN_SEQUENCE, RING_COLOUR);
      }
      if (framenumber % RING_SPEED == 0) {
        ROW_COUNT = (ROW_COUNT + 1) % LEDS_IN_SEQUENCE;
      }
    }

    virtual void setring(CRGB ledbuffer[], int pos, CRGB color) {
      if(pos<LEDS_PER_ROW) {
        for (int i = 0; i < ROWS; i++) {
          ledbuffer[ledid(i, pos)] = color;
        }
      } else {
        fillstar(ledbuffer, color, pos-LEDS_PER_ROW);
      }
    }
};

/*
 * a small pulsating circle. Used as an "off" state
 */
class Eye: public Pattern {

  int audiolevel;

  public:
    Eye() {
    }

    virtual void setup() {
    }

    virtual void update(CRGB ledbuffer[]) {
      Pattern::update(ledbuffer);
      int diff = soundlevel.getAudioLevel() - audiolevel;
      diff = constrain(diff, -2, 2);
      audiolevel += diff;
      int bright_a = 255;
      int bright_b = 96;
      int bright_c = 72;
      int sat_a = 255;
      int sat_b = 96;
      int sat_c = 72;
      //vary the brightness
      int bright = sin8((map(framenumber % 120, 0, 120 - 1, 0, 255) + 192) % 256);
      bright = map(bright, 0, 255, 192, 255);
      bright_a = bright_a * bright / 255;
      bright_b = bright_b * bright / 255;
      bright_c = bright_c * bright / 255;
      int sat = map(constrain(audiolevel, 0, 100), 0, 100, 0, 255);
      sat_a = sat_a * sat / 255;
      sat_b = sat_b * sat / 255;
      sat_c = sat_c * sat / 255;

      ledbuffer[ledid(8+1, 15)] = CHSV(0, sat_a, bright_a);
      ledbuffer[ledid(8+0, 15)] = CHSV(0, sat_b, bright_b);
      ledbuffer[ledid(8+2, 15)] = CHSV(0, sat_b, bright_b);
      ledbuffer[ledid(8+1, 14)] = CHSV(0, sat_b, bright_b);
      ledbuffer[ledid(8+1, 16)] = CHSV(0, sat_b, bright_b);
      ledbuffer[ledid(8+0, 14)] = CHSV(0, sat_c, bright_c);
      ledbuffer[ledid(8+0, 16)] = CHSV(0, sat_c, bright_c);
      ledbuffer[ledid(8+2, 14)] = CHSV(0, sat_c, bright_c);
      ledbuffer[ledid(8+2, 16)] = CHSV(0, sat_c, bright_c);
    }
};

/*
 * A fire effect
 * adapted from http://www.tweaking4all.com/hardware/arduino/adruino-led-strip-effects/
 */

class Fire: public Pattern {

    const static byte NOROWS = 4;
    byte heat[NOROWS][LEDS_PER_ROW];
    const int Cooling = 55;
    const int Sparking = 120;
    int cooldown;
    
  public:
    Fire() {
    }

    virtual void setup() {
    }

    virtual void update(CRGB ledbuffer[]) {
      clear_star(ledbuffer);
      cooldown = random(0, ((Cooling * 10) / LEDS_PER_ROW) + 2);
      for (int i = 0; i < NOROWS; i++)
        updaterow(i, ledbuffer);
    }

    virtual void updaterow(int row, CRGB ledbuffer[]) {

      // Step 1.  Cool down every cell a little
      for ( int i = 0; i < LEDS_PER_ROW; i++) {
        if (cooldown > heat[row][i]) {
          heat[row][i] = 0;
        } else {
          heat[row][i] = heat[row][i] - cooldown;
        }
      }

      // Step 2.  Heat from each cell drifts 'up' and diffuses a little
      for ( int k = LEDS_PER_ROW - 1; k >= 2; k--) {
        heat[row][k] = (heat[row][k - 1] + heat[row][k - 2]*2) / 3;
      }

      // Step 3.  Randomly ignite new 'sparks' near the bottom
      if ( random8(255) < Sparking ) {
        int y = random8(7);
        heat[row][y] = heat[row][y] + random8(160, 255);
      }

      // Step 4.  Convert heat to LED colors
      for ( int j = 0; j < LEDS_PER_ROW; j++) {
        setPixelHeatColor(row, ledbuffer, j, heat[row][j] );
      }
    }

    void setPixelHeatColor (int row, CRGB ledbuffer[], int Pixel, byte temperature) {
      // Scale 'heat' down from 0-255 to 0-191
      byte t192 = (int)temperature * 191 / 255;

      // calculate ramp up from
      byte heatramp = t192 & 0x3F; // 0..63
      heatramp <<= 2; // scale up to 0..252

      // figure out which third of the spectrum we're in:
      CRGB colour;
      if ( t192 > 0x80) {                    // hottest
        colour = CRGB(255, 255, heatramp);
      } else if ( t192 > 0x40 ) {            // middle
        colour = CRGB(255, heatramp, 0);
      } else {                               // coolest
        colour = CRGB(heatramp, 0, 0);
      }
      for (int i = row; i < ROWS; i += NOROWS)
        ledbuffer[ledid(i, Pixel)] = colour;
    }
};

/*
 * Each strip shows a red green and blue bounding dot
 * adapted from http://www.tweaking4all.com/hardware/arduino/adruino-led-strip-effects/
 */
class BouncingBall: public Pattern {

    const byte NOROWS=8;
    CRGB colors[3] = {CRGB(0xff, 0, 0), CRGB(0xff, 0xff, 0xff), CRGB(0, 0, 0xff)};
    static const int BallCount = 3;
    float Gravity = -9.81;
    int StartHeight = 1;

    float Height[ROWS][BallCount];
    float ImpactVelocityStart = sqrt( -2 * Gravity * StartHeight );
    float ImpactVelocity[ROWS][BallCount];
    float TimeSinceLastBounce[ROWS][BallCount];
    int   Position[ROWS][BallCount];
    long  ClockTimeSinceLastBounce[ROWS][BallCount];
    float Dampening[ROWS][BallCount];

  public:
    BouncingBall() {
    }

    virtual void setup() {
      for (int row = 0; row < ROWS; row++) {
        for (int i = 0 ; i < BallCount ; i++) {
          ClockTimeSinceLastBounce[row][i] = millis();
          Height[row][i] = StartHeight;
          Position[row][i] = 0;
          ImpactVelocity[row][i] = ImpactVelocityStart;
          TimeSinceLastBounce[row][i] = 0;
          Dampening[row][i] = 0.90 - float(random8()) / 1024;
        }
      }
    }

    virtual void update(CRGB ledbuffer[]) {
      Pattern::update(ledbuffer);
      for (int row = 0; row < NOROWS; row++) {
        updaterow(row, ledbuffer);
        for (int i = 0 ; i < BallCount ; i++) {
           for(int repeat = row; repeat < ROWS; repeat+=NOROWS) {
            ledbuffer[ledid(repeat, Position[row][i])] = colors[i];
          }
        }
      }
    }

    virtual void updaterow(int row, CRGB ledbuffer[]) {
      for (int i = 0 ; i < BallCount ; i++) {
        TimeSinceLastBounce[row][i] =  millis() - ClockTimeSinceLastBounce[row][i];
        Height[row][i] = 0.5 * Gravity * pow( TimeSinceLastBounce[row][i] / 1000 , 2.0 ) + ImpactVelocity[row][i] * TimeSinceLastBounce[row][i] / 1000;

        if ( Height[row][i] < 0 ) {
          Height[row][i] = 0;
          ImpactVelocity[row][i] = Dampening[row][i] * ImpactVelocity[row][i];
          ClockTimeSinceLastBounce[row][i] = millis();

          if ( ImpactVelocity[row][i] < 0.01 ) {
            ImpactVelocity[row][i] = ImpactVelocityStart;
          }
        }
        Position[row][i] = round( Height[row][i] * (LEDS_PER_ROW - 1) / StartHeight);
      }

    }
};

/*
 * chases rings up the tree. The brightness/colour of each ring determined by the audio volume in the room.
 */
class Loudness: public Pattern {

    SlidingWindow history = SlidingWindow(LEDS_PER_ROW);

  public:
    Loudness() {
    }

    virtual void setup() {
    }

    virtual void update(CRGB ledbuffer[]) {
      int level = soundlevel.getLastVolume();
      history.push(level);
      for (int i = 0; i < LEDS_PER_ROW; i++) {
        //for each ring we get a historic volume level
        level = history.getVal(LEDS_PER_ROW - i);
        //set brightness based on volume
        int bright = map16(constrain(level, 30, 150), 30, 150, 0, 255);
        //saturation varies inversely to brightness. louder is whiter and brighter
        int sat = 255-bright;
        //colour varies from red to blue based on loudness
        int hue = map16(constrain(level, 30, 100), 30, 100, 255, 160);
        CRGB colour = CHSV(hue, sat, bright);
        for (int j = 0; j < ROWS; j++) {
          ledbuffer[ledid(j, i)] = colour;
        }
      }
    }
};

/*
 * randomly lights up a number of dots every frame.
 */
class Sparkle: public Pattern {

    //number of leds lit at any one time
    const int SPARKLES = 10;

  public:
    Sparkle() {
    }

    virtual void setup() {
    }

    virtual void update(CRGB ledbuffer[]) {
      Pattern::update(ledbuffer);
      for(int i=0; i<SPARKLES; i++) {
        //1/4 leds are white, the remaining are coloured
        if(random8()<64) {
          ledbuffer[random(NUM_LEDS)]=CRGB::White;
        } else {
          ledbuffer[random(NUM_LEDS)]=CHSV(random8(), 255, 255);
        }
      }
    }
};

/*
 * shows a white band around the middle of the tree. The band moves up and down using subpixel rendering to gradually move up and down.
 * Based on https://plus.google.com/112916219338292742137/posts/2VYNQgD38Pw | http://pastebin.com/yAgKs0Ay
 */
class Wiggle: public Pattern {

    const int WIDTH = 2;

  public:
    Wiggle() {
    }

    virtual void setup() {
    }

    virtual void update(CRGB ledbuffer[]) {
      Pattern::update(ledbuffer);
      int height = 16 * (LEDS_PER_ROW)/2 - 255/4;
      height = height + sin8(framenumber*8)/2;
      //height of start of band
      int pos = height/16;
      //how bright the last ring should be
      int fractional = height & 0x0F;
      CRGB colour;
      for(int i=0; i<=WIDTH; i++) {
        if(i==0) {//first ring
          colour = CHSV( 0, 0, 255 - (fractional * 16));
        } else if(i==WIDTH) {//last ring
          colour = CHSV( 0, 0, fractional * 16);
        } else {//middle rings
          colour = CHSV( 0, 0, 255);
        }
        for(int row=0; row<ROWS; row++)
          ledbuffer[ledid(row, pos + i)] = colour;
      }
    }
};

/*
 * spins a spiral of colour around the tree
 */
class Diagonal: public Pattern {

  public:
    Diagonal() {
    }

    virtual void setup() {
    }

    virtual void update(CRGB ledbuffer[]) {
      //we calculate the colour of each led
      //constant for later calculation
      int c = 255 * 2 / ROWS;
      //start by varying all colours by frame
      int v1 =  255 - (framenumber % 64) * 4;
      for(int l = 0; l<LEDS_PER_ROW; l++) {
        //vary the colour depending on height
        int v2 = l*ROWS/2;
        for(int row = 0; row<ROWS; row++) {
          //vary colour based on row
          int v3 = row * c + v2 + v1;
          if(v3>256) v3 = v3 % 256;
          ledbuffer[ledid(row, l)]=CHSV(v3, 255, 64);
        }
      }
      copy_to_star(ledbuffer, 0);
    }
};

/*
 * shows random fireworks.
 * Each firework shoots up, explodes in a flash, before stars twinkle down and fade out.
 */
class Fireworks: public Pattern {

  //how many fireworks to have queued at any time
  static const int NO_FIREWORKS = 6;
  //how many frames the firework should shoot for
  const int SHOOT = 40;
  //how many frames the firework should explode for
  const int EXPLODE = 3;
  //how long the firework should take to fall
  const int FALL = 60;

  //start frame for each forework
  long startframe[NO_FIREWORKS];
  //row firework starts from
  int row[NO_FIREWORKS];
  //colour of rocket
  CHSV shootcolour[NO_FIREWORKS];
  //colour of burst
  CHSV burstcolour[NO_FIREWORKS];

  public:
    Fireworks() {
    }

    virtual void randomise(int i) {
      //start time is up to 5 seconds in the future
      startframe[i] = framenumber + random(30*5);
      row[i] = random(ROWS);
      //rocket is red/orange/yellow
      shootcolour[i] = CHSV(random8(64), 255-random8(64), 255);
      burstcolour[i] = CHSV(random8(), 255-random8(64), 255);
    }

    virtual void setup() {
      for(int i = 0; i<NO_FIREWORKS; i++) {
        randomise(i);
      }
    }

    virtual void update(CRGB ledbuffer[]) {
      Pattern::update(ledbuffer);
      for(int i = 0; i<NO_FIREWORKS; i++) {
        //dont display if firework hasn't launched yet
        if(framenumber<startframe[i]) continue;
        int frame = framenumber - startframe[i];
        if(frame < SHOOT) { // rocket phase
          //calculate height with an inverse parabola. y = 1 - x^2, from x=-1..1, and y is scaled to tree height
          unsigned int height = 256*(SHOOT-frame)/SHOOT;
          height *= height;
          height = (65536 - height) / 256 * LEDS_PER_ROW / 256;
          CHSV c = shootcolour[i];
          //draw a bright dot
          ledbuffer[ledidC(row[i], height)]=c;
          //draw two leds for a tail, each half the brightness of the last
          c.value = 128;
          ledbuffer[ledidC(row[i], height-1)]=c;
          c.value = 64;
          ledbuffer[ledidC(row[i], height-2)]=c;
          //if we have already reached the top of the tree, jump ahead
          if(height==LEDS_PER_ROW-1) startframe[i] = framenumber - SHOOT - 1;
        } else if(frame < SHOOT + EXPLODE) { // initial explosion
          frame-=SHOOT;
          //we draw a white circle if increasing size
          int circlesize = (frame+1);
          for(int x = -circlesize; x<circlesize; x++) {
            for(int y = -circlesize; y<=0; y++) {
              //if we are outside the circle, do nothing
              if((x*x + y*y)>circlesize*circlesize) continue;
              //circle centre
              int o_x = row[i];
              int o_y = LEDS_PER_ROW - 1;
              ledbuffer[ledidC(o_x + x, o_y + y)]=CRGB::White;
            }
          }
        } else if(frame < SHOOT + EXPLODE + 8) {//explosion
          frame-=SHOOT;
          //falling shows a circle of randomly lit leds
          int circlesize = (frame+1);
          if(circlesize>4) circlesize=4;
          int height = LEDS_PER_ROW - (LEDS_PER_ROW-1) * frame / FALL;
          for(int x = -circlesize; x<circlesize; x++) {
            for(int y = -circlesize; y<circlesize; y++) {
              //if we are outside the circle, do nothing
              if((x*x + y*y)>(frame+1)*(frame+1)) continue;
              //determine led colour
              CHSV c1 = burstcolour[i];
              //becomes dimmer as circle expands
              c1.value = 256>>(circlesize/2);
              //and is a random brightness
              c1.value = random8(c1.value*2);
              int o_x = row[i];
              int o_y = height;
              ledbuffer[ledidC(o_x + x, o_y + y)]=c1;
            }
          }
        } else if(frame < SHOOT + EXPLODE + FALL) {
          frame-=SHOOT;
          //falling shows a circle of randomly lit leds, with white sparkles
          int circlesize = (frame+1);
          if(circlesize>4) circlesize=4;
          int height = LEDS_PER_ROW - (LEDS_PER_ROW-1) * frame / FALL;
          for(int x = -circlesize; x<circlesize; x++) {
            for(int y = -circlesize; y<circlesize; y++) {
              //if we are outside the circle, do nothing
              if((x*x + y*y)>circlesize*circlesize) continue;
              //determine led colour
              CHSV c1 = burstcolour[i];
              //not too bright
              c1.value = 64;
              //randomise brightness
              c1.value = random8(c1.value*2);
              //1 in 16 times we add a white sparkle
              if(random8()>240) c1 = CHSV(0, 0, 255);
              //and we fade out the lower we get
              c1.value = c1.value * height / LEDS_PER_ROW ;
              int o_x = row[i];
              int o_y = height;
              ledbuffer[ledidC(o_x + x, o_y + y)]=c1;
            }
          }
        } else {
          randomise(i);
        }
      }
    }
};

/*
 * stripes with random colors
 */
class AltStripes: public Pattern {

    const static int NO_STRIPES = ROWS/2;
    const CHSV background_colour = CHSV( 96, 255, 64);
    const int frames_between_column_change = 30*5;
    const int fade_frames = 30*1;

    CHSV stripes[NO_STRIPES];
    long next_column_change;
    int next_column_to_change;

    CHSV points[STAR_POINTS];
    long next_point_change;
    int next_point_to_change;

  public:
    AltStripes() {
    }

    virtual int randomise() {
      //avoid 64-140
      #define diff (140-64)
      return (random(255-diff)+140)%256;
    }

    virtual int randomise(int oldval) {
      int hue;
      do {
        hue = randomise();
      } while(abs(hue-oldval)<30);
      return hue;
    }

    virtual void setup() {
      for (int i = 0; i < NO_STRIPES; i++) {
        stripes[i] = CHSV(randomise(), 255, 128);
      }
      next_column_change = framenumber+frames_between_column_change;
      next_column_to_change=random(NO_STRIPES);

      for (int i = 0; i < STAR_POINTS; i++) {
        points[i] = CHSV(randomise(), 255, 128);
      }
      next_point_change = framenumber+frames_between_column_change*1.5;
      next_point_to_change=random(STAR_POINTS);
    }

    virtual void update(CRGB ledbuffer[]) {
      CHSV color;
      for (int i = 0; i < NO_STRIPES; i++) {
        for (int l = 0; l < LEDS_PER_ROW; l++) {
          ledbuffer[ledid(i*2,l)] = background_colour;
        }
        color = stripes[i];
        if(i== next_column_to_change && framenumber>=next_column_change) {
          if(framenumber==next_column_change+fade_frames*2) {
            next_column_change = framenumber + frames_between_column_change - fade_frames*2;
            next_column_to_change = (next_column_to_change+1)%NO_STRIPES;
          } else {
            color.value=color.value*(1.0*abs(next_column_change+fade_frames-framenumber)/fade_frames);
            if(framenumber==next_column_change+fade_frames) {
              stripes[i] = CHSV(randomise(stripes[i].hue), 255, 128);
            }
          }
        }
        for (int l = 0; l < LEDS_PER_ROW; l++) {
          ledbuffer[ledid(i*2+1,l)] = color;
        }
      }
      for(int i = starid(1,0, 0); i <= starid(0,0,1); i++) {
        leds[i]=background_colour;
      }
      for (int i = 0; i < STAR_POINTS; i++) {
        color = points[i];
        if(i== next_point_to_change && framenumber>=next_point_change) {
          if(framenumber==next_point_change+fade_frames*2) {
            next_point_change = framenumber + frames_between_column_change - fade_frames*2;
            next_point_to_change = (next_point_to_change+1)%STAR_POINTS;
          } else {
            color.value=color.value*(1.0*abs(next_point_change+fade_frames-framenumber)/fade_frames);
            if(framenumber==next_point_change+fade_frames) {
              points[i] = CHSV(randomise(points[i].hue), 255, 128);
            }
          }
        }
        for (int l = 0; l < STAR_POINT_LEDS*2; l++) {
          ledbuffer[starid(2, i, l)] = color;
        }
      }
    }
};

/*
 * stripes with random colors
 */
class SwirlPaint: public Pattern {

    const static int STRIPE_WIDTH = 4;
    const static int NO_STRIPES = ROWS/STRIPE_WIDTH;
    constexpr static float swirl_twist_multiplier = 1.2;
    const int frames_between_column_change = 30*4;
    const int fade_frames = 40;

    CHSV stripes[NO_STRIPES];
    CHSV new_stripes[NO_STRIPES];
    long next_column_change;
    int next_column_to_change;

    CHSV star;
    CHSV new_star;
    const int star_fade_frames = 15;

  public:
    SwirlPaint() {
    }

    virtual int randomise() {
      return random(255);
    }

    virtual int randomise(int oldval) {
      int hue;
      do {
        hue = randomise();
      } while(abs(hue-oldval)<40);
      return hue;
    }

    virtual int randomise(int oldval1, int oldval2) {
      int hue;
      do {
        hue = randomise(oldval1);
      } while(abs(hue-oldval2)<40);
      return hue;
    }

    virtual int randomise(int oldval1, int oldval2, int oldval3) {
      int hue;
      do {
        hue = randomise(oldval1, oldval2);
      } while(abs(hue-oldval3)<40);
      return hue;
    }

    virtual void setup() {
      stripes[0] = CHSV(randomise(), 255, 128);
      for (int i = 1; i < NO_STRIPES-1; i++) {
        stripes[i] = CHSV(randomise(stripes[i-1].hue), 255, 128);
      }
      stripes[NO_STRIPES-1] = CHSV(randomise(stripes[0].hue, stripes[NO_STRIPES-2].hue), 255, 128);
      next_column_change = framenumber+frames_between_column_change;
      next_column_to_change=random(NO_STRIPES);
      star=CHSV(0,0,0);
    }
    
    virtual void update(CRGB ledbuffer[]) {
      CHSV *stripe_set;
      int transition_pos = (next_column_change-star_fade_frames-framenumber)*LEDS_PER_ROW/fade_frames;
      if(framenumber==next_column_change-fade_frames-star_fade_frames) {
        for(int i = 0; i < NO_STRIPES; i++) new_stripes[i]=stripes[i];
        new_stripes[next_column_to_change].hue=randomise(
              new_stripes[next_column_to_change].hue,
              new_stripes[(next_column_to_change-1)%NO_STRIPES].hue,
              new_stripes[(next_column_to_change+1)%NO_STRIPES].hue
              );
        new_star=new_stripes[next_column_to_change];
      } else if(framenumber==next_column_change) {
        for(int i = 0; i < NO_STRIPES; i++) stripes[i]=new_stripes[i];
        star=new_star;
        next_column_change = framenumber + frames_between_column_change;
        next_column_to_change = (next_column_to_change+1)%NO_STRIPES;
      }
      for (int h = 0; h < LEDS_PER_ROW; h++) {
        if(framenumber>=next_column_change-fade_frames-star_fade_frames) {
          if(LEDS_PER_ROW-h>transition_pos) {
            stripe_set=new_stripes;
          } else {
            stripe_set=stripes;
          }
        } else {
          stripe_set = stripes;
        }
        int hswirl = h/swirl_twist_multiplier;
        for (int r = 0; r < ROWS; r++) {
          ledbuffer[ledid(r,h)] = stripe_set[((r+hswirl)/STRIPE_WIDTH)%NO_STRIPES];
        }
      }
      transition_pos = -(next_column_change-star_fade_frames-framenumber)*8/star_fade_frames;
      for(int r = 0; r < 8; r++)
        if(transition_pos>r) {
          fillstar(ledbuffer, new_star, r);
        } else {
          fillstar(ledbuffer, star, r);
        }
    }
};

/*
 * test
 */
class TestPattern: public Pattern {
    int position;

  public:
    TestPattern() {
    }

    virtual void setup() {
      position = 0;
    }

    virtual void update(CRGB ledbuffer[]) {
      Pattern::update(ledbuffer);
      if(position%2==0) ledbuffer[starid(1,position/2, 0)]=CRGB::White;
      else              ledbuffer[starid(1,position/2, 2)]=CRGB::White;
      position = (position + 1) % (STAR_POINTS*2);
    }
};
