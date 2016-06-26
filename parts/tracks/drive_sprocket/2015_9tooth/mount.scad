include <drive_param.scad>

drive_mount_ht=0.5*25.4+32; // total height, including gearbox drive pegs
mount_screw_r=1.6; // wood screw radius

outside_radius=mount_screw_dis*1.3; // overall body


module barbie_gearbox(wiggle)
{
    height=16.5;
    radius=16;
    inside_radius=25.2/2-wiggle.x;
    cube_width=13;
    cube_diameter=50;
    difference() {
        // Main body
        translate([0,0,-height/2])
        {
            cylinder(height+wiggle.z,radius+wiggle.x,radius+wiggle.x,center=true);
            cube([cube_width,cube_diameter,height]+wiggle,center=true);
            cube([cube_diameter,cube_width,height]+wiggle,center=true);
        }
        // Central hole
      //  translate([0,0,-height])
      //      cylinder(r=inside_radius,h=height);
    }
}


module drive_mount()
{
    thru_hole=drive_mount_ht+2*epsilon;
    taper=outside_radius-28; // slope in to gearbox
    
    difference() {
        union() {
            // Main body
            cylinder(r=outside_radius, h=drive_mount_ht-taper);
            translate([0,0,drive_mount_ht-taper])
                cylinder(r1=outside_radius, r2=outside_radius-taper, h=taper);
        }
        
        // Center thru hole
        translate([0,0,-epsilon]){
            cylinder(d=center_hole_diam+wiggle, h=thru_hole);
        }
        
        // Gearbox drive splines
        rotate([0,0,45])
            translate([0,0,thru_hole])
                barbie_gearbox([0.8,0.8,0.8]);
        
        // Inside bearing hole
        translate([0, 0, thru_hole-16-bearing_width-epsilon]){
            cylinder(d=bearing_outer_diam+wiggle, h=bearing_width+wiggle+1);
        }


        // Mounting Holes
        for(i=[0:mount_hole_num-1]){
            rotate([0,0,i*360/mount_hole_num]){
                translate([0, mount_screw_dis, -epsilon])
                union() {
                    cylinder(r=mount_screw_r, h=thru_hole);
                    cylinder(r1=1.7*mount_screw_r,r2=mount_screw_r, h=3);
                }
            }
        }
        
    }
}

drive_mount();
