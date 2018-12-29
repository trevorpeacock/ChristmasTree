/*
 * Global variables and constants, and utility classes/functions.
 */
//Enables soundlevel and frame debug patterns
#define DEBUG false

#define SOUND_SENSOR true
#define LIGHT_SENSOR true



#define ROWS             16 //the number of strips.
#define LEDS_PER_ROW     30 // number of leds on each strip
#define STAR_POINTS       5 // 
#define STAR_POINT_LEDS   5
#define STAR_CORE_LEDS    3
#define STAR_CENTER_LEDS  2

#define NUM_LEDS_TREE    (ROWS*LEDS_PER_ROW)
#define NUM_LEDS_STAR    (STAR_POINTS*STAR_POINT_LEDS*2+STAR_POINTS*STAR_CORE_LEDS+STAR_CENTER_LEDS)

#define NUM_LEDS         (NUM_LEDS_TREE+NUM_LEDS_STAR)

//The main framebuffer. Managed by PatternManager
CRGB leds[NUM_LEDS];

//global framenumber
long framenumber = 0;

/*
 * WaitFor(int) starts a timer. wait() returns false until the specified number of milliseconds expires.
 */
class WaitFor {
    unsigned long start;
    int time_to_wait;

  public:
    WaitFor(int w) {
      start = millis();
      time_to_wait = w;
    }

    bool wait() {
      return millis() < (start + time_to_wait);
    }

    long timeRemaining() {
      return start + time_to_wait - millis();
    }
};

/*
 * SlidingWindow(int) maintains a sliding window of defined size. push(int) adds a value. getVal(0..n) gets a value from the window.
 */
class SlidingWindow {

  int *window;
  int windowsize;
  int nextposition;
  
  public:
    SlidingWindow(int s) {
      windowsize = s;
      window = new int[windowsize];
      nextposition = 0;
    }
    ~SlidingWindow() {
      delete [] window;
    }

    void push(int i) {
      window[nextposition]=i;
      nextposition ++;
      if(nextposition==windowsize) nextposition=0;
    }

    int getVal(int pos) {
      pos+=nextposition;
      if(pos>=windowsize) pos -= windowsize;
      if(pos<0 || pos>=windowsize) pos = pos % windowsize;
      return window[pos];
    }

};

//Copy of the arduino map function, using int rather than long.
int map16(int x, int in_min, int in_max, int out_min, int out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
