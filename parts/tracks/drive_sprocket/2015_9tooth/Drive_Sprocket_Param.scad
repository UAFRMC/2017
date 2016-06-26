// Parameterizable Drive Sprocket for Robot Mining
// Ryker Dial
// October 25, 2015

include <drive_param.scad>

hole_depth = 30.5;
hole_diam_outer = 32; //37;
hole_diam_inner = 23;
teeth_top_depth = 17.5; 
base_extra = 0; // Add extra height to sprocket
sprocket_height = 32+base_extra;

hole_tilt=5; // degrees that peg holes are tilted inwards

hole_Z_center=25-hole_diam_outer/2; // Z height to center of drive sprocket


module peg_hole_trapezoid(add_width,ht) {
  hull(){
    cylinder(h=ht, d1= hole_diam_inner, d2=hole_diam_outer+add_width);
    translate([hole_depth, 0, 0])
        cylinder(h=ht, d1= hole_diam_inner, d2=hole_diam_outer+add_width);
  }
}


module sprocket(){
    sprocket_circum = hole_num*hole_period;
    sprocket_radius = sprocket_circum/(2*PI);
    difference(){
   // minkowski(){
    difference(){
        cylinder(r=sprocket_radius, h=sprocket_height);
        // Remove center material
        translate([0,0,10+epsilon]){
            cylinder(r1=sprocket_radius-hole_depth+10*epsilon, r2=sprocket_radius-teeth_top_depth, h=sprocket_height-10);
        }
        // Peg holes
        translate([0,0,hole_Z_center]){
            for(i=[0:hole_num-1]){
                translate([0,0,hole_diam_outer/2]){
                    rotate([0,270-hole_tilt,i*360/hole_num]){
                        translate([0,0,sprocket_radius-hole_depth]){
                            peg_hole_trapezoid(0,hole_depth+3);
                            translate([0,0,hole_depth-7])
                              rotate([0,hole_tilt,0])
                                peg_hole_trapezoid(10,10);
                        }
                    }
                }
            }
        }
    }
   // sphere(1);
//}
        // Center hole
        translate([0,0,-(epsilon+1)]){
            cylinder(d=center_hole_diam+wiggle, h=sprocket_height);
        }
        // Hole for bearing
        translate([0, 0, -(epsilon+1)]){
            cylinder(d=bearing_outer_diam+wiggle, h=bearing_width+wiggle+1);
        }

        // Mounting Holes
        for(i=[0:mount_hole_num-1]){
            rotate([0,0,i*360/mount_hole_num]){
                translate([0, mount_screw_dis, -(epsilon+1)]){
                    cylinder(d=mount_screw_hole+wiggle, h=sprocket_height);
                }
            }
        }
 //   }
    }
}

sprocket();
