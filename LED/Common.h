#define ROWS 16
#define LEDS_PER_ROW 30
#define NUM_LEDS ROWS*LEDS_PER_ROW

CRGB leds[NUM_LEDS];

long framenumber = 0;

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

int map16(int x, int in_min, int in_max, int out_min, int out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
