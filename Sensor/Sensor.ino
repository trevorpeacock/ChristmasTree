#include <SoftwareSerial.h>

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


class WaitFor {
  unsigned long start;
  int time_to_wait;

  public: WaitFor(int w) {
    start=millis();
    time_to_wait=w;
  }

  bool wait() {
    //Serial.println(String(millis()) + " " + String(start) + " " + String(time_to_wait));
    return millis()<(start+time_to_wait);
  }
};

/*
 * 4 modes
 * quiet - timid
 * soft glow
 * standard patterns
 * agressive - sound reactive
 * 
 * triggered as follows
 * quiet - quiet sound for an extended time, goes when volume increase, or immediately on a loud sound
 * soft, 
 * 
 * need histerisis between levels
 */

class SoundReactor {

  int level;
  const float Kp=0.05;
  
  public: SoundReactor() {
    level=500;
  }

  void update(int target) {
    level = level + Kp * (target-level);
  }

  int getLevel() {
    return level;
  }
};

class SoundReactor2 {

  float audiolevel;
  const float Kp_p=0.005;
  const float Kp_n=0.001;
  const int levels = 3;
  const int thresholds[3] = {45, 80, 160};
  const int threshold_negative_hysteresis[3] = {5, 10, 20};
  const int threshold_positive_hysteresis[3] = {0, 10, 20};
  int currentlevel;
  long currentlevel_entry;
  const int level_minimum_duration_positive[4] = {0, 10, 10, 10};
  const int level_minimum_duration_negative[4] = {0, 30, 30, 10};
  
  public: SoundReactor2() {
    audiolevel = 40;
    currentlevel = 1;
    currentlevel_entry = millis();
  }

  void update(float target) {
    int error = target - audiolevel;
    if(error>0) {
      audiolevel += pow(Kp_p, 1.0-5.0/max(2.5, audiolevel - 30)) * error;
    } else {
      audiolevel += Kp_n * error;
    }
        
    //Serial.println(String(audiolevel) + " " + String(error) + " " + String(1.0-5.0/(audiolevel - 30)) + " " + String(pow(Kp_p, 1.0-5.0/(audiolevel - 30))));
    
  }

  unsigned int getAudioLevel() {
    return audiolevel;
  }

  unsigned int getLevel() {
    if(currentlevel>0) {
      //mySerial.println(String(audiolevel) + "<" + String(thresholds[currentlevel-1]-threshold_negative_hysteresis[currentlevel-1]) + " " + String(millis() - currentlevel_entry) + " " + String(level_minimum_duration[currentlevel]*1000));
      Serial.println(String(audiolevel) + "<" + String(thresholds[currentlevel-1]-threshold_negative_hysteresis[currentlevel-1]) + " " + String((millis() - currentlevel_entry)/1000) + " " + String(level_minimum_duration_negative[currentlevel]));
    }
    if(currentlevel<levels) {
      //mySerial.println(String(audiolevel) + ">" + String(thresholds[currentlevel]+threshold_positive_hysteresis[currentlevel]) + " " + String(millis() - currentlevel_entry) + " " + String(level_minimum_duration[currentlevel]*1000));
      Serial.println(String(audiolevel) + ">" + String(thresholds[currentlevel]+threshold_positive_hysteresis[currentlevel]) + " " + String((millis() - currentlevel_entry)/1000) + " " + String(level_minimum_duration_positive[currentlevel]));
    }
    if(currentlevel>0 &
        audiolevel<thresholds[currentlevel-1]-threshold_negative_hysteresis[currentlevel-1] &
        millis() - currentlevel_entry > level_minimum_duration_negative[currentlevel]*1000) {
      currentlevel += -1;
      currentlevel_entry = millis();
    } else if(currentlevel<levels &
        audiolevel>thresholds[currentlevel]+threshold_positive_hysteresis[currentlevel] &
        millis() - currentlevel_entry > level_minimum_duration_positive[currentlevel]*1000) {
      currentlevel += 1;
      currentlevel_entry = millis();
    }
    return currentlevel;
  }
};

SoundReactor2 soundlevel = SoundReactor2();
MinMax minmax = MinMax();

MinMax mmamplitude = MinMax();

SoftwareSerial mySerial(0, 13);

MinMax mm;

void setup() {
  Serial.begin(250000);
  mySerial.begin(250000);
  pinMode(12, INPUT_PULLUP);
  mm = MinMax();  
}

bool signalpin = false;

void loop() {
  int signalpinval = digitalRead(12);
  if(signalpinval==HIGH && signalpin!=signalpinval) {
    int a = mm.getRange();
    unsigned int r = mm.getMax()-mm.getMin();
    unsigned int l = (analogRead(1)/16) << 10;
    r |= l;
    mySerial.write(byte(r>>8));
    mySerial.write(byte(r&255));
    mm = MinMax();
  } else {
    WaitFor t = WaitFor(1);
    while(t.wait()) {
      mm.update(analogRead(0));
    }
  }
  signalpin = signalpinval;
  return;
  //mmamplitude.update(mm.getRange());
  //mySerial.write((byte)255);
  //mySerial.write((byte)255);
  //mySerial.write(a>>8);
  //mySerial.write(a&255);
  //Serial.write((byte)255);
  //Serial.write((byte)255);
  //Serial.write(a>>8);
  //Serial.write(a&255);
  //analogWrite(11, mm.getRange()/4);
  //a = analogRead(1);
  //a = analogRead(2);
  //a = mm.getRange();
  //r=a;
  //**soundlevel.update(a);
  //**int r=soundlevel.getLevel();
  //digitalWrite(2, r>=0);
  //digitalWrite(3, r>=1);
  //digitalWrite(4, r>=2);
  //digitalWrite(5, r>=3);
  //digitalWrite(10, a>soundlevel.getAudioLevel()*2);
  //analogWrite(9, map(constrain(analogRead(2), 100, 600), 50, 600, 0, 255));
  /*int minval=40;
  int diff = 40;
  digitalWrite(2, r>minval+diff*0);
  digitalWrite(3, r>minval+diff*1);
  digitalWrite(4, r>minval+diff*2);
  digitalWrite(5, r>minval+diff*3);
  digitalWrite(6, r>minval+diff*4);
  digitalWrite(7, r>minval+diff*5);
  digitalWrite(8, r>minval+diff*6);
  digitalWrite(9, r>minval+diff*7);
  digitalWrite(10, a>r*2);*/
  /*Serial.println(String(mmamplitude.getMin()) + " " + String(mmamplitude.getMax()) + " " + String(mm.getRange()));
  */
}

