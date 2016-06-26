This is the public repo for UAF's NASA Robot Mining Contest team,
Aurora Robotics.
This is the firmware for UAF's entry in the NASA Robot Mining Competition.

Public access:
	git clone http://projects.cs.uaf.edu/robotmining

Ask Dr. Lawlor to get write access!


Laptop battery power check:
	cat /sys/class/power_supply/BATX/charge_*
The first two are charge values for full; the last is current charge.


-------------
As-built storage bucket dimensions:
  17.75 inch * 17 inch * 3.5 inch = 17.3 liters -> 17Kg at 1 Kg/L density (fluffed)


----------
New 2014-05 board:

Power plug:
  tx1 via opto-isolator: orange/white
  a8: orange, not connected (avoid noise on opto line)
  a9: blue, mine voltage
  a10: green, spare input
  a11: brown, 24v bus
  brown/white: motor side ground

Front plug:
  53/A15: blue, front wheel right encoder
  52: orange, (5v) encoder supply
  51: spare button input
  50/A14: green, front wheel left encoder (optional)
  White: all grounds

Back/bucket plug:
  49: orange, (5v) encoder supply
  48: orange/white, spare input
  47: green, back right button
  46/A13: blue back bucket
  45: brown/white, spare
  44: brown & blue/white, back left button 
  43: spare input


Infrared board: pink RJ45 wires
A5	A1	orange/white: read side of mining detector
   5		orange: 5V feed for ALL ir detectors (soft power for both)

A6	A2	green/white: ir detector, bin-full
3	A4	green: LED, bin-full

4	7	blue: LED, bin-half-full
A7	A3	blue/white: ir detector, bin-half-full
	
 hardwired	brown/white: LED ground (COMMON ground, hardwired)
2	6	brown: LED for mining encoder

Wiring hookup: 
mining:  brown brown/white LED    orange orange/white detector
full:	green  brown/white LED    orange green/white detector
half:   blue   brown/white LED    orange blue/white detector



Actuator Ecoder wires:
Yellow: High
White: GND
Purple: Signal

Linear actuator to RJ45 wiring:
	yellow: 5V for potentiometer position encoder, orange on RJ-45.
	white: ground for potentiometer, white/blue on RJ-45.
	purple: potentiometer output, blue or green on RJ-45.
	red: motor+, either brown (front) or separate wires (bucket)
	black: motor-, either white/brown or separate wires



# Add backend and viewer directories to path
AURORA=/home/robot/robotmining/autonomy
PATH="$AURORA/aruco/viewer:$AURORA/backend:$PATH"
export PATH




-------------
OLD 2014-03 board:

OLD: Internal voltage monitoring: 2014-03 board (OLD)
	A8: orange battery 12V bus
	A9: battery ground (drive sabertooth)
	A10: brown battery 24V bus
		Weirdly, it reads about 3VDC lower than reality, possibly due to some sort of current backfeed in the voltage divider.
		Battery sags by about 15DN under a 10A load.
	A11: battery ground (mine sabertooth)
	A12: green mining motor voltage
		Typical run voltage is 10-13DN less than battery.
		Stall voltage is 20-22DN less than battery.
		Firmware calls it "stalled" with >18DN difference.
		Swaping to a CAT5 cable, still about 10DN run, but 30+DN stall.
	
OLD: External switches:
	44- left bumper switch (brown & white/green ground)
	47- right bumper switch (green & white/green ground)

OLD: Dump actuator potentiometer hookup:
	49-orange (5V) & white/blue (gnd)
	46 - blue - analog out for dump potentiometer (jumpered to A0 for now)

