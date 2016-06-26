$fa=4;

module barbie_gearbox(wiggle)
{
    height=16.5;
    radius=16;
    cube_width=13;
    cube_diameter=50;
    translate([0,0,-height/2])
    {
        cylinder(height+wiggle.z,radius+wiggle.x,radius+wiggle.x,center=true);
        cube([cube_width,cube_diameter,height]+wiggle,center=true);
        cube([cube_diameter,cube_width,height]+wiggle,center=true);
    }
}


// Drive pegs to match tracks
//    radius is outside of cylinder
module barbie_holes(radius,peg_or=14,peg_ir=10,peg_num=4,peg_h=20)
{
    for(ii=[1:peg_num])
        rotate(ii*360/peg_num,[0,0,1])
            translate([0,radius-peg_h/2,0])
                rotate(90,[1,0,0])
                    cylinder(peg_h,peg_or,peg_ir,center=true);
}

module barbie_adapter(bolt_r=5,height=40,ceil=0,floor=4,peg_or=14,peg_ir=10,peg_num=4,peg_h=20,gearbox_offset=0)
{
    epsilon=0.001;
    circ=peg_num*50;
    radius=circ/(2*PI);
    real_height=height+ceil+floor;
    
    start_Z=30; // Z for first drive hole
    delta_Z=50; // distance between drive hole centers
    drive_Z=16; // sink this far into gearbox
    
    difference()
    {
        // Outside
        cylinder(real_height,radius,radius);

        // Drive pegs
        translate([0,0,start_Z])
            barbie_holes(radius,peg_or,peg_ir,peg_num,peg_h);
        translate([0,0,start_Z+delta_Z])
            barbie_holes(radius,peg_or,peg_ir,peg_num,peg_h);
        
        // Lightening / dust escape holes
        for(ii=[1:peg_num])
            rotate(ii*360/peg_num,[0,0,1])
                translate([0,radius*0.6,1.3*drive_Z])
                    cylinder(height,r=radius*0.25);
    
        // Gearbox drive hole
        translate([0,0,drive_Z])
            rotate(gearbox_offset,[0,0,1])
                barbie_gearbox([0.8,0.8,0.8]);

        // Axle hole:
        cylinder(3*height+epsilon,bolt_r,bolt_r,center=true);
    }
}

// Full:
rotate([180,0,0]) // print with drive pegs on top
    barbie_adapter(floor=0,height=85,peg_num=5,gearbox_offset=45,ceil=15);


height=50;
r1=30;
r2=400;
epsilon=1;

/*
// Chamfering test
difference()
{
    difference()
    {
        cylinder(height,r1+r2,r1+r2,center=true);
        cylinder(height+epsilon,r2,r2,center=true);
    }

    translate([0,0,-r1])
    {
        rotate_extrude(convexity=10)
        {
            translate([r2-epsilon,-epsilon,-epsilon])
            {
                intersection()
                {
                    circle(r=r1+epsilon);
                    square(r1+epsilon);
                }
            }
        }
    }
}
*/
