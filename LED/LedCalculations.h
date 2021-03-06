/*
 * Utility functions for calculating led positions
 */

//returns the ID of the led on the given row, at the give height.
//Will "wrap" out of range leds between rows and heights
int ledid(int row, int height) {
  while(row<0) row+=ROWS;
  if(row>=int(ROWS))
    row = row % int(ROWS);
  int r = LEDS_PER_ROW * row;
  while(height<0) height+=LEDS_PER_ROW;
  if(height >=int(LEDS_PER_ROW))
    height = height % int(LEDS_PER_ROW);
  if(row%2==0) {
    r+=height;
  } else {
    r+=LEDS_PER_ROW - 1 - height;
  }
  return r;
}


//returns the ID of the led on the given row, at the give height.
//Will "wrap" out of rows, but constrains height to be within range
int ledidC(int row, int height) {
  while(row<0) row+=ROWS;
  if(row>=int(ROWS))
    row = row % int(ROWS);
  int r = LEDS_PER_ROW * row;
  height = constrain(height, 0, LEDS_PER_ROW-1);
  if(row%2==0) {
    r+=height;
  } else {
    r+=LEDS_PER_ROW - 1 - height;
  }
  return r;
}


int starid(int section, int point, int pos) {
  if(section==0)
    return NUM_LEDS-2+pos;
  if(section==1)
    return NUM_LEDS_TREE+STAR_POINTS*STAR_POINT_LEDS*2+point*STAR_CORE_LEDS+pos;
  if(section==2)
    return NUM_LEDS_TREE+point*STAR_POINT_LEDS*2+pos;
}

void fillcenter(CRGB ledbuffer[], CRGB color) {
  ledbuffer[starid(0,0,0)]=color;
  ledbuffer[starid(0,0,0)+1]=color;
}

void fillstar(CRGB ledbuffer[], CRGB color, int rad) {
  if(rad==0){
    ledbuffer[starid(0,0,0)]=color;
    ledbuffer[starid(0,0,1)]=color;
  } else if(rad==1) {
    for(int i = 0; i < STAR_POINTS; i++)
      ledbuffer[starid(1,i,1)]=color;
  } else if(rad==2) {
    for(int i = 0; i < STAR_POINTS; i++) {
      ledbuffer[starid(1,i,0)]=color;
      ledbuffer[starid(1,i,2)]=color;
    }
  } else {
    for(int i = 0; i < STAR_POINTS; i++) {
      ledbuffer[starid(2,i,0+(rad-3))]=color;
      ledbuffer[starid(2,i,9-(rad-3))]=color;
    }
  }
}
