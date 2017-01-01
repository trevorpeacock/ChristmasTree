#define AUDIODEBUG true

class SoundReactor {

  float audiolevel;
  int lastvolume;
  const float Kp_p=0.015;
  const float Kp_n=0.003;
  const int maxlevel = 2;
  const int thresholds[3] = {43, 70, 160};
  const int threshold_negative_hysteresis[3] = {3, 0, 20};
  const int threshold_positive_hysteresis[3] = {0, 5, 20};
  int currentlevel;
  //long currentlevel_entry;
  const int level_minimum_duration_positive[4] = {0, 10, 10, 10};
  const int level_minimum_duration_negative[4] = {0, 30, 30, 10};
  int prospective_level;
  long prospective_level_time;
  
  public: SoundReactor() {
    audiolevel = 70;
    lastvolume = 0;
    currentlevel = 1;
    //currentlevel_entry = millis();
  }

  void update(float target) {
    lastvolume = target;
    int error = target - audiolevel;
    if(error>0) {
      //audiolevel += Kp_p * error;
      audiolevel += pow(Kp_p, 1.0-5.0/max(2.5, audiolevel - 30)) * error;
    } else {
      audiolevel += Kp_n * error;
    }
        
    //Serial.println(String(audiolevel) + " " + String(error) + " " + String(1.0-5.0/(audiolevel - 30)) + " " + String(pow(Kp_p, 1.0-5.0/(audiolevel - 30))));
    updateLevel();
  }

  int getLastVolume() {
    return lastvolume;
  }

  unsigned int getAudioLevel() {
    return audiolevel;
  }

  void updateLevel() {
    int negative_threshold = thresholds[currentlevel-1]-threshold_negative_hysteresis[currentlevel-1];
    int positive_threshold = thresholds[currentlevel]+threshold_positive_hysteresis[currentlevel];
    if(currentlevel>0 & audiolevel<negative_threshold) {
      if(prospective_level!=currentlevel-1) {
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
    if(prospective_level<currentlevel & 
        millis() - prospective_level_time > level_minimum_duration_negative[currentlevel]*1000) {
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

