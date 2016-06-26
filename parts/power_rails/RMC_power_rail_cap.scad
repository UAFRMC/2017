// RMC Power Rail Insulator

$fn = 20;
inch=25.4; // mm per inch

// Variables
pwrW = 0.42*inch; // + rail (little top rail)
pwrH = 0.52*inch;
gndW = 0.9*inch; // - rail (big lower rail)
gndH = 0.6*inch;

btsH = 0.9*inch; // BTS motor controller height (rail-rail clearance)

hole = 2.5; // M3 thread diameter (can be tapped or clearance drilled)
boltHead = 7.5; // M3 cap screw head diameter + clearance

thick = 12; // Z thickness
wall=10;  // XY padding around sides
eps = 0.01;
capWall = -eps; 

difference(){
// Block
translate([0,0,-capWall-eps])
linear_extrude(height = thick+capWall, convexity=4){
  difference(){
    translate([-2*wall,0])
    square([7/2*wall+gndW,2*wall+btsH+pwrH+gndH]);
    
    translate([-2*wall, wall+gndH])
    square([wall,btsH+wall+pwrH]);
    
    translate([gndW,-eps])
    rotate([0,0,atan((gndW-pwrW)/(pwrH/2+btsH))])
    translate([2*wall,-wall])
    square([thick*3,3*thick+pwrH+gndH+btsH+2*eps]);
  }
}
union(){
//Holes
linear_extrude(height = thick+eps){
  translate([0,wall+gndH+btsH])
  square([pwrW,pwrH]);
  translate([0,wall])
  square([gndW, gndH]);
}

// PWR screw
rotate([0,90,0])
translate([-thick/2,wall+gndH+btsH+pwrH/2,-thick]){
cylinder(d=hole,h=2*wall+pwrW);
translate([0,0,-wall])
cylinder(d=boltHead,h=wall*3/2);  
}

// GND screw
rotate([0,90,0])
translate([-thick/2,wall+gndH/2,-thick]){
cylinder(d=hole,h=wall+gndW);
  translate([0,0,-wall-eps*2])
cylinder(d=boltHead,h=wall*3/2);
}

// Base screws
rotate([-90,0,0])
translate([gndW+wall,-(thick-capWall)/2,]){
  translate([0,0,thick])
cylinder(d=boltHead,h=wall*5);
cylinder(d=hole,h=wall+gndH);
}
rotate([-90,0,0])
translate([-3*wall/2,-(thick-capWall)/2,]){
  translate([0,0,thick])
cylinder(d=boltHead,h=wall*3/2);
cylinder(d=hole,h=wall+gndH);
}
}

}
