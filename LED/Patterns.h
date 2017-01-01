#include "UtilityPatterns.h"

class Pattern {
  public:
    virtual void setup() {
    }

    virtual void update(CRGB ledbuffer[]) {
      for (int i = 0; i < NUM_LEDS; i++) {
        ledbuffer[i] = CRGB::Black;
      }
    }
};

class Blank: public Pattern {
  public: Blank() {
    }

    void setup() {
    }

    void update(CRGB ledbuffer[]) {
      Pattern::update(ledbuffer);
    }
};

class Ornaments: public Pattern {

    const static int NO_ORNAMENTS = 32;
    const static int BLINK_DURATION = 120; //frames
    const CHSV background_colour = CHSV( 96, 255, 64);

    CHSV ornament_colour[NO_ORNAMENTS];
    int ornament_position[NO_ORNAMENTS];
    int ornament_brightness[NO_ORNAMENTS];
    CRGB background_colour_rgb = background_colour;

  public:
    Ornaments() {
    }

    bool testposition(int t) {
      for (int i = 0; i < NO_ORNAMENTS; i++) {
        if (ornament_position[i] == t) return true;
      }
      return false;
    }

    void randomise(int i) {
      ornament_colour[i] = CHSV( random8(), 255, 64);
      int pos;
      while (testposition(pos = random(NUM_LEDS)));
      ornament_position[i] = pos;
    }

    virtual void setup() {
      for (int i = 0; i < NO_ORNAMENTS; i++) {
        ornament_brightness[i] = -random(BLINK_DURATION);
        randomise(i);
      }
    }

    virtual void update(CRGB ledbuffer[]) {
      for (int i = 0; i < NUM_LEDS; i++) {
        ledbuffer[i] = background_colour_rgb;
      }
      for (int i = 0; i < NO_ORNAMENTS; i++) {
        if (ornament_brightness[i] < 0) {
          ornament_brightness[i] += 1;
          continue;
        }
        ornament_brightness[i] = (ornament_brightness[i] + 1) % BLINK_DURATION;
        int transition = sin8((map(ornament_brightness[i], 0, BLINK_DURATION - 1, 0, 255) + 192) % 256);
        CHSV b = background_colour;
        ornament_colour[i].value = transition;
        b.value = (b.value * 255 - b.value * transition) / 256;
        ledbuffer[ornament_position[i]] = b + ornament_colour[i];
        if (ornament_brightness[i] == 0) {
          randomise(i);
        }
      }
    }
};

class FallingStar: public Pattern {

    const static int NO_ORNAMENTS = 10;
    const static int DELAY = 30 * 5; //frames
    const static int FALL_DURATION = 20; //frames
    const static int FADE_DURATION = 30; //frames
    const static int MIN_HEIGHT = 10;

    int ornament_startframe[NO_ORNAMENTS];
    int ornament_row[NO_ORNAMENTS];
    int ornament_height[NO_ORNAMENTS];
    int ornament_max_brightness[NO_ORNAMENTS];
    int ornament_hue[NO_ORNAMENTS];
    int ornament_sat[NO_ORNAMENTS];

  public:
    FallingStar() {
    }

    void randomise(int i) {
      ornament_startframe[i] = framenumber + random(DELAY);
      ornament_row[i] = random(ROWS);
      ornament_height[i] = MIN_HEIGHT + random(LEDS_PER_ROW);
      ornament_max_brightness[i] = 128 + random8(127);
      ornament_hue[i] = random8(64);
      ornament_sat[i] = random8(128);
    }

    virtual void setup() {
      for (int i = 0; i < NO_ORNAMENTS; i++) {
        randomise(i);
      }
    }

    virtual void update(CRGB ledbuffer[]) {
      Pattern::update(ledbuffer);
      for (int i = 0; i < NO_ORNAMENTS; i++) {
        if (ornament_startframe[i] > framenumber) {
          continue;
        }
        int fall = FALL_DURATION * ornament_max_brightness[i] / 255;
        int fade = FADE_DURATION * ornament_max_brightness[i] / 255;

        int animationframes = fall + fade;
        int ornamentframe = framenumber - ornament_startframe[i];
        int framesremaining = animationframes - ornamentframe;

        if (!(framesremaining > 0)) {
          randomise(i);
          continue;
        }
        for (int j = 0; j < min(ornamentframe, fall) - 1; j++) {
          int height = ornament_height[i] - j;
          unsigned int bright = sin8((map(j, 0, fall - 1, 0, 255) + 192) % 256);
          bright = bright * ornament_max_brightness[i] / 255;
          int fade_led = max(0, fade - ornamentframe + j + 1);
          bright = bright * fade_led / fade;
          setLed(ledbuffer, ornament_row[i], height, CHSV(ornament_hue[i], ornament_sat[i], bright));
        }
        if (ornamentframe < fall) {
          int height = ornament_height[i] - ornamentframe;
          unsigned int bright = sin8((map(ornamentframe, 0, fall - 1, 0, 255) + 192) % 256);
          bright = bright * ornament_max_brightness[i] / 255;
          setLed(ledbuffer, ornament_row[i], height, CHSV(ornament_hue[i], ornament_sat[i] / 2, bright));
        }
      }
    }
    void setLed(CRGB ledbuffer[], int row, int height, CRGB colour) {
      if (!(row >= 0 && row < ROWS && height >= 0 & height < LEDS_PER_ROW)) return;
      ledbuffer[ledid(row, height)] = colour;
    }
};


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
        ledbuffer[(position + LEDS_PER_ROW * i + 0) % int(NUM_LEDS)] = CRGB::Red;
        ledbuffer[(position + LEDS_PER_ROW * i + 1) % int(NUM_LEDS)] = CRGB::Green;
        ledbuffer[(position + LEDS_PER_ROW * i + 2) % int(NUM_LEDS)] = CRGB::Blue;
      }
      position = (position + 1) % LEDS_PER_ROW;
    }

};

class Chase2: public Pattern {

    int position;

  public: Chase2() {
    }

    virtual void setup() {
      position = 0;
    }

    virtual void update(CRGB ledbuffer[]) {
      Pattern::update(ledbuffer);
      position = (position + 1) % int(NUM_LEDS);
      ledbuffer[ledid(position % ROWS, position / ROWS)] = CRGB::White;
    }

};

class FlashRow: public Pattern {

    const int FRAMES_CYCLE = 12;
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

class FlashRing: public Pattern {

    const int FRAMES_CYCLE = 12;
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


class ChristmasRadio: public Pattern {

    const int RING_NUMBER = 5;
    const int RING_SPACER = 2;
    const int RING_SPEED = 2;

    int ROW_COUNT = 0;

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
        for (int i = 0; i < ROWS; i++) {
          ledbuffer[ledid(i, (ROW_COUNT + RING * RING_SPACER) % LEDS_PER_ROW)] = RING_COLOUR;
        }
      }
      if (framenumber % RING_SPEED == 0) {
        ROW_COUNT = (ROW_COUNT + 1) % LEDS_PER_ROW;
      }
    }
};

class Eye: public Pattern {

  public:
    Eye() {
    }

    virtual void setup() {
    }

    virtual void update(CRGB ledbuffer[]) {
      Pattern::update(ledbuffer);
      int a = 255;
      int b = 96;
      int c = 72;
      int transition = sin8((map(framenumber % 120, 0, 120 - 1, 0, 255) + 192) % 256);
      transition = map(transition, 0, 255, 192, 255);
      a = a * transition / 255;
      b = b * transition / 255;
      c = c * transition / 255;

      ledbuffer[ledid(1, 15)] = CHSV(0, 0, a);
      ledbuffer[ledid(0, 15)] = CHSV(0, 0, b);
      ledbuffer[ledid(2, 15)] = CHSV(0, 0, b);
      ledbuffer[ledid(1, 14)] = CHSV(0, 0, b);
      ledbuffer[ledid(1, 16)] = CHSV(0, 0, b);
      ledbuffer[ledid(0, 14)] = CHSV(0, 0, c);
      ledbuffer[ledid(0, 16)] = CHSV(0, 0, c);
      ledbuffer[ledid(2, 14)] = CHSV(0, 0, c);
      ledbuffer[ledid(2, 16)] = CHSV(0, 0, c);
    }
};


class Fire: public Pattern {

    const byte NOROWS = 4;
    byte heat[ROWS][LEDS_PER_ROW];
    const int Cooling = 55;
    const int Sparking = 120;
    int cooldown;
    
  public:
    Fire() {
    }

    virtual void setup() {
    }

    virtual void update(CRGB ledbuffer[]) {
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

class Loudness: public Pattern {

    SlidingWindow history = SlidingWindow(LEDS_PER_ROW);

  public:
    Loudness() {
    }

    virtual void setup() {
    }

    virtual void update(CRGB ledbuffer[]) {
      int level = soundlevel.getLastVolume() - 50;
      history.push(level);
      for (int i = 0; i < LEDS_PER_ROW; i++) {
        level = history.getVal(LEDS_PER_ROW - i);
        int bright = map16(constrain(level, -20, 100), -20, 100, 0, 255);
        int sat = 255-bright;
        int hue = map16(constrain(level, -20, 50), -20, 50, 255, 160);
        CRGB colour = CHSV(hue, sat, bright);
        for (int j = 0; j < ROWS; j++) {
          ledbuffer[ledid(j, i)] = colour;
        }
      }
    }
};

class Sparkle: public Pattern {

    const int SPARKLES = 10;

  public:
    Sparkle() {
    }

    virtual void setup() {
    }

    virtual void update(CRGB ledbuffer[]) {
      Pattern::update(ledbuffer);
      for(int i=0; i<SPARKLES; i++) {
        if(random8()<64) {
          ledbuffer[random(NUM_LEDS)]=CRGB::White;
        } else {
          ledbuffer[random(NUM_LEDS)]=CHSV(random8(), 255, 255);
        }
      }
    }
};

class Wiggle: public Pattern {

    const int WIDTH = 2;

  public:
    Wiggle() {
    }

    virtual void setup() {
    }

    virtual void update(CRGB ledbuffer[]) {
      Pattern::update(ledbuffer);
      int baseheight = 16 * (LEDS_PER_ROW)/2 - 255/4;
      int height = baseheight + sin8(framenumber*8)/2;
      int pos = height/16;
      int fractional = height & 0x0F;
      CRGB colour;
      for(int i=0; i<=WIDTH; i++) {
        if(i==0) {
          colour = CHSV( 0, 0, 255 - (fractional * 16));
        } else if(i==WIDTH) {
          colour = CHSV( 0, 0, fractional * 16);
        } else {
          colour = CHSV( 0, 0, 255);
        }
        for(int row=0; row<ROWS; row++)
          ledbuffer[ledid(row, pos + i)] = colour;
      }
    }
};

class Diagonal: public Pattern {

  public:
    Diagonal() {
    }

    virtual void setup() {
    }

    virtual void update(CRGB ledbuffer[]) {
      int c = 255 * 2 / ROWS;
      int v1 =  255 - (framenumber % 64) * 4;
      for(int l = 0; l<LEDS_PER_ROW; l++) {
        int v2 = l*ROWS/2;
        for(int row = 0; row<ROWS; row++) {
          int v3 = row * c + v2 + v1;
          if(v3>256) v3 = v3 % 256;
          ledbuffer[ledid(row, l)]=CHSV(v3, 255, 64);
        }
      }
    }
};

class Fireworks: public Pattern {

  static const int NO_FIREWORKS = 6;
  const int SHOOT = 40;
  const int EXPLODE = 3;
  const int FALL = 60;
  long startframe[NO_FIREWORKS];
  int row[NO_FIREWORKS];
  CHSV shootcolour[NO_FIREWORKS];
  CHSV burstcolour[NO_FIREWORKS];

  public:
    Fireworks() {
    }

    virtual void randomise(int i) {
      startframe[i] = framenumber + random(30*5);
      row[i] = random(ROWS);
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
        if(framenumber<startframe[i]) continue;
        int frame = framenumber - startframe[i];
        if(frame < SHOOT) {
          unsigned int height = 256*(SHOOT-frame)/SHOOT;
          height *= height;
          height = (65536 - height) / 256 * LEDS_PER_ROW / 256;
          CHSV c = shootcolour[i];
          ledbuffer[ledidC(row[i], height)]=c;
          c.value = 128;
          ledbuffer[ledidC(row[i], height-1)]=c;
          c.value = 64;
          ledbuffer[ledidC(row[i], height-2)]=c;
          if(height==LEDS_PER_ROW-1) startframe[i] = framenumber - SHOOT - 1;
        } else if(frame < SHOOT + EXPLODE) {
          frame-=SHOOT;
          int circlesize = (frame+1);
          for(int x = -circlesize; x<circlesize; x++) {
            for(int y = -circlesize; y<=0; y++) {
              int o_x = row[i];
              int o_y = LEDS_PER_ROW - 1;
              if((x*x + y*y)>circlesize*circlesize) continue;
              ledbuffer[ledidC(o_x + x, o_y + y)]=CRGB::White;
            }
          }
        } else if(frame < SHOOT + EXPLODE + 8) {
          frame-=SHOOT;
          int circlesize = (frame+1);
          if(circlesize>6) circlesize=4;
          int height = LEDS_PER_ROW - (LEDS_PER_ROW-1) * frame / FALL;
          for(int x = -circlesize; x<circlesize; x++) {
            for(int y = -circlesize; y<circlesize; y++) {
              CHSV c1 = burstcolour[i];
              c1.value = 256>>(circlesize/2);
              c1.value = random8(c1.value*2);
              int o_x = row[i];
              int o_y = height;
              if((x*x + y*y)>(frame+1)*(frame+1)) continue;
              ledbuffer[ledidC(o_x + x, o_y + y)]=c1;
            }
          }
        } else if(frame < SHOOT + EXPLODE + FALL) {
          frame-=SHOOT;
          int circlesize = (frame+1);
          if(circlesize>6) circlesize=4;
          int height = LEDS_PER_ROW - (LEDS_PER_ROW-1) * frame / FALL;
          for(int x = -circlesize; x<circlesize; x++) {
            for(int y = -circlesize; y<circlesize; y++) {
              CHSV c1 = burstcolour[i];
              c1.value = 64;
              c1.value = random8(c1.value*2);
              if(random8()>240) c1 = CHSV(0, 0, 255);
              c1.value = c1.value * height / LEDS_PER_ROW ;
              int o_x = row[i];
              int o_y = height;
              if((x*x + y*y)>circlesize*circlesize) continue;
              ledbuffer[ledidC(o_x + x, o_y + y)]=c1;
            }
          }
        } else {
          randomise(i);
        }
      }
    }
};

