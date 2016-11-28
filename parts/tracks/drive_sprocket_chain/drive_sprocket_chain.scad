$fa=5;
$fs=0.05;
use <barbie_gearbox.scad>;
use <sprocketlib2D.scad>;

h=12.5;
wiggle=0.1;
gearbox_fatten=0.2;
gearbox_walls=2.5;

difference() {
	union() {
		// Chain drive sprocket
		sprocketBikeWide(19);
		
		// Outside of gearbox output shaft
		barbie_gearbox_drive(height=h,fatten=gearbox_walls,wiggle=wiggle);
		
		// giant fillet between them
		rotate_extrude() {
			cornerX=33;
			cornerZ=h;
			difference() {
				square([cornerX,cornerZ]);
				translate([cornerX,cornerZ])
					circle(r=cornerZ-3.4);
			}
		}
	}
	translate([0,0,-1])
		barbie_gearbox_drive(height=h+2,fatten=gearbox_fatten);
}
