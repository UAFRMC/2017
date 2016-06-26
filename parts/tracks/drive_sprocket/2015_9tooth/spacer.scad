// Spacer between two drive sprockets

include <drive_param.scad>

plate_depth = 11; // thickness of plate

mount_hole_num=4;

center_hole_diam = 3/8*25.4; // 9.525;// 3/8 inch
bearing_outer_diam = 7/8*25.4; // 22.225; // 7/8 inch
bearing_width = 9/32*25.4; // 7.14375; // 9/32 inch
mount_screw_hole = 0.25*25.4; // 6.35; // 1/4 inch

epsilon = 0.1;

wiggle= 0.3; // wiggle room for holes

$fs=0.2; // flatness tolerance (mm)
$fa=5; // angle tolerange (deg)


module spacer_plate(){
    sprocket_circum = hole_num*hole_period;
    sprocket_radius = sprocket_circum/(2*PI);
    thru_hole=plate_depth+bearing_width+2;
    mount_screw_dia=5.0;
    difference() {
        union() {
            // Main body
            cylinder(r=sprocket_radius, h=plate_depth);

            // Fits into bearing hole
            translate([0,0,plate_depth-epsilon])
                cylinder(d=bearing_outer_diam-wiggle,h=bearing_width-wiggle);
        }
        
        // Center thru hole
        translate([0,0,-epsilon]){
            cylinder(d=center_hole_diam+wiggle, h=thru_hole);
        }

        // Mounting Holes
        for(i=[0:mount_hole_num-1]){
            rotate([0,0,i*360/mount_hole_num]){
                translate([0, mount_screw_dis, -epsilon]){
                    cylinder(d=mount_screw_dia+wiggle, h=thru_hole);
                }
            }
        }
        
        // Lightening / dust scooping holes
        for(i=[0:hole_num-1])
            rotate([0,0,i*360/hole_num])
                translate([58,0,-epsilon])
                    scale([1,0.9,1])
                     cylinder(d=35, h=thru_hole, $fn=7);
        
    }
}

spacer_plate();
