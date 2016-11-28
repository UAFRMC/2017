/*
Parametric OpenSCAD sprocket.

Began life as "sprocket.scad" from here:
  http://www.thingiverse.com/thing:7918

Modified to taper and trim the tips, see:
   http://www.gizmology.net/sprockets.htm

*/

//   Tooth count, roller diameter, distance between rollers
module sprocket(teeth=20, roller=3, pitch=17, thickness=3, tolerance=0.2){
	// $fs=0.05;
	roller=roller/2; //We need radius in our calculations, not diameter
	distance_from_center=pitch/(2*sin(180/teeth));
	echo(distance_from_center);
	angle=(360/teeth);
	pitch_circle=sqrt((distance_from_center*distance_from_center) - (pitch*(roller+tolerance))+((roller+tolerance)*(roller+tolerance)));
	// echo(pitch_circle);
	
	taper_start=pitch_circle-0.4*roller; // radius at start of taper
	taper_slope=3.0;  // higher values -> more tip taper
	taper_outer=taper_start+taper_slope*thickness; // radius halfway through
	
	trim_circle=distance_from_center-roller-tolerance+0.6*pitch; // cuts off tips (constant to taste)
	
  intersection() { 
	union() { // taper tooth tips
		cylinder(r1=taper_start,r2=taper_outer,h=0.5*thickness);
		translate([0,0,0.5*thickness]) {
		   cylinder(r1=taper_outer,r2=taper_start,h=0.5*thickness);
		}
	}
	
	cylinder(r=trim_circle,h=thickness); // trim off the tooth tips
	
	difference(){
		union(){ // add tooth bases to inside disk
			cylinder(r=pitch_circle,h=thickness);
			for(tooth=[1:teeth]){
				intersection(){
					rotate(a=[0,0,angle*(tooth+0.5)]){
						translate([distance_from_center,0,0]){
							cylinder(r=pitch-roller-tolerance,h=thickness);
						}
					}
					rotate(a=[0,0,angle*(tooth-0.5)]){
						translate([distance_from_center,0,0]){
							cylinder(r=pitch-roller-tolerance,h=thickness);
						}
					}
				}
			}
		}
		for(tooth=[1:teeth]){ // tooth gullets
			rotate(a=[0,0,angle*(tooth+0.5)]){
				translate([distance_from_center,0,-1]){
					cylinder(r=roller+tolerance,h=thickness+2);
				}
			}
		}
	}
  }
}

// Numbers for #25 roller chain
module sprocket25(teeth) {
    sprocket(teeth, 0.130*25.4, 0.25*25.4, 0.120*0.93*25.4-0.3, 0.6);
}

// Numbers for #40 roller chain 
module sprocket40(teeth) {
   sprocket(teeth, 0.312*25.4, 0.5*25.4,  5/16*0.93*25.4-0.3, 0.8);
}

// Numbers for 3/32" bicycle chain
module sprocketBikeWide(teeth) {
   sprocket(teeth, 0.312*25.4, 0.5*25.4,  1/8*25.4-0.3, 0.8);
}

// sprocket40(8);
