// Bracket to connect barbie jeep motor to 3/4" EMT, to wind up storage bag

include <drive_param.scad>
include <roundlib.scad>

$fa=20; // coarse preview
$fa=5; // fine final version
$fs=0.1;

EMT_tube_OD=0.710*25.4; // 1/2" EMT
thrupin_OD=1/8*25.4; // pins retaining the EMT
axle_OD=0.375*25.4;

barbie_gearbox_height=16;
overall_height=40;

wiggle=0.5;

// Holes to fit all the projections associated with a Barbie jeep gearbox:
module barbie_gearbox(fatten=0.0)
{
    radius=16;
    cube_width=13;
    cube_diameter=50;
    // Main body
	translate([0,0,-barbie_gearbox_height-fatten+epsilon])
		linear_extrude(height=barbie_gearbox_height+fatten,convexity=4)
			round_2D(1.5)
			offset(r=wiggle+fatten)
			{
				circle(r=radius,center=true);
				square([cube_width,cube_diameter],center=true);
				square([cube_diameter,cube_width],center=true);
			}

/*
	// Inside bearing hole
	translate([0, 0, -barbie_gearbox_height-bearing_width-fatten]){
		axle_bearing(fatten);
	}
*/

	// Thru axle hole
	translate([0, 0, -overall_height]){
		cylinder(d=11+2*(wiggle+fatten), h=overall_height);
	}
}



module winder_minus() {

// 3/8" roller bearing
bearing_z=barbie_gearbox_height-2*epsilon;
translate([0,0,bearing_z])
	cylinder(d=bearing_outer_diam, h=bearing_width);
	
// EMT tube
translate([0,0,bearing_z+bearing_width])
	cylinder(d=EMT_tube_OD+wiggle,h=50);

// Carbon fiber reinforcing rods (keeps layers from splitting)
for (side=[45:90:360])
	rotate([0,0,side])
		translate([20,0,0])
			rotate([0,0,0])
				translate([0,0,-5])
					cylinder(d=4.5,h=50);

// Through pins to secure EMT
for (side=[-1,+1])
for (pin=[0,1])
	translate([0,0,6*pin])
		rotate([0,0,-45+pin*90])
			rotate([0,-90,0])
				translate([30,side*(axle_OD+thrupin_OD)/2,0])
					cylinder(d=thrupin_OD,h=100,center=true);

rotate([0,180,0]) barbie_gearbox();

}


module winder() {
	difference() {
		hull() {
			cylinder(d=56,h=20);
			cylinder(d=48,h=42);
		}
		winder_minus();
	}
}

winder();
//winder_minus();


