/*
 * This file describes how animations are handled. If one wants to add a new animation they should respect the following:
 *      - Keep track of their cycle with a counter
 *      - !!!!! WHEN DONE, SET THE TRIGGER VALUE OF THE CORRESPONDING ACTION TO FALSE !!!!
 *      
 *      For example, if i add an action "DASHBACK", then at my last cycle i should do:
 *      
 *      animations[DASHBACK].triggered = false;
 * 
 *      This is vital to make this working.
 */
#ifndef __ANIMATION__
#define __ANIMATION__

#define TOTAL_ANIMATIONS 18 // Defining total number of animations

enum e_action {
  ACTION_X        = 0,
  ACTION_Y        = 1,
  ACTION_L        = 2,
  ACTION_R        = 3,
  NEUTRAL_A       = 4,
  UPTILT          = 5,
  DOWNTILT        = 6,
  FTILT           = 7,
  NEUTRAL_B       = 8,
  SIDE_B          = 9,
  UP_B            = 10,
  DOWN_B          = 11,
  FSMASH          = 12,
  DSMASH          = 13,
  USMASH          = 14,
  ACTION_START    = 15,
  ACTION_Z        = 16,
  NO_ACTION       = 17
};

typedef void (* Animation)(); // Array of function pointers towards animation functions. Used to store the different animations depending on the action.

typedef struct s_animationRegister {
  Animation animation;
  bool triggered;
} t_animationRegister;

t_animationRegister animations[TOTAL_ANIMATIONS];
e_action currentAction = NO_ACTION;
e_action lastAction = NO_ACTION;
unsigned long lastActionTiming = 0;

#endif
