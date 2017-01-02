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



