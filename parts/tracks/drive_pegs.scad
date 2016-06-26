include <peg_param.scad>;

clearance=0.5; // space to add around bolt and nut
bolt_R=4.9/2+clearance; // #10 bolt is 0.192"
bolthead_Z=8; // height of bolt head start (==bolt length, minus angle + track + nut thickness)
bolthead_R=10.6/2+clearance; // outside circle around hex head

module peg_main() {
rotate_extrude(convexity=6) {
    difference() {
        // main peg body
        hull() {
            peg_2D_oneside();
        }
        // space for bolt shaft
        translate([-peg_IR+bolt_R,0,0])
            square(size=[peg_IR,peg_height]);
    }
}

}

module peg_hexhole() {
    translate([0,0,bolthead_Z])
        cylinder(r=bolthead_R,h=peg_height,$fn=6);
}

module peg() {
    difference() {
        peg_main();
        peg_hexhole();
    }
}

module peg_array() {
    delta=2.5*peg_IR;
    for (dx=[0:5-1])
        translate([dx*delta,0,0])
            peg();
}

peg_array();

// Second row:
translate([1.25*peg_IR,2.0*peg_IR,0]) peg_array();
