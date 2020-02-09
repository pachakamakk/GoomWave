

void actionFinder() {
  currentAction = NO_ACTION;
  if (!oldPress[A] && currentPress[A]) { //if it was a 0 and then is a 1 (it was pressed down) then turn on the state machine
    if (attackDirection == NEUTRAL)
      currentAction = NEUTRAL_A;
    else if (attackDirection == SIDE)
      currentAction = FTILT;
    else if (attackDirection == UP)
      currentAction = UPTILT;
  }
  if(!oldPress[B] && currentPress[B]) { //if it was a 0 and then is a 1 (it was pressed down) then turn on the state machine
    if (attackDirection == NEUTRAL)
      currentAction = NEUTRAL_B;
    else if (attackDirection == SIDE)
      currentAction = SIDE_B;
    else if (attackDirection == UP)
      currentAction = UP_B;
    else if (attackDirection == DOWN)
      currentAction = DOWN_B;
  }
  if(!oldPress[X] && currentPress[X]) {  //if it was a 0 and then is a 1 (it was pressed down) then turn on the state machine
    currentAction = ACTION_X;
  }
  if(!oldPress[Y] && currentPress[Y]) {  //if it was a 0 and then is a 1 (it was pressed down) then turn on the state machine
    currentAction = ACTION_Y;
  }
  if(!oldPress[L] && currentPress[L]) {  //if it was a 0 and then is a 1 (it was pressed down) then turn on the state machine
    currentAction = ACTION_L;
  }
  if(!oldPress[R] && currentPress[R]) {  //if it was a 0 and then is a 1 (it was pressed down) then turn on the state machine
    currentAction = ACTION_R;
  }
  if(!oldPress[Z] && currentPress[Z]) {  //if it was a 0 and then is a 1 (it was pressed down) then turn on the state machine
    currentAction = ACTION_Z;
  }
  if(!oldPress[START] && currentPress[START]) {
    currentAction = ACTION_START;
  }  //if it was a 0 and then is a 1 (it was pressed down) then turn on the state machine
}

void savePresses() {
   oldPress[A] = currentPress[A]; //a update
   oldPress[B] = currentPress[B]; //b update
   oldPress[X] = currentPress[X]; //x update
   oldPress[Y] = currentPress[Y]; //y update
   oldPress[L] = currentPress[L]; //l update
   oldPress[R] = currentPress[R]; //r update
   oldPress[Z] = currentPress[Z]; //z update
   oldPress[START] = currentPress[START]; //s update  
}
