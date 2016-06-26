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
    float front=0;
    float mine=0;
    
    float powerLimit=1;
    float acceleration=.2;
    
    bool rattle=false;
    bool turbo=false;
    bool mineMode=false; // controls continous mining


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
   It'd be better to detect key-up here, and stop, but that's ncurses. */
        left*=0.95;
        right*=0.95;
        front*=0.5;
	mine*=0.5;
        dump*=0.5;

	if (rattle) { // turn off wheel power when exiting rattle mode
		left=right=0.0;
		leftPower=rightPower=0.0;
	        rattle=false;
	}

        if(keys[' ']) //spacebar--full stop
        {
            left=0;
            right=0;
            front=0;
	    mine=0;
	    dump=0;
            mineMode=false;
        }
        
        if(keys['w'])
        {
            left+=acceleration;
            right+=acceleration;
        }
        if(keys['s'])
        {
            left-=acceleration;
            right-=acceleration;
        }
        if(keys['a'])
        {
            left-=acceleration;
            right+=acceleration;
        }
        if(keys['d'])
        {
            left+=acceleration;
            right-=acceleration;
        }
	
	
        if(keys['f'])
        {
            front=1;
        }
        if(keys['v'])
        {
            front=-1;
        }
	
	
	if(keys['m'])
        {
            mineMode=!mineMode; // activate/deactivate minemode
	    //mine=1;
        }
	if(keys['j'])
        {
	    mineMode=false; // deactivate mining before reversing 
            mine=-1;
        }
	
        if(keys[KEY_DOWN])
        {
            dump=-1;
        }
        if(keys[KEY_UP])
        {
            dump=+1;
        }
	
	if (keys['r']) 
	{
		rattle=true;
		static int phase=0;
		float rattlePower=1;
		phase++;
		if (phase%4<2) left=right=rattlePower;
		else left=right=-rattlePower;
		
	}
	if(keys['p'])
	{
	  turbo =  !turbo;
	  mineMode=false; // safety, ratlle will disable mining mode
	}
	if(mineMode) mine=1; // continous mining when mining mode is activated
	if(turbo) powerLimit=0.5;
	else powerLimit=.3;
	
	float smoothing=0.4; // maximum change to apply per timestep
        smooth(leftPower,left=limit(left,-powerLimit,+powerLimit),smoothing);
        smooth(rightPower,right=limit(right,-powerLimit,+powerLimit),smoothing);
        frontPower=limit(front,-1.0,1.0);
	minePower=limit(mine,-1.0,1.0); // limit(arms,-powerLimit,+powerLimit); //???
        dumpPower=limit(dump,-1.0,1.0);
	// power levels are now set!
}


