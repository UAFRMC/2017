/*
  3D printed part to retain CNC cut sprocket.
*/
$fa=5;
$fs=0.05;
use <barbie_gearbox.scad>;
use <sprocketlib2D.scad>;

h=(16.0-3.4)/2;
wiggle=0.1;
gearbox_fatten=0.2;
gearbox_walls=2.5;

module gearbox_retainer(bottomplate=0.0) {
  difference() {
	union() {
	// Outside of gearbox output shaft
	barbie_gearbox_drive(height=h,fatten=gearbox_walls,wiggle=wiggle);
		
	// Big plate outside of shaft
	scale([1,1,-1])
	cylinder(h=bottomplate,d=52+2*gearbox_walls);
	}
	
	translate([0,0,-0.01])
		barbie_gearbox_drive(height=h+bottomplate+2,fatten=gearbox_fatten);
  }
}

bottomplate=2.0;
translate([0,0,bottomplate])
difference() {
	union() {
		// Fits over output shaft
		gearbox_retainer(bottomplate);

		// Fits into motor's bearing hole
		cylinder(d=24.5,h=8.3);
	}

	// Retaining thru-bolt
	translate([0,0,-10])
		cylinder(d=12.0,h=30);
}

translate([70.0,0,0]) gearbox_retainer();

