
/*
 * Keeps track of the min and max value provided to update(int).
 */
class MinMax {
  int min;
  int max;
  int samples;

  public: MinMax() {
    min=32767;
    max=-32768;
    samples=0;
  }

  void update(int val) {
    if(val<min) min=val;
    if(val>max) max=val;
    samples++;
  }

  int getMin() {
    return min;
  }

  int getMax() {
    return max;
  }

  int getSamples() {
    return samples;
  }

  int getRange() {
    return max-min;
  }
};

/*
 * WaitFor(int) starts a timer. wait() returns false until the specified number of milliseconds expires.
 */
class WaitFor {
  unsigned long start;
  int time_to_wait;

  public: WaitFor(int w) {
    start=millis();
    time_to_wait=w;
  }

  bool wait() {
    return millis()<(start+time_to_wait);
  }
};

