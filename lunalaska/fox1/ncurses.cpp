//
//  main.cpp
//  lunbaboticsGLUT
//
//  Created by Noah Betzen on 2/24/13.
//  Copyright (c) 2013 Noah Betzen. All rights reserved.
//

#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
using std::cout;


#include "fox_motor.h"
#include "fox_ui.h"

int main()
{
    motors_setup(); //Connect to Arduino

    //Initial set up attributes
	initscr();	
	noecho();		//doesn't print the values for the keys pressed
	raw();			//makes keyboard input imediately available for getch
	timeout(0); // non-blocking getch 
	keypad(stdscr,true);	//turns the keypad on (alloing arrow keys and fkeys)
	scrollok(stdscr,TRUE); //lets screen scroll endlessly    

    int input=0;
    int lastInput=0;
    
    
//TESTS TO SEE WHAT KEYS CORRESPOND TO WHICH VALUES
/*    while(input!=0x7f) //backspace on MBP
    {
        if(input==ERR)
        {
            input=getch();
            continue;
        }
        clear();
        usleep(30*1000);
        printw("character: %c\n",input);
        printw("hex: %x\n",input);
        printw("decimal: %d\n",input);
        refresh();
        input=getch();
    }*/

	enum {nkeys=0600}; // octal key length
	bool keys[nkeys];
	enum {naxes=8}; // joystick axes
	float axes[naxes]={0.0};

    while(true) 
    {
        clear();
        usleep(40*1000); // 40ms per control update
	
        //printw("character: %c\n",lastInput);
        //printw("hex: %x\n",lastInput);
        //printw("decimal: %d\n\n",lastInput);

        printw("%s %.2f %.2f\n", ((turbo)?"LR":"lr"),left,right);
        //printw("rightPower: %.2f\n",rightPower);
        printw("LA %.0f\n",leftArm);
	printw("RA %.0f\n",rightArm);
        //printw("armsPower: %.2f\n",armsPower);
        printw("CE %.0f\n",conveyor);
	printw("CP%s %.0f\n",((sCount>10)?"!!!":""),run);
	printw("SW %s  %s\n", ((arduinoStatus>>1)&1)?"L":" ", (arduinoStatus&1)?"R":" ");
	printw("MineCount: %d", sCount);
	if(mine) printw("MINE\n");
	if (rattle) printw("RATTLE!\n");
        //printw("conveyorPower: %.2f\n\n",conveyorPower);
/*
        printw("Writing to left: %d\n",powers[0]);

        printw("Writing to right: %d\n",powers[1]);
 
        printw("Writing to arms: %d\n",powers[2]);

        printw("Writing to conveyor: %d\n\n",powers[3]);
*/
        
	for (int i=0;i<nkeys;i++) keys[i]=false;

        refresh();
	input=ERR;
	while(1) { // clear out key buffer
		int cur=getch();
		if (cur==ERR) break;
		else input=cur;
        }


        if(input!=ERR)
        {
		keys[input]=true;
            lastInput=input;
        }
	
        if(input==KEY_BACKSPACE || input==0x1B) // escape
        {
		printw("QUIT--escape or backspace\n");
		
		// The robot jerks on exit, even with this.
		//  Skip this?  Still jerks.  Annoying.

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
		motors_send();
            break;
        }

	ui_update(keys,axes);
	motors_send();
    }
	
	endwin();
	return 0;
}
