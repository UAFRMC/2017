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
        usleep(100*1000); // 100ms per control update
	
        //printw("character: %c\n",lastInput);
        //printw("hex: %x\n",lastInput);
        //printw("decimal: %d\n\n",lastInput);
	

        printw("%s %.2f %.2f\n", ((turbo)?"LR":"lr"),leftPower,rightPower);
        //printw("rightPower: %.2f\n",rightPower);
        printw("F %.1f\n",frontPower);
	printw("M %.1f\n",minePower);
        //printw("armsPower: %.2f\n",armsPower);
        printw("D %.1f\n",dumpPower);
	printw("timeUI %d \n",timeUI);
	printw("datatime %d \n",dataTime);
	printw("battery %d \n",batteryVoltage);
	printw("mine %d\n",mineVoltage);
	if(mineMode) printw("MINE\n");
	if (rattle) printw("RATTLE!\n");
	if (turbo) printw("HIGH!\n");
        
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

            leftPower=rightPower=frontPower=minePower=dumpPower=0;
		motors_send();
            break;
        }

	ui_update(keys,axes);
	timeUI++;
	motors_send();
    }
	
	endwin();
	return 0;
}
