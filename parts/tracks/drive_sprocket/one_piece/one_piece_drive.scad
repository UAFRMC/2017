// One-piece
//   parameterizable Drive Sprocket for Robot Mining
// Ryker Dial
// October 25, 2015

include <drive_param.scad>
include <roundlib.scad>

$fa=20; // coarse preview
fa_outside=5; // angle tolerance for exterior surface is better

// Circumference of sprocket:
sprocket_circum = hole_num*hole_period;
// Outside radius of sprocket
sprocket_radius = sprocket_circum/(2*PI);

// Thickness of spacer between drive sprockets
spacer_thick=11; // from spacer.scad, plate_thick
sprocket_plate=10; // thickness of drive sprocket plate
drive_mount_ht=0.5*25.4+32; // total height of drive mount, including gearbox drive pegs

hole_depth = 20; // inset for drive peg hole
hole_diam_outer = 32; //37; // size of entire clearance around drive peg hole
hole_diam_inner = 23; // diameter of drive peg hole, inside corner
teeth_top_depth = 17.5; // height of drive pins on top
base_extra = 0; // Add extra height to sprocket
sprocket_height = 32+base_extra;

hole_tilt=5; // degrees that peg holes are tilted inwards
hole_hangle=360/hole_num/2; // half-angle between holes

hole_Z_center=25-hole_diam_outer/2; // Z height to center of drive sprocket


barbie_gearbox_height=16.5; // Z size of motor drive cross

sprockets_height=sprocket_height+spacer_thick+sprocket_height;
overall_height=sprocket_height+spacer_thick+sprocket_plate+drive_mount_ht;

wiggle=0.1; // distance to add around holes so parts fit
wall=1.3; // thickness of main walls
rib_rim=2.0; // thickness of outlines around ribs
drive_outside=50+2*wall; // diameter of support ring outside gearbox



module peg_hole_trapezoid(fatten,add_width,ht) {
  hull(){
    cylinder(h=ht, d1= hole_diam_inner+fatten, d2=hole_diam_outer+add_width+fatten);
    translate([hole_depth, 0, 0])
        cylinder(h=ht, d1= hole_diam_inner+fatten, d2=hole_diam_outer+add_width+fatten);
  }
}


// Put objects at peg hole centers.
//   Leaves coordinate system with Z up into peg
module peg_hole_array() {
	for(i=[0:hole_num-1])
			rotate([0,
					270,
					i*360/hole_num])
				children();
}

// Make all drive peg holes, fattened by this much
module peg_holes(fatten=0.0) {
	// Peg holes
	translate([0,0,hole_Z_center+hole_diam_outer/2])
		peg_hole_array() {
			rotate([0,-hole_tilt,0]) {
				translate([0,0,sprocket_radius-hole_depth]){
					peg_hole_trapezoid(fatten,0,hole_depth+3);
					translate([0,0,hole_depth-7])
					  rotate([0,hole_tilt,0])
						peg_hole_trapezoid(2*fatten,10,10);
				}
			}
		}
}

// Teeth of drive sprocket
module sprocket_teeth(){
    difference(){
        cylinder(r=sprocket_radius, h=sprocket_height);
		
        // Remove center material
        translate([0,0,sprocket_plate]){
            cylinder(r1=sprocket_radius-hole_depth+10*epsilon, r2=sprocket_radius-teeth_top_depth, h=sprocket_height+epsilon-sprocket_plate);
        }
		
		peg_holes();
		
        // Center hole
        translate([0,0,-(epsilon+1)]){
            cylinder(d=center_hole_diam+wiggle, h=sprocket_height);
        }
        // Hole for bearing
        translate([0, 0, -(epsilon+1)]){
            cylinder(d=bearing_outer_diam+wiggle, h=bearing_width+wiggle+1);
        }
	}
}






// Bearing that fits on axle
module axle_bearing(fatten=0.0) {
	cylinder(d=bearing_outer_diam+2*wiggle+fatten, h=bearing_width+fatten+wiggle+1);
}


// Holes to fit all the projections associated with a Barbie jeep gearbox:
module barbie_gearbox(fatten=0.0)
{
    radius=16;
    cube_width=13;
    cube_diameter=50;
    // Main body
	translate([0,0,-barbie_gearbox_height-fatten])
		linear_extrude(height=barbie_gearbox_height+fatten,convexity=4)
			round_2D(1.5)
			offset(r=wiggle+fatten)
			{
				circle(r=radius,center=true);
				square([cube_width,cube_diameter],center=true);
				square([cube_diameter,cube_width],center=true);
			}

	// Inside bearing hole
	translate([0, 0, -barbie_gearbox_height-bearing_width-fatten]){
		axle_bearing(fatten);
	}

	// Thru axle hole
	translate([0, 0, -overall_height]){
		cylinder(d=11+2*(wiggle+fatten), h=overall_height);
	}
}

// 2D ring with a hole in the middle
module holy_ring(ro,ri,h) {
	difference() {
		circle(r=ro,$fa=fa_outside);
		circle(r=ri,$fa=fa_outside);
	}
}

// Cylinder with a hole in the middle
module holy_cylinder(ro,ri,h) {
	difference() {
		cylinder(r=ro,h=h);
		translate([0,0,-epsilon])
			cylinder(r=ri,h=h+2*epsilon);
	}
}

tooth_dy=9; // Y coordinate of start of gear tooth

// Reinforcing carbon fiber rods ('rebar')
rebar_ID=4.5;
rebar_wall=wall;
rebar_OD=rebar_ID+2*rebar_wall;
rebar_dx=sprocket_radius-rebar_OD/2-0.5;
rebar_dy=-tooth_dy+rebar_OD/2+3; // no overlap, center rib reinforce

module rebar_array() {
	n_rebar=3; // 9;
	for(i=[0:n_rebar-1])
			rotate([0,0,i*360/n_rebar])
				children();
}

module rebar(dia,extra_z=0.0) {
	d=dia;
	
	rebar_array()
	{
		// Vertical sprocket reinforcers
		translate([rebar_dx,rebar_dy,-extra_z])
			cylinder(d=d,h=sprockets_height*1.1+2*extra_z);
		
/*		// Diagonal drive lines
		translate([sprocket_radius,-tooth_dy-1,sprockets_height-rebar_OD/2-1])
			rotate([-90+18,0,90-29])
				translate([0,0,-extra_z])
					cylinder(d=d,h=sprockets_height+2*extra_z);
*/
	}
}



module drive_ribs(fatten=0.0) {
	drive_dy=drive_outside*0.42;
	drive_dx=drive_outside*0.23;
	linear_extrude(h=overall_height,convexity=15) 
	round_2D(wall*0.4) 
	offset(r=fatten)
	{
		rebar_array() translate([rebar_dx,rebar_dy]) circle(d=rebar_OD);
		
		holy_ring(sprocket_radius,sprocket_radius-wall); // outside
		holy_ring(bearing_outer_diam/2+wall,bearing_outer_diam/2); // bearing
		holy_ring(drive_outside/2+wall/2,drive_outside/2-wall/2); // drive
		
		for (hole_i=[0:hole_num-1]) rotate([0,0,360*hole_i/hole_num])
			union() {
				// Linear spokes
				translate([0,-wall/2+tooth_dy*0.70,0]) rotate([0,0,0]) square([sprocket_radius,wall]);
				left_start=0; // drive_outside/2-wall;
				translate([left_start,-wall/2-tooth_dy*0.70,0]) rotate([0,0,0]) square([sprocket_radius-left_start,wall]);
				
				
				// Diagonal spoke(s)
				end_x=sprocket_radius*cos(hole_hangle)+2*wall;
				//for (flip=[-1,+1]) scale([1,flip,1])
				polygon([
					[end_x,-tooth_dy],
					[end_x,-tooth_dy+wall],
					[drive_dx,drive_dy+wall],
					[drive_dx,drive_dy]
				]);
				
			}
	}
}

// Overall shape of drive sprocket
module drive_profile(zshrink=0.0) {
	translate([0,0,+zshrink])
	difference() {
		union() {
			// Base
			cylinder(r=sprocket_radius,h=sprockets_height-2*zshrink,$fa=fa_outside);
			// Trim up to motor
			translate([0,0,sprockets_height-2*zshrink-epsilon])
				cylinder(r1=sprocket_radius,r2=drive_outside/2,h=overall_height-sprockets_height);
		}
	}
}

// Hole in middle drive sprocket
module drive_middle_clear(fatten=0.0) {
	rotate_extrude(convexity=2) 
	intersection() {
		square([200,200]); // keep away from X axis
		
		round_2D(12.0) 
		offset(r=fatten)
		{
			polygon([
				[epsilon,overall_height-20],
				[sprocket_radius*0.7,overall_height-50],
				[sprocket_radius*0.9,10],
				[epsilon,10]
			]);
		}
	}
}

// Walls around each of the drive pegs
module pegwalls(fatten=0.0) {
	intersection() {
		union() {
			// Bottom teeth:
			translate([0,0,sprocket_height]) scale([1,1,-1]) children();

			// Top teeth:
			translate([0,0,sprocket_height+spacer_thick]) children();
			
		}
		// Trim donut
		holy_cylinder(ro=sprocket_radius+fatten,ri=sprocket_radius-hole_depth*0.8-fatten,
			h=sprockets_height+fatten);
	}
}



module drive_plus() {
	// Main ribs
	intersection() { 
		union() { drive_ribs(); rebar(dia=rebar_OD); }
		drive_profile(); 
	};
	
	// Rim of ribs (interior)
	difference() { 
		intersection() { drive_ribs(rib_rim*0.5); drive_profile(); };
		difference() { drive_profile(rib_rim); drive_middle_clear(rib_rim); };
	};
	
	translate([0,0,overall_height]) barbie_gearbox(wall);
	
	pegwalls() peg_holes(2*wall);
	
	axle_bearing(2*wall);
}

module drive_minus() {
	pegwalls(1.0) peg_holes(0.0);
	
	translate([0,0,overall_height]) barbie_gearbox();
	
	translate([0,0,-epsilon]) axle_bearing(0.0);
	
	drive_middle_clear();
	
	/*
	// Lightening holes in middle of each sprocket
	translate([0,0,sprocket_height+spacer_thick/2])
	rotate([0,0,hole_hangle]) peg_hole_array() translate([0,0,sprocket_radius+10]) scale([1,1,-1]) cylinder(d=20,h=60);
	*/
	// Space for actual rebar
	rebar(dia=rebar_ID,extra_z=3.0);
}

module drive_final() {
	difference() {
		drive_plus();
		drive_minus();
	}
}


/* Ribs: 
difference() {
	intersection() { union() { drive_ribs(); rebar(dia=rebar_OD); } drive_profile(); };
	rebar(dia=rebar_ID,extra_z=3.0);
}
*/

/*
// Drive peg walls:
difference() {
	pegwalls() peg_holes(2*wall);
	pegwalls(1.0) peg_holes(0.0);
}
*/

//drive_plus();
//drive_minus();
drive_final();




