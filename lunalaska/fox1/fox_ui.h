/**
LunAlaska keyboard user interface header:
	Read the keyboard (directly via ncurses, GLUT, or over the network)
	Prepare an array of key bools, one per key, indicating which keys are down.
	Call ui_update.  It writes the Power globals in the motor code.

Dr. Orion Lawlor's refactoring of Noah Betzen's code.
2013-05-16 (Public Domain)
*/

// These are user interface state variables
    float left=0;
    float right=0;
	bool rattle=false;
    float leftArm=0;
    float rightArm=0;
    float conveyor=0;
    float run=0;

    int leftGo=0;
    int rightGo=0;
    int conveyorExtend=0;
    int conveyorRun=0;

    bool turbo=false;
    bool mine=false;
    
    float powerLimit=1;
    float acceleration=.05;

    bool cTick=false;
    int sCount=0;


/*
  This function updates the UI state variables above, and 
   writes out motor Power levels, based on the current set of keys down.

  You MUST call this function every 40ms.

  keys: the keyboard keys that are currently pressed.
    Index runs at least to octal 0600 (see ncurses.h for constants).

  axes: the positions of the joystick axes.
*/
void ui_update(const bool keys[], const float axes[])
{
/* scale back, so things die away when key is released.
   It'd be better to detect key-up here, and stop, but that's life. */
        left*=0.95;
        right*=0.95;
        leftArm*=0.5;
	rightArm*=0.5;
        conveyor*=0.5;
	run*=0.5;

	if (rattle) { // turn off wheel power when exiting rattle mode
		left=right=0.0;
		leftPower=rightPower=0.0;
	        rattle=false;
	}

        if(keys[' ']) //spacebar--full stop
        {
            left=0;
            leftPower=0;
            right=0;
            rightPower=0;
            leftArm=0;
	    leftArmPower=0;
	    rightArm=0;
	    rightArmPower=0;
            conveyor=0;
            conveyorPower=0;
	    run=0;
	    runPower=0;
	
	    leftGo=0;
    	    rightGo=0;
    	    conveyorExtend=0;
    	    conveyorRun=0;
            mine=false;
        }
        
        if(keys[KEY_UP])
        {
            left+=acceleration;
            right+=acceleration;
        }
        if(keys[KEY_DOWN])
        {
            left-=acceleration;
            right-=acceleration;
        }
        if(keys[KEY_LEFT])
        {
            left-=acceleration;
            right+=acceleration;
        }
        if(keys[KEY_RIGHT])
        {
            left+=acceleration;
            right-=acceleration;
        }
        if(keys['i'])
        {
            if(leftGo==1) leftGo=0;
	    else leftGo=1;
        }
        if(keys['k'])
        {
            if(leftGo==-1) leftGo=0;
	    else leftGo=-1;
        }
	if(keys['m'])
        {
            mine=!mine;
	    if(!mine){
		conveyorExtend=1;
		conveyorRun=0;
	    }
        }
	if(keys['o'])
        {
            if(rightGo==1) rightGo=0;
	    else rightGo=1;
        }
        if(keys['l'])
        {
            if(rightGo==-1) rightGo=0;
	    else rightGo=-1;
        }
        if(keys['u'])
        {
            if(conveyorExtend==1) conveyorExtend=0;
	    else conveyorExtend=1;
	    mine=false;
        }
        if(keys['j'])
        {
            if(conveyorExtend==-1) conveyorExtend=0;
	    else conveyorExtend=-1;
	    mine=false;
        }
	if(keys['y'])
        {
            if(conveyorRun==1) conveyorRun=0;
	    else conveyorRun=1;
	    mine=false;
        }
        if(keys['h'])
        {
            if(conveyorRun==-1) conveyorRun=0;
	    else conveyorRun=-1;
	    mine=false;
        }
	if(keys['p'])
        {
            turbo=!turbo;
        }
	if (keys['r']) 
	{
		leftGo=0;
		rightGo=0;
		rattle=true;
		static int phase=0;
		float rattlePower=1;
		phase++;
		if (phase%4<2) left=right=rattlePower;
		else left=right=-rattlePower;
		
	}

	cTick=(arduinoStatus>>2)&1;
	if(mine){
		conveyorRun=1;
		if(sCount<6)
			conveyorExtend=-1;
		else
			conveyorExtend=1;
		if(cTick) sCount=0;
		sCount++;
	}
	
	//Set powers according to toggles
	if(leftGo==1) leftArm+=1.0;
        else if(leftGo==-1) leftArm-=1.0;
	else leftArm=0;

	if(rightGo==1) rightArm+=1.0;
        else if(rightGo==-1) rightArm-=1.0;
	else rightArm=0;
        
	if(conveyorExtend==1) conveyor+=1.0;
        else if(conveyorExtend==-1) conveyor-=1.0;
	else conveyor=0;

	if(conveyorRun==1) run+=1.0;
        else if(conveyorRun==-1) run-=1.0;
	else run=0;
	
	if(turbo) powerLimit=1;
	else powerLimit=.4;
	
	float smoothing=0.4; // maximum change to apply per timestep
        smooth(leftPower,left=limit(left,-powerLimit,+powerLimit),smoothing);
        smooth(rightPower,right=limit(right,-powerLimit,+powerLimit),smoothing);
        leftArmPower=leftArm=limit(leftArm,-1.0,1.0);
	rightArmPower=rightArm=limit(rightArm,-1.0,1.0); // limit(arms,-powerLimit,+powerLimit); //???
        conveyorPower=conveyor=limit(conveyor,-1.0,1.0);
	runPower=run=limit(run,-1.0,1.0); // limit(conveyor,-powerLimit,+powerLimit); //???       

	// power levels are now set!
}


