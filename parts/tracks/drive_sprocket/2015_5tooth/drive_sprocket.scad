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

module barbie_adapter(bolt_r=5,height=40,ceil=0,floor=4,peg_or=14,peg_ir=10,peg_num=4,peg_h=20,gearbox_offset=0)
{
    epsilon=0.001;
    circ=peg_num*50;
    radius=circ/(2*PI);
    real_height=height+ceil+floor;
    
    difference()
    {
        difference()
        {
            difference()
            {
                cylinder(real_height,radius,radius,center=true);

                translate([0,0,-real_height/2+peg_or+floor])
                    for(ii=[1:peg_num])
                        rotate(ii*360/peg_num,[0,0,1])
                            translate([0,radius-peg_h/2,0])
                                rotate(90,[1,0,0])
                                    cylinder(peg_h,peg_or,peg_ir,center=true);
            }

            translate([0,0,real_height/2])
                rotate(gearbox_offset,[0,0,1])
                    barbie_gearbox([1.5,1.5,1.5]);
        }

        cylinder(height+epsilon,bolt_r,bolt_r,center=true);
    }
}

barbie_adapter(height=1,peg_num=4,gearbox_offset=0,ceil=30,floor=20);