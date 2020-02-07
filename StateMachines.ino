#include "Animation.h"

void downbStateMachine() { //State machine to do a small flash and then hold a color every time b is pressed. All other button press state machines follow the exact same logic.
  switch(state_b) //Switch statement acts as a state machine
  {
  case 0: //TURN ON
      for(int i=0;i<5;i++) //Set the LEDs color to bFlashColor
      {   
        leds[i] = CRGB(39,226,241);
      }
      FastLED.show();
      time0b = millis(); //start the timer on the flash
      state_b = 1; //go to the next state
    break;
    case 1: //STAY ON
      //Wait for time to elapse, then proceed to HOLD COLOR
      time1b = millis();
      if (time1b - time0b > buttonDelay) {state_b = 2;}  //if enough time has elapsed (flash is done), turn the hold color on
    break;
    case 2: //HOLD COLOR: Turn LEDs to bHoldColor
      for(int i=0;i<5;i++)
      {   
        leds[i] = CRGB(39,226,241);
      }
      FastLED.show();
      state_b = 3; //next state
    break;
    case 3: //blue
      if (currentPress[B]==0) {state_b = 0;} //if B goes low again, reset to the beginning.
      animations[NEUTRAL_B].triggered = false;
    break;    
  }
}

void bStateMachine() { //State machine to do a small flash and then hold a color every time b is pressed. All other button press state machines follow the exact same logic.
  switch(state_b) //Switch statement acts as a state machine
  {
  case 0: //TURN ON
      for(int i=0;i<5;i++) //Set the LEDs color to bFlashColor
      {   
        leds[i] = CRGB(255,255,255);
      }
      FastLED.show();
      time0b = millis(); //start the timer on the flash
      state_b = 1; //go to the next state
    break;
    case 1: //STAY ON
      //Wait for time to elapse, then proceed to HOLD COLOR
      time1b = millis();
      if (time1b - time0b > buttonDelay) {state_b = 2;}  //if enough time has elapsed (flash is done), turn the hold color on
    break;
    case 2: //HOLD COLOR: Turn LEDs to bHoldColor
      for(int i=0;i<5;i++)
      {   
        leds[i] = CRGB(255,255,255);
      }
      FastLED.show();
      state_b = 3; //next state
    break;
    case 3: //blue
      if (currentPress[B]==0) {state_b = 0;} //if B goes low again, reset to the beginning.
      animations[NEUTRAL_B].triggered = false;
    break;    
  }
}

void aStateMachine() { //State machine to do a small flash and then hold a color every time a is pressed.
switch(state_a)
  {
  case 0: //TURN white red 
      for(int i=0;i<5;i++) //Set the LEDs color to aFlashColor
      {   
        leds[i] = CRGB(aFlashColor);
      }
      FastLED.show();
      time0b = millis(); //update the timer for the flash part
      state_a = 1; //go to the next state
    break;
    case 1: //ON
      //Wait for time to elapse, then proceed to HOLD COLOR
      time1a = millis();
      if (time1a - time0a > buttonDelay) {state_a = 2;}  
    break;
    case 2: //HOLD COLOR: Turn LEDs to aHoldColor
      for(int i=0;i<5;i++)
      {   
        leds[i] = CRGB(aHoldColor);
      }
      FastLED.show();
      state_a = 3; //next state
    break;
    case 3: //White  
      if (currentPress[A]==0) {state_a = 0;} //if a goes low again, reset to the beginning.
      animations[NEUTRAL_A].triggered = false;
    break;    
  }
}

void xStateMachine() { //State machine to do a small flash and then hold a color every time x is pressed.
switch(state_x)
  {
    case 0: //TURN Flash ON
      for(int i=0;i<5;i++)
      {   
        leds[i] = CRGB(xFlashColor);
      }
      FastLED.show();
      time0x = millis();
      state_x = 1;
    break;
    case 1: //ON
      //Wait for time to elapse, then proceed to HOLD COLOR
      time1x = millis();
      if (time1x - time0x > buttonDelay) {state_x = 2;}  
    break;
    case 2: //HOLD COLOR
      for(int i=0;i<5;i++)
      {   
        leds[i] = CRGB(xHoldColor);
      }
      FastLED.show();
      state_x = 3; 
    break;
    case 3:
      if (currentPress[X]==0) {state_x = 0;}
      animations[ACTION_X].triggered = false; 
    break;    
  }
}

void yStateMachine() { //State machine to do a small flash and then hold a color every time y is pressed.
  switch(state_y) //Switch statement acts as a state machine
  {
     case 0: //TURN ON
      for(int i=0;i<5;i++) //Set the LEDs color to bFlashColor
      {   
        leds[i] = CRGB(yFlashColor);
      }
      FastLED.show();
      time0y = millis(); //start the timer on the flash
      state_y = 1; //go to the next state
    break;
    case 1: //STAY ON
      //Wait for time to elapse, then proceed to HOLD COLOR
      time1y = millis();
      if (time1y - time0y > buttonDelay) {state_y = 2;}  //if enough time has elapsed (flash is done), turn the hold color on
    break;
    case 2: //HOLD COLOR: Turn LEDs to bHoldColor
      for(int i=0;i<5;i++)
      {   
        leds[i] = CRGB(yHoldColor);
      }
      FastLED.show();
      state_y = 3; //next state
    break;
    case 3: //blue
      if (currentPress[Y]==0) {state_y = 0;} //if y goes low again, reset to the beginning.
      animations[ACTION_Y].triggered = false;
    break;    
  }
}

void lStateMachine() { //State machine to do a small flash and then hold a color every time l is pressed.
switch(state_l)
  {
    case 0: //TURN ON
      for(int i=0;i<5;i++)
      {   
        leds[i] = CRGB(lFlashColor);
      }
      FastLED.show();
      time0l = millis();
      state_l = 1;
    break;
    case 1: //ON
      //Wait for time to elapse, then proceed to HOLD COLOR
      time1l = millis();
      if (time1l - time0l > buttonDelay) {state_l = 2;}  
    break;
    case 2: //HOLD COLOR
      for(int i=0;i<5;i++)
      {   
        leds[i] = CRGB(lHoldColor);
      }
      FastLED.show();
      state_l = 3; 
    break;
    case 3: //White  
      if (currentPress[L]==0) {state_l = 0;}
      animations[ACTION_L].triggered = false;
    break;    
  }
}

void rStateMachine() { //State machine to do a small flash and then hold a color every time r is pressed.
switch(state_r)
  {
    case 0: //TURN ON
      for(int i=0;i<5;i++)
      {   
        leds[i] = CRGB(rFlashColor);
      }
      FastLED.show();
      time0r = millis();
      state_r = 1;
    break;
    case 1: //ON
      //Wait for time to elapse, then proceed to HOLD COLOR
      time1r = millis();
      if (time1r - time0r > buttonDelay) {state_r = 2;}  
    break;
    case 2: //HOLD COLOR
      for(int i=0;i<5;i++)
      {   
        leds[i] = CRGB(rHoldColor);
      }
      FastLED.show();
      state_r = 3; 
    break;
    case 3: //White  
      if (currentPress[R]==0) {state_r = 0;}
      animations[ACTION_R].triggered = false;
    break;    
  }
}

void zStateMachine() { //State machine to do a small flash and then hold a color every time z is pressed.
switch(state_z)
  {
    case 0: //TURN ON
      for(int i=0;i<5;i++)
      {   
        leds[i] = CRGB(zFlashColor);
      }
      FastLED.show();
      time0z = millis();
      state_z = 1;
    break;
    case 1: //ON
      //Wait for time to elapse, then proceed to HOLD COLOR
      time1z = millis();
      if (time1z - time0z > buttonDelay) {state_z = 2;}  
    break;
    case 2: //HOLD COLOR
      for(int i=0;i<5;i++)
      {   
        leds[i] = CRGB(zHoldColor);
      }
      FastLED.show();
      state_z = 3; 
    break;
    case 3: //White  
      if (currentPress[Z]==0) {state_z = 0;}
      animations[ACTION_Z].triggered = false;
    break;    
  }
}

void sStateMachine() { //State machine to do a small flash and then hold a color every time z is pressed.
switch(state_s)
  {
    case 0: //TURN ON
      for(int i=0;i<5;i++)
      {   
        leds[i] = CRGB(sFlashColor);
      }
      FastLED.show();
      time0s = millis();
      state_s = 1;
    break;
    case 1: //ON
      //Wait for time to elapse, then proceed to HOLD COLOR
      time1s = millis();
      if (time1s - time0s > buttonDelay) {state_s = 2;}
    break;
    case 2: //HOLD COLOR
      for(int i=0;i<5;i++)
      {   
        leds[i] = CRGB(sHoldColor);
      }
      FastLED.show();
      state_s = 3; 
    break;
    case 3: //White  
      if (currentPress[START]==0) {state_s = 0;}
      animations[ACTION_START].triggered = false;
    break;    
  }
}

void xysStateMachine() { //State machine to reset the origin for the leds when x+y+start is held for a while
switch(state_xys)
  {
  case 0: //WAIT
  //do nothing unless the condition is met
  if (currentPress[X]&&currentPress[Y]&&currentPress[START]) //if x and y and start are being pressed, go to the next step
  {
    state_xys=1;
  }
    break;
  case 1: //Begin timer
    time0xys = millis(); //timer0 started
    state_xys = 2;
    break;
  case 2: //Running timer
    time1xys = millis(); //timer1 ticking
    if (currentPress[X]==0||currentPress[Y]==0||currentPress[START]==0) //if you stopped pressing x+y+start
    {
      state_xys = 3; //it was a false alarm
    }
    if (time1xys-time0xys>resetDelay) //if you exceeded the amount of time we set for a sufficiently long xystart hold
    {
      state_xys = 4; //confirmed
    }
    break;
  case 3: //False Alarm
    time1xys=time0xys; //reset the timer
    state_xys = 0; //reset to the beginning
    break;
  case 4: //Confirmed trying to reset
    started=false; //act as if the controller was just plugged in, redo the origin poll
    originPollNumber=0;
    mode=1; //also go to the idle animation to confirm the reset worked
    state_xys = 0; //reset the state machine
    break;    
  }
}
