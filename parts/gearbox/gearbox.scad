/**
 Peg Perego replacement gearbox


Shaft sizes:

A has 12mm shaft hole, 25mm outside (ribbed)

    A-B outside: 67.2
    B-C C-D outside: 45.5
    D-M outside: 39.9
        (M shaft: 3.2)

B-D have 8mm shaft hole
*/

// $fa=40; $fs=1.5; // fast coarse preview
//  $fa=10; $fs=0.1; // good clean version
  $fa=5; $fs=0.1; // precise final version


// Add ability to mount motor + encoder on either side:
mount_left_side=true;
mount_right_side=false;
encoder_hole=true;

// Maximum size of extruder blobs
clearance=0.3;
// Avoids roundoff
epsilon=0.01;

// Gear shafts for B-D are this identical length
shaft_length=48; // 25.4*2.0;

// Output axle hole diameter
axle_hole_dia=0.375*25.4+2*clearance;

// Output axle post height
axle_post_Z=30.0;


// Thin support walls
support=0.7; 

// General sidewall thickness
wall=1.7;

// General top/bottom thickness
wallz=wall;

// Thickness underneath main output gear
wallzout=wall;

// General reinforcing rib thickness
rib=1.1;

// Extra clearance diameter around gears (in X-Y direction)
gear_cleard=3.0+clearance;
// Extra clearance distance around top of gears in Z direction
gear_clearz=0.4+clearance;


Ashaft=25.0; // A gear's shaft diameter (as measured, do not change!)
BDshaft=8.0; // B-D shaft diameter (as measured)


/*
    gears array field allocation:
    [0] OD, [1] ID, [2] shaft, [3] shaft Z start, [4] low height, [5] high height
*/
gear_OD=0; // index of outside (big gear) clearance diameter
gear_ID=1; // index of inner gear clearance diameter
gear_shaft=2; // index of shaft size
gear_shaft_Z=3; // index of shaft Z start location
gear_LZ=4; // index of height of big (low) side of gear
gear_HZ=5; // index of height of total (high) side of gear
gear_topboss=6; // z inset on top of gear around shaft

gears=[ // values measured with calipers
    [ 76.2, 53.5, axle_hole_dia, 0, 18.0, 26.0, 0.0], // [0] A final output gear
    [ 56.5, 34.1, BDshaft, -0.5, 13.5, 31.5, 3.9 ], // [1] B
    [ 58.5, 25.0, BDshaft, 1, 11.0, 24.5, 0.7 ], // [2] C
    [ 61.5, 21.1, BDshaft, 8.0, 12, 23.2, 2.4 ], // [3] D
    [ 38.3-1.5, 12.9 /* 9.5*/ , 0.0, 0.0, 0.0, 100.0, 0.0], // [4] motor is weird
];
tot_gears=5;

// Distances between axle centers of each gear:
between_gears=[
    67.2 - Ashaft/2 - BDshaft/2, // A to B
    45.5 - BDshaft/2 - BDshaft/2, // B to C
    45.5 - BDshaft/2 - BDshaft/2, // C to D
    39.9 - BDshaft/2 - 3.2/2 - 0.5, // D to M
];

// M3 capscrew
module M3_capscrew(dia_shaft=3.3,len_shaft=30,len_head=30,big=0) {
    cylinder(d=dia_shaft+2*big,h=len_shaft);
    translate([0,0,-len_head+big+epsilon])
        cylinder(d=8+big,h=len_head);
}


// Translate to the center of this gear number.
//   Uses funky recursion because OpenSCAD doesn't have true iteration.
module gear_center(target_num,cur_num=0) {
    if (cur_num>=target_num) {
        children();
    }
    else {
        if (cur_num<tot_gears-1) {
            translate([between_gears[cur_num],0,-gears[cur_num+1][gear_LZ]])
                gear_center(target_num,cur_num+1) children();
        } else { // for motor, we're already there.
            gear_center(target_num,cur_num+1)  children();
        }
    }
}

// Single gear, including shafts
module gear_single(num,big=0,longer_below=0,longer_above=0) {
    bottom_d=gears[num][gear_OD]+gear_cleard+2*big;
    lht=gears[num][gear_LZ]+gear_clearz+longer_above;
    hht=gears[num][gear_HZ]+gear_clearz+longer_above;
    
    // bottom gear teeth (big)
    if (longer_below==0 && num==3) { 
        // Need special boss around D gear bottom
        difference() {
            cylinder(d=bottom_d, h=lht);
            translate([0,0,-epsilon])
                cylinder(d=14, h=2);
        }
    } else { // normal gear
        translate([0,0,-longer_below])
        cylinder(d=bottom_d, h=lht+longer_below);
    }

    // top gear teeth (small)
    translate([0,0,lht-epsilon])
     difference() {
        cylinder(d=gears[num][gear_ID]+gear_cleard+2*big, h=hht-lht);
        topboss=gears[num][gear_topboss];
        if (topboss>0) {
            translate([0,0,hht-lht-topboss+epsilon])
                cylinder(d=BDshaft+2*2.25, h=topboss);
        }
     }

    // mounting shaft
    z=-10+gears[num][gear_shaft_Z];
    translate([0,0,z])
     cylinder(d=gears[num][gear_shaft]+clearance+2*big, h=shaft_length+gear_clearz);
}


// Z start of motor face, above D gear bottom
motor_start_Z=20;

// Geartrain, starting from output gear and recursing down to motor:
module gear_train(num=0,big=0,longer_below=0,longer_above=0) {
    if (num<tot_gears-1) {
        gear_single(num,big,longer_below,longer_above);
        
        translate([between_gears[num],0,-gears[num+1][gear_LZ]])
           gear_train(num+1,big,longer_below,longer_above);
    }
    else { // motor at end of train
        // motor gearhead
        translate([0,0,-wall/2])
            cylinder(d=gears[num][gear_ID]+clearance+2*big, h=gears[num][gear_HZ]);
        
        // motor body
        translate([0,0,motor_start_Z-longer_below])
            cylinder(d=gears[num][gear_OD]+gear_cleard+2*big, h=70+longer_below);
        
        // motor mounting screws (18mm M3)
        for (side=[-1,+1]) 
            translate([0,25/2*side,motor_start_Z-12])
                M3_capscrew(big=big);
    }
}

module rib_star(nrib=4,maxR=400,maxZ=400) {
    for (angle=[0:180/(nrib/2):180]) {
        rotate([0,0,angle+360/(2*nrib)])
            cube(size=[maxR,rib,maxZ],center=true);
    }
}
module rib_cyl(D,wall=rib) {
    difference() {
        cylinder(d=D+2*wall,h=200,center=true);
        cylinder(d=D,h=200,center=true);
    }   
}

module gear_train_rib_star(shaft,ID,OD,nrib) {
    rib_star(nrib=nrib,maxR=3*OD);
    if (shaft>0) rib_cyl(shaft,wall=wall);
    //if (ID>0) rib_cyl(ID,wall=rib);
   // rib_cyl(OD,wall=rib);
}

// Geartrain, with mounting ribs
module gear_train_ribs(num=0) {
    if (num<tot_gears-1) {
       // nribs=[12,6,2,2,4]; // (bottom) rib count for each gear
        nribs=[6,4,2,2,4]; // (top) rib count for each gear
        
        gear_train_rib_star(
            shaft=gears[num][gear_shaft],
            ID=gears[num][gear_ID]+gear_cleard,
            OD=gears[num][gear_OD]+gear_cleard,
            nrib=nribs[num]);

        if (num==0) { // extra stiffness around output gear
            rib_cyl(gears[num][gear_ID]+gear_cleard);
            rib_cyl(gears[num][gear_OD]+gear_cleard);
        }

        translate([between_gears[num],0,-gears[num+1][gear_LZ]])
           gear_train_ribs(num+1);
    }
    else { // motor at end of train
        gear_train_rib_star(shaft=0,
            ID=gears[num][gear_ID]+clearance,
            OD=gears[num][gear_OD]+gear_cleard,
            nrib=4);
        
        // motor mounting screws
        for (side=[-1,+1]) 
            translate([0,25/2*side,motor_start_Z-12])
                gear_train_rib_star(shaft=0, // 3.3+clearance,
                    ID=0,OD=8, nrib=4);
    }
}

// gear_train(); // ,longer_above=100);


// Overall outside dimensions
box_y=25.4*3.27; // symmetric in Y
box_lx=0; // X low
box_hx=125; // X high
box_lz=-38-wallz; // Z low
box_hz=gears[0][gear_HZ]+gear_clearz-1; // Z high
//translate([box_lx,-box_y/2,box_lz])
//    cube(size=[box_hx-box_lx,box_y,box_hz-box_lz]);

// Space between top and bottom half of gearbox
opening_clearance=0.3+clearance;

// Two wall thicknesses, for where inside and outside overlap.
box_2walls=wall+opening_clearance+wall;

// Outer box's 2D XY cross section
module box_2D() {
    offset(r=+20) offset(r=-20) // smooth all edges
    hull() {
        // output gear
        circle(d=gears[0][gear_OD]+gear_cleard+2*wall);
        
        // main body box
        translate([box_lx,-box_y/2])
            square(size=[box_hx-box_lx,box_y]);
        
        // motor
        gear_center(4)
            circle(d=gears[4][gear_OD]+gear_cleard+2*wall);
    }
}

// Back of box is tilted at this angle, to save material.
box_back_angle=15;

// Box back trimming in X-Z plane
module box_trim() {
    translate([0,0,-12])
        rotate([0,box_back_angle,0])
            translate([0,0,-100])
            cube(size=[400,400,200],center=true);
    
}


// 3D overall box
module box_3D(inset=0) {
    difference() {
        // Main box body
        translate([0,0,box_lz])
            linear_extrude(height=box_hz-box_lz)
                offset(r=inset)
                    box_2D();
        
        // Bottom trim
        box_trim();
    }
}


// Splitting surface between top and bottom pieces
module box_split_surface(bottom_bigger=0.0) {
    big=10;
    rotate([90,0,0])
        linear_extrude(height=box_y+2*big,center=true,convexity=4) {
            offset(r=bottom_bigger) {
                polygon([
                    [200,box_lz-big],
                    [200,-28],
                    [130,-28], // D gear
                    [10,10],
                    [-200,10],
                    [-200,box_lz-big],
                ]);
            }
        }
}

// Side length of mounting frame square tubing:
frame=1.030*25.4+clearance;

// Centerline of top row of frame
frame_topZ=frame/2-wallzout;

// Square tubing frame
module frame_parts(big=0) {
    // Show frame as-mounted
    color([0.5,0.5,0.5]) {
        // Example mounting frame pieces
        translate([0,0,-frame/2-wallzout])
            cube([frame+2*big,400,frame+2*big],center=true);

        for (side=[-1,+1])
            translate([0,side*(box_y/2+frame/2),frame_topZ])
                rotate([0,0,90])
                    cube([frame+2*big,400,frame+2*big],center=true);
    }
}

// Shift and deploy centers of each frame mounting bolt
module frame_bolt_centers() {
    boltX=1.5*25.4;
    translate([0,0,frame_topZ])
    if (mount_left_side)
        translate([boltX,box_y/2+wall/2,0])  children();
    if (mount_right_side)
        scale([1,-1,1])
        translate([boltX,box_y/2+wall/2,0])  children();
}

// Space for frame mounting bolt & nut trap
module frame_bolts(big=0) {
    // Thru hole for 1/4" bolt
    bolt_dia=0.255*25.4+2*clearance+2*big;
    
    // Slot for 1/4" nylock nut
    nut_flats=0.445*25.4+2*clearance+2*big;
    nut_height=0.345*25.4+2*clearance+2*big;
    
    // Middle of trapped nut is this distance inside 
    nut_Y_deep=-0.5*25.4;
    
    frame_bolt_centers() {
        rotate([90,0,0])
            cylinder(d=bolt_dia,h=40,center=true);
        
        translate([0,nut_Y_deep-big])
        {
            // nut itself (use nophead's hack of 6 sided cylinder)
            rotate([-90,30,0])
                cylinder(r=nut_flats/(2*cos(30)),h=nut_height,$fn=6);
            
            // chute to insert nut from inside
            scale([1,1,-1]) // extend downward
                translate([-nut_flats/2,0])
                    cube(size=[nut_flats,nut_height,75]);
            
        }        
    }
}


module box_screw(dia_shaft,big=0) {
    len=32; // length of screws (M3x18mm)
    M3_capscrew(dia_shaft=3.5,big=big,len_shaft=len);
}

// Screws connecting top and bottom halves
module box_screws(dia_shaft) {
    for (side=[-1,+1]) {
        //side=[-1,+1];
        // two screws on motor side
        translate([101,33*side,box_lz+10])
            children();
        
        // two screws near output side
       // translate([28,35*side,-5])
          //  children();
    }
}

// Bottom half of box, solid (no ribs)
module box_bottom_solid() {
  union() {
    difference() {
        // Bottom = whole box intersected with split
        intersection() {
            box_3D(-(wall+opening_clearance));
            box_split_surface(-opening_clearance);
        }
        
        // Space for frame
        frame_parts();
        
        // Space for gears
        gear_train(longer_above=100);
        
        // Space for top-bottom screws
        box_screws() box_screw(dia_shaft=3.5);
    }
    difference() {
        // Main post for output shaft.  Low $fn is to make ribs
        translate([0,0,-epsilon])
        cylinder(d=25.0-clearance,h=axle_post_Z,$fn=12);
       
        // Axle hole
        translate([0,0,-2*epsilon])
            cylinder(d=axle_hole_dia,h=axle_post_Z+3*epsilon);
    }
  }
}

// Support material under bottom of box
module box_bottom_supports() {
    difference() {
        intersection() {
            union() {
                // Main stripes
                for (ystripe=[-box_y/2-wall-opening_clearance:8:+box_y/2])
                    rotate([0,box_back_angle,0])
                    translate([+50,ystripe,-5])
                        cube([300,support,20],center=true);
                
                // Add sidewall supports
                translate([0,0,-70])
                    linear_extrude(height=70)
                    difference() {
                        offset(r=-wall-opening_clearance)
                            box_2D();
                        offset(r=-wall-opening_clearance-support)
                            box_2D();
                    }
            }
            // Trim down to only under main body
            translate([0,0,-100])
                linear_extrude(height=200)
                    offset(r=-wall-opening_clearance)
                        box_2D();
        }
        
        // don't go below floor
        box_trim();
        
        // remove where supports hit main body
        translate([0,0,-0.1]) // spacing for easy removal
        difference() {
            box_3D();
            frame_parts(big=-0.1);
        }
    }
}

// Box bottom, with ribs
module box_bottom_ribs() {
  union() {
    intersection() {
        box_bottom_solid();
        
        union() 
        { // All possible ribs:
            
            // Main side walls
            difference() {
                cube(size=[400,400,400],center=true);
                translate([0,0,wall])
                    box_3D(-(wall+opening_clearance+wall));
            }
            
            // Centerline
            cube(size=[400,rib,400],center=true);
            
            // Frame opening
            frame_parts(big=wall);
            
            gear_train_ribs(num=0);
            
            box_screws() union() {
                rib_star(maxR=100);
                box_screw(dia_shaft=3.5, big=wall);
            }
            
            rib_cyl(D=25.0-clearance-rib,$fn=12);
        }
    }
    
    // Add support stripes under main body
	box_bottom_supports();
	
	// Increase first-layer support thickness, to keep slicer from omitting it
	for (shift=[-1,+1])
		translate([0,shift*support*0.9,0])
			intersection() {
				box_bottom_supports();
				translate([0,0,support]) box_trim();
			}
  }
}

// Block for mounting motor bolts
motor_rebar_x=18;
motor_rebar_z=15;

// Slot in rebar block for airflow
motor_airflow_y=16;

// Top half of box, solid (no ribs)
module box_top_solid() {
    difference() {
        // Whole body
        box_3D();
        
        // Clearance for bottom half
        translate([0,0,-2*epsilon])
        intersection() {
            box_3D(-wall);
            box_split_surface(0);
        }
        
        // Space for frame
        frame_parts();
        
        // Space for gears
        gear_train(); // ,longer_below=100);
        
        // Airflow for motor
        translate([160,0,-20]) 
            cube(size=[50,motor_airflow_y,11],center=true);
        
        // Space for top-bottom screws (tapped in)
        box_screws() box_screw(dia_shaft=2.6);
        
        // Space for frame mounting bolts
        difference() {
            frame_bolts();
            /*
            // Add very thin membrane across holes, to keep dust out of unused holes
            for (side=[-1,+1])
                translate([(box_hx+box_lx)/2,side*(box_y/2-wall/2),frame/2])
                    cube(size=[box_hx,0.5,frame],center=true);
            */
        }
    }
}



// RJ45 connector for magnetic encoder board, on D gear
module rj45() {
    rj_width = 19;
    rj_length = 32;
    rj_height = 17; // rj45 connector itself
    circuit_height=3.5; // height of circuit board

    board_x = 125+8+2;
    board_z = 33.5 - rj_height - 8.76 - circuit_height;

    translate([-board_x, 12.5, board_z]){
        // Circuit board
        translate([0, 0, rj_height-circuit_height]){
            cube(size=[rj_width, rj_length, circuit_height]);
        }
        
        // Actual RJ45 plug
        translate([0, rj_length - 15.5, 0]){
            cube(size=[rj_width, 15.5, rj_height]);
        }
        
        // Clearance above ribs
        translate([0, 0, rj_height]){
            cube(size=[rj_width, rj_length-10, 30]);
        }
    }
}


module box_top_ribs() {
    union() {
    difference() {
    intersection() {
        box_top_solid();
        
        union() 
        { // union of all possible rib shapes
            // Main side walls
            difference() {
                cube(size=[400,400,400],center=true);
                translate([0,0,-wall])
                    box_3D(-wall);
            }
            
            // Centerline
            cube(size=[400,rib,400],center=true);
            
            gear_train_ribs(num=0);
            
            box_screws() union() {
                rib_star(maxR=40);
                box_screw(dia_shaft=2.7, big=wall);
            }
            
            // Reinforcing around motor mount
            translate([160,0,-22])
                cube(size=[motor_rebar_x,80,motor_rebar_z],center=true);
            
            // Space for frame mounting bolts
            intersection() {
                translate([0,0,50])
                    cube(size=[400,400,100],center=true);
                union() {
                    frame_bolts(big=wall);
                    // Ribs for each big bolt
                    frame_bolt_centers() {
                        translate([0,-10,0])
                        rib_star(nrib=2,maxR=90);
                    }
                }
            }
        }
    }

        // Subtract off RJ45 connector holes, left and right
    if (encoder_hole)
      rotate([0,180,0]) {
        if (mount_left_side) scale([1,1,1]) rj45();
        if (mount_right_side) scale([1,-1,1]) rj45();
      }
  }
  
  
    // Add support material under motor
    support_ht=45; // Z height of motor support material
        // Support for motor
        translate([160,0,box_hz-support_ht/2]) 
            difference() {
                cube(size=[motor_rebar_x-2*support,motor_airflow_y+2*support,support_ht],center=true);
                translate([0,0,-support])
                cube(size=[motor_rebar_x-2*support,motor_airflow_y,support_ht],center=true);
            }
  }
}

// All internal parts (for debug, not printing)
module box_internal_parts() {
    frame_parts();
    gear_train();
    //box_screws();
    frame_bolts();
}

//box_internal_parts();

module box_bottom_backtilt() {
    translate([-100,100,-14.42]) rotate([0,-box_back_angle,0])
        children();
}

if (0) { // solids (good for tuning geometry)
    box_bottom_backtilt()
        box_bottom_solid();

    rotate([0,180,0])
        box_top_solid();
}
else { // full ribbed versions (slow!)
    if (0) {
      box_bottom_backtilt()
          box_bottom_ribs();
    }

    if (1) {
      rotate([0,180,0])
          box_top_ribs();
    }
}
