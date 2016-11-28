$fa=5;
$fs=0.05;
use <barbie_gearbox.scad>;
use <sprocketlib2D.scad>;

sprocketz=3.4; // approx, see sprocketBikeWide for real value
fillet=3; // curving blend up to sprocket
h=16.0+fillet; // height over sprocket
wiggle=0.1;
gearbox_fatten=0.2;
gearbox_walls=2.5;

difference() {
	union() {
		// Chain drive sprocket
		sprocketBikeWide(27);
		
		// Outside of gearbox output shaft
		barbie_gearbox_drive(height=h,fatten=-gearbox_fatten,wiggle=0.0);
		
		// Fillet curve
		rotate_extrude() {
			cornerX=25.3+fillet;
			cornerZ=sprocketz+fillet;
			difference() {
				square([cornerX,cornerZ]);
				translate([cornerX,cornerZ])
					circle(r=cornerZ-3.4);
			}
		}
	}
	
	// mounting axle
	translate([0,0,-1])
	cylinder(d=3/8*25.4,h=h+2);
}
