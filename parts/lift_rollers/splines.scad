/**
  Drive splines for dust ejection rollers.
*/

$fa=3;
$fs=0.1;

// Inside holes shrink this much diameter on this printer
printer_fattening=0.5;

// Outside diameter of pipe, plus clearance
pipe_OD=48.5+printer_fattening;

// cylinder(d=pipe_OD,h=200);

// Thickness of sturdy parts
wall=1.6;

// Length of spline drive area
spline_Z=15; // 25;
// Length of spline interface area
spline_slopeZ=8;

// Range of drive teeth projections
spline_minht=wall;
spline_maxht=6+spline_minht;

// Outside diameter of drive splines
spline_OD=pipe_OD+2*spline_maxht;
spline_ID=pipe_OD+2*spline_minht;

// Number of drive splines
spline_count=12;

// Scale factor for spline tips: 1.0 is ID to OD (sharp tips), 2.0 has flat tops
spline_tips=1.3;


// Roller
roller_clearance=0.6+printer_fattening; // mm between moving parts
roller_OD=spline_OD+2*wall+2*roller_clearance;
roller_Z=spline_Z+spline_slopeZ+wall; // overall height
roller_fillet=5; // radius around inside walls
roller_rim=2*roller_fillet;



epsilon=0.01;


// Rotate one object per spline
module spline_teeth(skip=1) {
    for (ang=[0:360*skip/spline_count:360-epsilon]) 
        rotate([0,0,ang]) 
            children();
}

// 2D outline of spline interface surface
module spline_2D(clearance) {
    offset(r=clearance) {
        union() {
            //circle(d=spline_ID);
            da=360/spline_count/2; // tooth half-angle
            spline_teeth() {
                del=(spline_OD-spline_ID)/4; // total radius of spline tooth
                c=cos(da+epsilon);
                s=sin(da+epsilon);
                spline_cen=(spline_OD+spline_ID)/4; // average radius
                r=spline_cen-del*spline_tips; // inside radius
                polygon([
                  [r*c,r*s],
                  [spline_cen+del*spline_tips,0],
                  [r*c,-r*s]
                ]);
                  
                // Hacky triangle to connect to center:
                r2=r+epsilon;
                polygon([
                  [r2*c,r2*s],
                  [0,0],
                  [r2*c,-r2*s]
                ]);
            }
        }
    }
}

// 3D shape of spline interface object
//   clearance is extra space around outside (grows diameter)
//   topscale determines if tips slope in or out
module spline_3D(clearance,topscale) {
    intersection() {
        cylinder(d=spline_OD+2*clearance,h=spline_Z+spline_slopeZ);
        union() {
            cylinder(d=spline_ID+2*clearance,h=spline_Z+spline_slopeZ);
            
            linear_extrude(height=spline_Z,convexity=10) {
                spline_2D(clearance);
            }
            translate([0,0,spline_Z-epsilon])
            linear_extrude(height=spline_slopeZ,scale=topscale,convexity=10) {
                spline_2D(clearance);
            }
        }
    }
}

// Mounting bolt: M3 bolt
module M3_bolt() {
    head_len=8;
    rotate([0,-90,0]) {
        cylinder(d=3.2+printer_fattening,h=20);
        translate([0,0,-head_len+epsilon])
            cylinder(d=6.2+printer_fattening,h=head_len);
    }
}

// Drive spline, around pipe:
module spline_driver() {
    difference() {
        union() {
            translate([0,0,wall-epsilon])
                spline_3D(0.0,0.8); // teeth
            cylinder(d=spline_OD,h=wall); // base wall
        }
        
        // Clearance for central pipe
        translate([0,0,-epsilon])
            cylinder(d=pipe_OD+epsilon,h=spline_Z+spline_slopeZ+wall+2*epsilon);
        
        // Clearance for mounting bolts
        spline_teeth(2)
            translate([spline_OD/2-6,0,(spline_Z+spline_slopeZ)/2])
            {
                M3_bolt();
            }
    }
}


// Spool:
module spline_spool() {
    difference() {
        // roller body
        cylinder(
            r1=roller_OD/2+1.0*roller_rim,
            r2=roller_OD/2+0.6*roller_rim,
            h=roller_Z);
        
        // Hole for pipe
        translate([0,0,-epsilon])
            cylinder(d=pipe_OD,h=roller_Z+2*epsilon);
        
        // middle channel for cord storage
        rotate_extrude(angle=360) {
            hull() {
                translate([roller_OD/2+roller_fillet,roller_fillet+wall])
                    circle(r=roller_fillet);
                translate([roller_OD/2+roller_fillet,roller_Z-roller_fillet-wall])
                    circle(r=roller_fillet);
                translate([roller_OD*2,roller_Z*0.5-2*wall])
                    circle(r=roller_Z*0.5);
            }
        }
        
        // holes for tying off cord
        spline_teeth()
            rotate([0,0,360/(2*spline_count)])
            translate([roller_OD/2+2,0,-epsilon])
                   // rotate([0,-90,0])
                        cylinder(d=3.2+printer_fattening,h=roller_Z/2+2*epsilon);
        
    
        // clearance for splines in middle
        translate([0,0,roller_Z-(spline_Z+spline_slopeZ)+2*epsilon])
            spline_3D(roller_clearance,1.0/0.8);
    }
}

// section view:
module roller_section() {
    difference() {
        union() {
            translate([0,0,roller_Z+1.5*wall])
            rotate([0,180,0])
                spline_driver();
            spline_spool();
        }
        translate([0,0,-epsilon])
            cube([roller_OD,roller_OD,roller_Z/2]);
    }
}

// printable side-by-side version
module roller_printable() {
    union() {
        spline_spool();
        translate([roller_OD/2+roller_rim*2+spline_OD/2,0,0]) spline_driver();
    }
}

//roller_section();
roller_printable();


