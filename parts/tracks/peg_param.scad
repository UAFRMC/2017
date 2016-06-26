/*
  Track drive stud parameters.
*/
$fa=5;
$fs=0.2;

peg_IR=12; // overall peg size
peg_height=16; // overall peg height
peg_tilt=0.5; // sides sloped inward this far
peg_CR=6; // peg rounded top radius

module peg_2D_oneside() {
    translate([peg_IR-peg_tilt-peg_CR,peg_height-peg_CR,0])
        circle(r=peg_CR);
    polygon(points=[
        [0,0],
        [peg_IR,0],
        [0,peg_height]
    ]);
}
