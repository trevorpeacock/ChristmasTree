#define AUDIODEBUG true

/*
 * This model produces an average audio level, weighted such that it goes up quicker than down, and is biased against reaching a minimum value
 * It produces a "level", eg representing "quiet", "meduim" or "loud" based in parameters
 */
class SoundReactor {

  float audiolevel;
  int lastvolume;
  //coefficient controlling the speed audio levels increase
  const float Kp_p=0.015;
  //coefficient controlling the speed audio levels decrease
  const float Kp_n=0.003;

  int currentlevel;
  int prospective_level;
  long prospective_level_time;

  //the number of levels, 0..n
  const int maxlevel = 2;
  //the audio level thresholds seperating levels
  const int thresholds[2] = {/*0<>1*/ 43, /*1<>2*/ 70};
  //histerisis for moving from lower to higher levels
  const int threshold_negative_hysteresis[2] = {/*1->0*/ 3, /*2->1*/ 0};
  //histerisis for moving from higher to lower levels
  const int threshold_positive_hysteresis[2] = {/*0->1*/ 0, /*1->2*/ 5};
  //how long (seconds) must a new level be sustationed to transition
  const int level_minimum_duration_negative[2] = {/*1->0*/ 30, /*2->1*/ 30};
  const int level_minimum_duration_positive[2] = {/*0->1*/ 0, /*1->2*/10};
  
  public: SoundReactor() {
    audiolevel = 70;
    lastvolume = 0;
    currentlevel = 1;
  }

  void update(float target) {
    lastvolume = target;
    int error = target - audiolevel;
    if(error>0) {
      //as audiolevel gets closer to 30, Kp_p gets closer to 1
      //this causes the model to be very sensitive to sounds at low volumes
      audiolevel += pow(Kp_p, 1.0-5.0/max(5, audiolevel - 30)) * error;
    } else {
      audiolevel += Kp_n * error;
    }
        
    updateLevel();
  }

  int getLastVolume() {
    return lastvolume;
  }

  unsigned int getAudioLevel() {
    return audiolevel;
  }

  void updateLevel() {
    //thresholds for audio changes
    int negative_threshold = thresholds[currentlevel-1]-threshold_negative_hysteresis[currentlevel-1];
    int positive_threshold = thresholds[currentlevel]+threshold_positive_hysteresis[currentlevel];
    //test to see if we should considder going up or down a level
    if(currentlevel>0 & audiolevel<negative_threshold) {
      if(prospective_level!=currentlevel-1) {
        //start a timer. If audio level remains we will switch after timeout expires
        prospective_level=currentlevel-1;
        prospective_level_time = millis();
      }
    } else if(currentlevel<maxlevel & audiolevel>positive_threshold) {
      if(prospective_level!=currentlevel+1) {
        prospective_level=currentlevel+1;
        prospective_level_time = millis();
      }
    } else {
      prospective_level = currentlevel;
    }

    //Debugging
    if(AUDIODEBUG && framenumber%30==0) {
      if(prospective_level<currentlevel) {
        Serial.println(String(audiolevel) + "<" + String(negative_threshold) + 
          " " + String(currentlevel) + "->" + String(prospective_level) + 
          " " + String((millis() - prospective_level_time)/1000) + 
          "/" + String(level_minimum_duration_negative[currentlevel]));
      } else if(prospective_level>currentlevel) {
        Serial.println(String(positive_threshold) + "<" + String(audiolevel) +
          " " + String(currentlevel) + "->" + String(prospective_level) + 
          " " + String((millis() - prospective_level_time)/1000) + 
          "/" + String(level_minimum_duration_positive[currentlevel]));
      } else {
        if(currentlevel==0) {
          Serial.println(String(audiolevel) + "<" + String(positive_threshold) + 
            " " + String(currentlevel));
        } else if(currentlevel==maxlevel) {
          Serial.println(String(negative_threshold) + "<" + String(audiolevel) + 
            " " + String(currentlevel));
        } else {
          Serial.println(String(negative_threshold) + "<" + String(audiolevel) + "<" + String(positive_threshold) + 
            " " + String(currentlevel));
        }
      }
    }
    
    //If we have over the threshold for the specifed time, change levels.
    if(prospective_level<currentlevel & 
        millis() - prospective_level_time > level_minimum_duration_negative[currentlevel-1]*1000) {
      currentlevel = prospective_level;
    }
    if(prospective_level>currentlevel & 
        millis() - prospective_level_time > level_minimum_duration_positive[currentlevel]*1000) {
      currentlevel = prospective_level;
    }
  }
  unsigned int getLevel() {
    return currentlevel;
  }
};

SoundReactor soundlevel = SoundReactor();

