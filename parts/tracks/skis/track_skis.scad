/*
  The "skis" keep the track aligned and tensioned between the drive rollers.  They are a replacement for elf shoes.
 
  2016-02-19
*/
include <../peg_param.scad>;
// $fa=50; $fs=3.0; fn_offset=8; // fast and coarse
$fa=5; $fs=0.2; fn_offset=16; // smooth and nice

peg_clear=3; // enlarge peg slots this much in all directions
peg_fillet=5; // smooth peg joints this much

// Ski mounting bolt locations:
//    2.875" between inner holes
//    4.5" between outer holes

// Big bottom skis:
peg_linear=100; // big straight-line portion of ski
spring_z=4.5/2*25.4; // z coord of centerline of spring bolt

// Small top skis:
//peg_linear=50; // small top skis
//spring_z=peg_linear-15; // z coord of centerline of spring bolt -> 2.875 inches between pegs

spring_x=0; // -0.375*25.4; // x coord of centerline of spring bolt
spring_y=0.25*25.4; // y coord of bottom of spring bolt



// Carbon fiber rod diameters
rod_crossmount=4.5;
rod_wheelmount=4.5;

    
peg_curve_radius=75; // curved portion radius of curvature
peg_curve_size_y=40; // curved portion dimensions
peg_curve_size_z=45;

// Overall height
totz=peg_linear+peg_curve_size_z;

// Ski tips have little wheels
ski_wheel_radius=30;
ski_wheel_thick=23;

ski_axle_dia=rod_wheelmount; // 3.5; // M3 screw as axle

ski_wheel_axle_z=totz-ski_wheel_radius+10;
ski_wheel_axle_y=ski_wheel_radius-2;
ski_wheel_clearance=0.7; // between wheel and ski
ski_wheel_xoutside=ski_wheel_thick*0.5+ski_wheel_clearance;



wall=1.2;
rib=0.9;
top_plate=2*25.4; // Y coordinate of top surface
plate_tilt=-8; // degrees of tilt back (easier printing curved surfaces)


epsilon=0.05;

// 2D cross section of one peg
module peg_2D(wall=0.0) {
    offset(r=peg_clear+wall) {
        hull() {
            peg_2D_oneside();
            mirror() peg_2D_oneside();
        }
    }
}

// 2D cross section of both tracks of pegs
module pegtracks_2D(wall=0.0,wider=0.0) {
    offset(r=-peg_fillet,$fn=fn_offset) 
    offset(r=peg_fillet+wall,$fn=fn_offset) 
    union() {
        scale([1,-1,1])
            translate([-50+1.1*wall-wider,0])
                square([100-2.2*wall+2*wider,25-2*wall]);
        translate([+25,0]) peg_2D();
        translate([-25,0]) peg_2D();
    }
}

// New rounded 2D cross section
module pegtracks_round_2D(wider=0.0) {
	offset(r=5+wider) offset(r=-5)
	difference() {
		wide=50; // half-width in X of whole part
		flattop=15; // Y end of box
		overtop=40; // Y end of part
		union() {
			translate([0,flattop]) scale([1,(overtop-flattop)/wide]) circle(r=wide);
			translate([-wide,0]) square([2*wide,flattop]);
		}
		pegtracks_2D();
	}
}
// pegtracks_round_2D();


// Isolates curved portion of tracks
module pegtrack_topcube(big=0.0) {
    translate([-100-big,-big,peg_linear-big])
            cube([200+2*big,peg_curve_size_y+2*big,peg_curve_size_z+2*big]);
}

// Take this 2D cross section, and extrude the linear and arc sections
module pegtrack_linear_and_rotated(big=0.0) {
    translate([0,0,-big])
    linear_extrude(height=peg_linear+epsilon+2*big,convexity=8) children();
    
    intersection() {
        pegtrack_topcube(big);
        translate([0,+peg_curve_radius,peg_linear])
            rotate([90,90,90]) // rotate about global X axis
                rotate_extrude(convexity=8,$fn=32) 
                    rotate([0,0,90])
                        translate([0,-peg_curve_radius,0])
                            children();
    }
}



// Ski wheel, down flat and printable
module ski_wheel_flat(clearance=0.0) {
    y=0.5*ski_wheel_thick+clearance;
	wheel_rounded=0.6*ski_wheel_thick;
    rotate_extrude() hull() {
        translate([ski_wheel_radius-wheel_rounded+2*clearance,0])
			offset(r=2*clearance)
			intersection() {
				translate([0,-y]) square([ski_wheel_radius,2*y]);
				circle(d=2*wheel_rounded); // +5*clearance);
			}
        translate([ski_axle_dia*0.5-clearance,-y,0])
            square([1,2*y]);
    }
}


// Ski wheel floating up at real position
module ski_wheel_up(clearance=0.0) {
translate([0,ski_wheel_axle_y,ski_wheel_axle_z])
    rotate([0,90,0])
        ski_wheel_flat(clearance);
}

module M3x50mm(wall=0.0,len=0.0) {
		bolt_length=50; // mm length of bolt
        cylinder(d=2.6+2*wall,h=bolt_length+len); // threaded portion
        cylinder(d=3.2+2*wall,h=35); // clearance portion
		scale([1,1,-1])
			translate([0,0,-epsilon])
				cylinder(d=7.5-wall,h=50-wall); // socket head
}

// All mounting bolts:
module ski_bolts(wall=0.0,len=0.0) {
    // wheel axle
    translate([-50,ski_wheel_axle_y,ski_wheel_axle_z])
        rotate([0,90,0])
			cylinder(d=rod_wheelmount,h=200);
            //M3x50mm(wall); 
    
    // quarter inch spring bolt
	//for (side=[-1,+1]) scale([side,1,1])
		translate([spring_x,spring_y,spring_z-len])
			rotate([-90,0,0])
				cylinder(d=0.25*25.4+2*wall,h=top_plate-spring_y+2*len);
    
}

// Tips of peg tracks are chamfered
module pegtrack_chamfer_tips(wall=0.0) {
    intersection() {
        pegtrack_topcube();
            union()
                for (side=[-1,+1]) scale([side,1,1])
                    translate([30,10,totz-30])
                        cylinder(d1=10+2*wall,d2=55+2*wall,h=32);
    }
}

// Everything added to the skis:
module ski_plus() {
    // Curved floor:
    pegtrack_linear_and_rotated(0.0) pegtracks_round_2D();
	
	// "volcanos" around mounting bolts
	for (side=[-1,+1]) scale([side,1,1])
		translate([spring_x,30,spring_z])
			rotate([-90,0,0])
				cylinder(d2=6,d1=36,h=20);
	
/*    
    pegtrack_chamfer_tips(2*wall);
    // Ribs:
    // x ribs
    for (side=[-1,+1]) scale([side,1,1])
        for (x=[ski_wheel_xoutside,35,50-rib]) 
        {
            translate([x,0,0]) 
                rotate([0,0,-plate_tilt*side])
                    cube([rib,top_plate,totz]);
        }
    
    // z ribs
    for (z=[0,25-rib,80,115]) if (z<totz) translate([-100,0,z]) cube([200,top_plate,rib]);

    // y plate
    difference() {
        union() {
            for (y=[top_plate/2,top_plate]) translate([-100,y-rib,0]) cube([200,rib,totz]);
        }
        // space to drop in crossbolts (to print in halves)
        // translate([50-15,0,25]) cube([15,top_plate*2,55]);
    }
    
    wheelrib=2*rib;
    for (side=[-1,+1])
        scale([side,1,1])
            for (x=[0,25-wheelrib-ski_wheel_xoutside-epsilon])
               translate([ski_wheel_xoutside+x,ski_wheel_axle_y+6,ski_wheel_axle_z])
                    rotate([135,0,0])
                        cube([wheelrib,15,30]);
	ski_bolts(2.5,0.0);
*/
}
module ski_minus() {
    // pegtrack_linear_and_rotated(1.0) pegtracks_2D(0.0,20.0);
    pegtrack_chamfer_tips(0.0);
    
    ski_wheel_up(ski_wheel_clearance-epsilon);
	
    ski_bolts(); // 0.0,1.0);
	
	// Cross mounting bolts
	for (side=[-1,0,+1])
		translate([(2*25.4-6)*side,6,-epsilon])
			cylinder(d=4.5,h=15+peg_linear);
	

    // Chamfer leading edge below wheel
	big=0.0;
    translate([-25,0,peg_linear-12]) 
        rotate([-25,0,0])
            translate([-big*epsilon,-big*wall,-big*epsilon])
                scale([1,-1,1])
                       cube([50+2*big*epsilon,25,50+2*big*epsilon]);
	
}

// Final overall part:
module ski() {
   // union() for (end=[-1,+1]) scale([1,1,end]) translate([0,0,-wall]) {
        difference() {
            ski_plus();
            ski_minus();
			
			/*
            // Trim back and sides
            scale([1,-1,1]) translate([-100,0,-100]) cube([200,200,400]);
           for (side=[-1,+1]) translate([side*50,0,-100]) rotate([0,0,-plate_tilt]) translate([0,-25,0]) scale([side,1,1]) cube([200,200,400]);
			*/
        }
   
	/*
        // Support material (when tilted on side
        support=0.6; // thickness of support material
        xoutside=ski_wheel_xoutside+1.0*ski_wheel_clearance;
        translate([-xoutside,ski_wheel_axle_y-16,ski_wheel_axle_z-20])
        {
            rotate([-45,0,0])
                cube([2*xoutside,support,30]);
            rotate([-26,0,0])
                cube([2*xoutside,support,45]);
        }
		*/
        
    //}
}



// Simple fast basic geometry
if (false) {
    difference() {
        pegtrack_linear_and_rotated(0.0) pegtracks_2D(wall,10.0);
        pegtrack_linear_and_rotated(1.0) pegtracks_2D(0.0,20.0);
    }
    ski_bolts();
    ski_wheel_up(0.0);
}

// As assembled:
// ski();  ski_wheel_up(0.0);

//ski_plus(); // parts
//ski_minus();

// Printable ski:
ski(); 

// Printable wheel:
translate([0,-ski_wheel_radius-5,ski_wheel_thick/2]) ski_wheel_flat(); // just the wheel


// Final part:
//rotate([0,90,0]) rotate([0,0,plate_tilt]) ski(); // tipped over, so layers run along track


/*
// Indicate top of drive sprocket
translate([0,0,370/2]) cube([10,10,10]); // clearance between drive sprockets
// Indicate top of motor housing
translate([0,50,150/2]) cube([10,10,10]); // clearance between drive sprockets
*/
