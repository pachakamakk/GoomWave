/*
Copyright 2019 Jonah Eskin

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

/*
This is the GoomWave project, an attempt at a hackable responsive LED GameCube controller framework, based on 2 different projects. NintendoSpy and ShineWave. 
This project aims to be an improved version of ShineWave with more versatility and understandability.
Currently this project is designed for Arduino Nano and DotStar LEDs, although other LED types and microcontrollers may be added in the future

Resources:
GameCube controller data protocol: http://www.int03.co.uk/crema/hardware/gamecube/gc-control.html
NintendoSpy project: https://github.com/jaburns/NintendoSpy
ShineWave project: https://github.com/Serisium/Shinewave
This helpful video on Switch Case State Machines: https://youtu.be/v8KXa5uRavg?list=PLH_Belnbfmpi3nsjzXG1jGfH4rfW11P48
 */
#include <FastLED.h>    //Publicly available library for sending data to LEDs
#include "Animation.h"

//Declarations for reading the GameCube Data                        
#define dataStringLength              89                                                                      //25 for GC data + 64 for controller data
#define readPin(controllerDataPin)    ((PIND&(1<<(controllerDataPin)))>>controllerDataPin)                    //returns a 1 or 0 based off if pin is high or low
bool rawData[dataStringLength];                                                                               //rawData is an array of booleans which represent a 1 or 0 in the serial string
const uint8_t controllerDataPin = 7;                                                                          //Pin going to the arduino.
#define MICROSECOND_NOPS "nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n"   //Assembly for 1 microsecond using clock pulses on a 16MHz processor. Needs to be changed for other clock speed processors.

//LED declarations
#define NUM_LEDS 5    // How many leds in your strip?
//All color declarations are made for Dotstars. Neopixels would need to be changed to GBR format.
#define Black     0, 0, 0     //Colors are RedBlueGreen (RBG) format for DotStars.
#define Red       255, 0, 0
#define Blue      0, 255, 0
#define Green     0, 0, 255
#define White     255, 255, 255
#define Purple    255, 255, 0

typedef struct s_recordedInput {
  bool isAngle;
  uint8_t angle;
} t_recordedInput;

CRGB leds[NUM_LEDS]; // Define the array of leds
uint8_t gHue = 0; //number that dictates how far along the idle animation we are

//origin poll declarations for whether to calibrate the controller or not
bool started = false;
uint8_t originPollNumber = 0;

enum e_button {
  A       = 0,
  B       = 1,
  X       = 2,
  Y       = 3,
  DDOWN   = 4,
  HARDR   = 5,
  HARDL   = 6,
  HARD2   = 7,
  Z       = 8,
  START   = 9,
  RUMBLE  = 10
};

enum e_angle {
  XA  = 0,
  YA  = 1,
  XC  = 2,
  YC  = 3,
  LANGLE   = 4,
  RANGLE   = 5
};

typedef enum e_direction {
  UP      = 0,
  DOWN    = 1,
  SIDE    = 2,
  NEUTRAL = 3
};

//Variable containing the direction of the attack 
e_direction attackDirection; 

//Variable declarations for buttons and mode states (b = true/1 means b is pressed, b = false/0 means b is not pressed, etc.)
bool currentPress[11];

//declarations for the analog parts of the data
uint8_t xA=0;   //Analog stick x axis
uint8_t yA=0;   //Analog stick y axis
uint8_t xC=0;   //C stick x axis
uint8_t yC=0;   //C stick y axis
uint8_t L=0;    //L analog
uint8_t R=0;    //R analog
signed int xAorigin = 0; //Origin offsets to be calculated for Melee
signed int yAorigin = 0; 
signed int xCorigin = 0; 
signed int yCorigin = 0; 
signed int Lorigin = 0; 
signed int Rorigin = 0;

//Post-calibrated variables for analog sections
float xAresult = 0;
float yAresult = 0;
float xCresult = 0;
float yCresult = 0;
float Lresult = 0;
float Rresult = 0;

//after more math to convert them to very close to melee values
float hA = 0; //hA = hypotenuse of analog stick
float hC = 0;
float maxhAnew = 0; //Hypotenuse max'd to 1.0
float maxhCnew = 0;

float fixedXA = 0;
float fixedYA = 0;

float currentAnalog[6];

//Variables for the previous read of button presses
bool oldPress[11];

//Mode variables
byte modeCount = 0; //To switch between modes
byte mode = 0; //Which mode we're in

//State machine variable declarations
const unsigned long buttonDelay = 10; //How long the button will flash bright before holding its color

//b button press variables
int state_b = 0; //for the state machine
unsigned long time1b = 0; //Timer
unsigned long time0b = 0; //Value at start of timer
#define bFlashColor 240,255,240 //Change the numbers (0-255) in this line for different flash colors
#define bHoldColor  100,255,100 //Change the numbers (0-255) in this line for different hold colors

//a button press variables
int state_a = 0; //for the state machine
unsigned long time1a = 0; //Timer
unsigned long time0a = 0; //Value at start of timer
#define aFlashColor 240,240,180 //Change the numbers (0-255) in this line for different flash colors
#define aHoldColor  240,240,10 //Change the numbers (0-255) in this line for different hold colors

//x button press variables
int state_x = 0; //for the state machine
unsigned long time1x = 0; //Timer
unsigned long time0x = 0; //Value at start of timer
#define xFlashColor 180,180,250 //Change the numbers (0-255) in this line for different flash colors
#define xHoldColor  50,50,220 //Change the numbers (0-255) in this line for different hold colors

//y button press variables
int state_y = 0; //for the state machine
unsigned long time1y = 0; //Timer
unsigned long time0y = 0; //Value at start of timer
#define yFlashColor 240,255,240 //Change the numbers (0-255) in this line for different flash colors
#define yHoldColor  100,255,100 //Change the numbers (0-255) in this line for different hold colors

//l button press variables
int state_l = 0; //for the state machine
unsigned long time1l = 0; //Timer
unsigned long time0l = 0; //Value at start of timer
#define lFlashColor 80,220,255 //Change the numbers (0-255) in this line for different flash colors
#define lHoldColor  10,180,255 //Change the numbers (0-255) in this line for different hold colors

//r button press variables
int state_r = 0; //for the state machine
unsigned long time1r = 0; //Timer
unsigned long time0r = 0; //Value at start of timer
#define rFlashColor 255,50,255 //Change the numbers (0-255) in this line for different flash colors
#define rHoldColor  255,10,255 //Change the numbers (0-255) in this line for different hold colors

//z button press variables
int state_z = 0; //for the state machine
unsigned long time1z = 0; //Timer
unsigned long time0z = 0; //Value at start of timer
#define zFlashColor 205,205,100 //Change the numbers (0-255) in this line for different flash colors
#define zHoldColor  205,205,50 //Change the numbers (0-255) in this line for different hold colors

//start (s) button press variables
int state_s = 0; //for the state machine
unsigned long time1s = 0; //Timer
unsigned long time0s = 0; //Value at start of timer
#define sFlashColor 255,255,255 //Change the numbers (0-255) in this line for different flash colors
#define sHoldColor  200,200,200 //Change the numbers (0-255) in this line for different hold colors

//xystart reset variables
int state_xys = 0; //for the state machine
unsigned long time1xys = 0; //Timer
unsigned long time0xys = 0; //Value at start of timer
const unsigned long resetDelay = 100; //How long to wait holding x+y+start to recalibrate

//strobe light variables
int stateStrobe=0; //for the state machine
unsigned long time1Strobe = 0; //Timer
unsigned long time0Strobe = 0; //Value at start of timer
const unsigned long delayStrobe = 1; //how long to hold before switching
bool alt=0; //whether you're lighting up inside or outside LEDs

#define DATA_PIN 2 //this line is only needed for NeoPixels. Data line = D2
//Wire the DotStars (APA102s) to the SCK (pin 13) and MOSI (pin 11) on arduino nano for SPI communication

void setup() 
{ 
//  Optional Serial monitor prints for debugging purposes  
  Serial.begin(2000000);
  Serial.println("Initializing LEDs");

  //Uncomment this next line for APA102s (Dotstars). Note: Color values are RBG not GBR like NeoPixels, so colors will be different
  FastLED.addLeds<APA102, RGB>(leds, NUM_LEDS); //what type of LEDs to use

  Serial.println("Loading animations");
  initializeAnimations(); // Loading the different animations for each input (RESPONSIVE + MODE)
  Serial.println("Done");
  

  //Uncomment this next line for WS2812s (NeoPixels). Note: Color values are GBR not RBG like Dotstars, so colors will be different
//  FastLED.addLeds<WS2812, DATA_PIN, RGB>(leds, NUM_LEDS);
}

void readControllerData(int bits)     //Function that gets called for reading the controller+GameCube data
{
  bool *rawDataPtr = rawData;    //makes a pointer to increment through the different memory locations of rawData
  while(bits>0)                  //while loop to go through all 89 bits of data
  {
        while( readPin(controllerDataPin)==0 ) //while the line is pulled low, get stuck
        {
        }
        while( readPin(controllerDataPin)==1 ) //while the line is high, get stuck... this has the effect of reading a falling edge with the 2 while loops
        {
        }
        asm volatile( MICROSECOND_NOPS MICROSECOND_NOPS ); //wait 2 microseconds
    
        *rawDataPtr = readPin(controllerDataPin);           //stores the data in 1 section of rawData
         ++rawDataPtr;                                      //increments the pointer to the next spot in the array
         bits--;                                            //decreases the bits by 1
  }
}

void sendRawData() //Function for sending messages to LEDs after reading the data
{
    interpretData(); //interpret the raw serial data into usable data with similar values to Melee
    xysStateMachine(); //State machine for recalibrating the LEDs with X+Y+start
    EVERY_N_MILLISECONDS( 5 ) { gHue++; } //function to increment every once in a while for the idle animation

    //To go to different modes
    if(!oldPress[DDOWN] && currentPress[DDOWN])  //if it was a 0 and then is a 1 (it was pressed down) then increment to the next mode
    {
      modeCount = modeCount + 1;
      mode = modeCount %5; //5 modes
//      Serial.print("the mode is: ");
//      Serial.println(mode); //Optional Serial Prints
    }
    oldPress[DDOWN] = currentPress[DDOWN];
    //end setup before modes
    
    /////////////////////////////////////////////////modes
      
      if (mode == 0) //Mode 0: Reactive craziness on every input
      {
        lightUpButtons(); //Attach the state machines for lighting up the buttons
//        if (currentPress[X]==0&&currentPress[Y]==0&&currentPress[A]==0&&currentPress[B]==0&&currentPress[L]==0&&currentPress[R]==0&&currentPress[Z]==0&&currentPress[START]==0) //if no button is pressed
//        {
//          if (currentAnalog[XC]>=.2875||currentAnalog[YC]>=.2875) //if the C stick is being pressed
//          {
//            for(int i=0;i<5;i++)
//            {
//              //The order of currentAnalog[XC] and currentAnalog[YC] as well as the *255 can be changed to change the colors
//              leds[i] = CRGB(currentAnalog[YC]*255,currentAnalog[XC]*255,currentAnalog[XC]*255); //light up based on C stick inputs
//            }
//          }
//          else //the c stick is not being pressed
//          {
//            for(int i=0;i<5;i++)
//            { 
//              //The order of currentAnalog[XA] and currentAnalog[YA] as well as the *255 can be changed to change the colors
//              leds[i] = CRGB(currentAnalog[XA]*255,currentAnalog[YA]*255,currentAnalog[XA]*255); //light up based on analog stick inputs
//            }
//          }
//          if(currentAnalog[XC]<.2875&&currentAnalog[YC]<.2875&&currentAnalog[XA]<.2875&&currentAnalog[YA]<.2875) //if none of the sticks are pressed
//          {
//            for(int i=0;i<5;i++)
//            { 
//              //The order of currentAnalog[L] and currentAnalog[R] as well as the *255 can be changed to change the colors
//              leds[i] = CRGB(currentAnalog[L]*255,currentAnalog[R]*255,currentAnalog[R]*255); //light up based on the analog triggers
//            }
//          }
//        }

        FastLED.show();
        
      }
      else if (mode == 1)//mode 1, idle animation
      {
            fill_rainbow(leds, NUM_LEDS, gHue, 10); //FastLED built in function for rainbow animation
            FastLED.show();
      }
      else if (mode == 2) //mode 2, LED notches
      {
        if (currentAnalog[YA]>.9&&currentAnalog[XA]>.3&&currentAnalog[XA]<.35||currentAnalog[XA]>.9&&currentAnalog[YA]>.3&&currentAnalog[YA]<.35) //if the values are within a certain range
        {
          for(int i=0;i<5;i++)
          { 
            //The order of currentAnalog[XA] and currentAnalog[YA] as well as the *255 can be changed to change the colors
            leds[i] = CRGB(currentAnalog[YA]*255,currentAnalog[XA]*255,currentAnalog[XA]*255); //light up based on analog stick inputs
          }
          FastLED.show();
        }
        else
        {
          for(int i=0;i<5;i++)
          { 
            leds[i] = CRGB(Black); //Just show black
          }
          FastLED.show();
        }
      }
      else if (mode == 3) //mode 3, Flash white for rumble
      {
        if (currentPress[RUMBLE]==1)
        {
          for(int i=0;i<5;i++)
          {   
            leds[i] = CRGB(White);
          }
          FastLED.show();
        }
        else
        {
          for(int i=0;i<5;i++)
          {   
            leds[i] = CRGB(Black);
          }
          FastLED.show();
        }
      
      }
      else if (mode == 4) //mode 4, Strobe Light
      {
        strobeLight(); //Strobe light state machine
      }
      else //It should never be here
      {
        Serial.println("error");
      }
    ////////////////////////////////////end modes
}

void loop() 
{ 
    //Fairly self explanitory main loop. It just reads and then writes. Only issue is the sendRawData function can't take up too long or it will not read the data in time for the next polling cycle
    //readControllerData takes at least .36 ms to execute. Super Smash Bros Melee polls every 8ms. So sendRawData can't take more than 7.64ms to execute.
    //If you are attempting to modify sendRawData try to incorporate functions that don't take a long time to execute, such as millis() instead of delay()
    
    noInterrupts(); //interrupts will mess up the timing of the readControllerData function
    readControllerData(dataStringLength);   
    interrupts(); //allow interrupts for things like Serial prints and other such arduino things
    sendRawData(); //Interpret the data and tell the LEDs what to do, don't take over 7.64 ms to do it tho
}
void interpretData() {
    //for calculating the origin offset for calculating Melee analog inputs
    if (originPollNumber < 20) //Use the 20th poll for calculating inputs
    {
      started=false; //We're at the beginning
      originPollNumber++; //now we're 1 step closer to not the beginning
    }
    else
    {
      started=true; //No longer at the beginning
    }

    //setup before modes
    currentPress[A] = rawData[32];
    currentPress[B] = rawData[31];
    currentPress[X] = rawData[30];
    currentPress[Y] = rawData[29];
    currentPress[L] = rawData[34];
    currentPress[R] = rawData[35];
    currentPress[Z] = rawData[36];
    currentPress[START] = rawData[28];
    currentPress[RUMBLE] = rawData[23];
    currentPress[DDOWN] = rawData[38];

    /////////////////////////////////Analog data calculations start
    xA = 0; //Inital x Axis 8 bit value
    yA = 0;
    xC = 0;
    yC = 0;
    L = 0;
    R = 0;
    //write rawData into the unsigned 8 bit number for each analog input
    for(int i=7; i>=0;i--){xA = xA+(rawData[48-i] << i);} //Analog stick x axis
    for(int i=7; i>=0;i--){yA = yA+(rawData[56-i] << i);} //Analog stick y axis
    for(int i=7; i>=0;i--){xC = xC+(rawData[64-i] << i);} //C stick x axis
    for(int i=7; i>=0;i--){yC = yC+(rawData[72-i] << i);} //C stick y axis
    for(int i=7; i>=0;i--){L = L+(rawData[80-i] << i);}   //L analog
    for(int i=7; i>=0;i--){R = R+(rawData[88-i] << i);}   //R analog

    if (started == false) //Only do this if the controller was just plugged in (20th polling cycle)
    {
      xAorigin = xA-128;  //calibrate the origin offsets
      yAorigin = yA-128;
      xCorigin = xC-128; 
      yCorigin = yC-128; 
      Lorigin = L;
      Rorigin = R;
    }
    
    //Convert the raw controller numbers to (close to) Melee numbers
    xAresult=((xA-xAorigin)-128)/80.0; //Factor in the origins that were found at the beginning with the origin poll
    yAresult=((yA-yAorigin)-128)/80.0;
    xCresult=((xC-xCorigin)-128)/80.0;
    yCresult=((yC-yCorigin)-128)/80.0;
    Lresult = (L-Lorigin)/140.0;
    Rresult = (R-Rorigin)/140.0;

    //calculate the exact melee values
    hA = sqrt(yAresult*yAresult+xAresult*xAresult); //hA = hypotenuse of analog stick
    hC = sqrt(yCresult*yCresult+xCresult*xCresult); //hC = hypotenuse of C stick
    maxhAnew=max(1.0,hA); //round the hypotenuse to 1.0 if it's over 1.0
    maxhCnew=max(1.0,hC);
    fixedXA = xAresult/maxhAnew;
    fixedYA = yAresult/maxhAnew;
    attackDirection = NEUTRAL;
    if (fixedXA >= -.6 && fixedXA <= .6)
    {
      if (fixedYA >= .3)
        attackDirection = UP;
      else if (fixedYA <= -.3)
        attackDirection = DOWN;
      else
        attackDirection = NEUTRAL;
    }
    else if (abs(fixedXA) > .6)
      attackDirection = SIDE;
    else
      attackDirection = NEUTRAL;
    currentAnalog[XA]=abs(xAresult/maxhAnew); //take the absolute value so you don't give a negative number to the LEDs
    currentAnalog[YA]=abs(yAresult/maxhAnew);
    currentAnalog[XC]=abs(xCresult/maxhCnew);
    currentAnalog[YC]=abs(yCresult/maxhCnew);
    if (Lresult<0){Lresult=0.0;} //don't return a negative number for the leds
    if (Rresult<0){Rresult=0.0;}
    currentAnalog[L]=abs(Lresult);
    currentAnalog[R]=abs(Rresult);

    //create melee deadzones
    if (currentAnalog[XA]<.2875){currentAnalog[XA]=0.0;} 
    if (currentAnalog[YA]<.2875){currentAnalog[YA]=0.0;}
    if (currentAnalog[XC]<.2875){currentAnalog[XC]=0.0;}
    if (currentAnalog[YC]<.2875){currentAnalog[YC]=0.0;}
    if (currentAnalog[L]<.3){currentAnalog[L]=0.0;}
    if (currentAnalog[R]<.3){currentAnalog[R]=0.0;}
    if (currentAnalog[L]>1.0){currentAnalog[L]=1.0;}
    if (currentAnalog[R]>1.0){currentAnalog[R]=1.0;}
    /////////////////////////////////////////Analog data calculations end
}

void strobeLight() { //State machine to do a strobe light animation indefinitely
  switch(stateStrobe)
  {
    case 0: //Alt? Is this only going to light up 0,2,4 or will it light up 1,3?
      if (alt==0)
      {
        stateStrobe=1; //normal mode
      }
      else
      {
        stateStrobe=3; //alt mode
      }
    break;
    case 1: //Turn on Normal LEDs (0,2,4)
      time0Strobe=millis();
      leds[0] = CRGB(200,200,200);
      leds[2] = CRGB(200,200,200);
      leds[4] = CRGB(200,200,200);
      FastLED.show();
      stateStrobe=2;
    break;
    case 2: //On Normal
      time1Strobe=millis();
      if (time1Strobe-time0Strobe>delayStrobe) {stateStrobe=5;} //if the sufficient time has passed turn off the LEDs
    break;
    case 3: //Turn on Alt (1,3)
      time0Strobe=millis();
      leds[1] = CRGB(255,255,255);
      leds[3] = CRGB(255,255,255);
      FastLED.show();
      stateStrobe=4;
    break;
    case 4: //On Alt
      time1Strobe=millis();
      if (time1Strobe-time0Strobe>delayStrobe) {stateStrobe=5;} //if the sufficient time has passed turn off the LEDs
    break;  
    case 5: //Turn off
      time0Strobe=millis(); //update the timer
      alt=!alt; //switch to the other mode
      for(int i=0;i<5;i++) //turn the LEDs off
      {   
        leds[i] = CRGB(Black); 
      }
      FastLED.show(); 
      stateStrobe=6;     
    break;
    case 6: //Wait OFF
      time1Strobe=millis();
      if(time1Strobe-time0Strobe>delayStrobe) {stateStrobe=0;} //wait until enough time has passed then go to the beginning
    break;
  }
}

void initializeAnimations() // This function assigns each animation to the corresponding action
{
  animations[ACTION_X].animation = xStateMachine;
  animations[ACTION_X].triggered = false;
  animations[ACTION_Y].animation = yStateMachine;
  animations[ACTION_Y].triggered = false;
  animations[ACTION_L].animation = lStateMachine;
  animations[ACTION_L].triggered = false;
  animations[ACTION_R].animation = rStateMachine;
  animations[ACTION_R].triggered = false;
  animations[NEUTRAL_A].animation = aStateMachine;
  animations[NEUTRAL_A].triggered = false;
  animations[UPTILT].animation = NULL;
  animations[UPTILT].triggered = false;
  animations[DOWNTILT].animation = NULL;
  animations[DOWNTILT].triggered = false;
  animations[FTILT].animation = NULL;
  animations[FTILT].triggered = false;
  animations[NEUTRAL_B].animation = bStateMachine;
  animations[NEUTRAL_B].triggered = false;
  animations[SIDE_B].animation = NULL;
  animations[SIDE_B].triggered = false;
  animations[UP_B].animation = NULL;
  animations[UP_B].triggered = false;
  animations[DOWN_B].animation = downbStateMachine;
  animations[DOWN_B].triggered = false;
  animations[FSMASH].animation = NULL;
  animations[FSMASH].triggered = false;
  animations[DSMASH].animation = NULL;
  animations[DSMASH].triggered = false;
  animations[USMASH].animation = NULL;
  animations[USMASH].triggered = false; 
}

void lightUpButtons(){//Declarations for lighting up buttons using state machines 
  actionFinder(); // Look at the current input and try to deduce what is the current action
  if (currentAction != NO_ACTION) // If we are not idle, we trigger the corresponding animation
  {
    animations[currentAction].triggered = true;
  }
  for (int i=0; i < TOTAL_ANIMATIONS; i++)
  {
    if (animations[i].triggered && animations[currentAction].animation != NULL) // We check if we have a triggered animation. If so, run another cycle of it.
      animations[i].animation();
  }
  savePresses();
}
