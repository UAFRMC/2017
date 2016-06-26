
$fa=5;
$fs=0.5;

// Leave this much clearance for printer blobs
clearance=0.5;

box=25.4;
inside=22.5-clearance;
epsilon=0.01;

difference() {
hull() {
// Mount base
translate([-box/2,0,0])
    cube(center=true,size=[3,box,box]);

// Slicing prow
for (side=[-1,+1]) {
    scale([1,1,side])
    translate([25,25,3])
    cylinder(d1=40,d2=10,h=box/2-3);
}

// Rounded top leading edge
translate([50,50,0])
    sphere(r=5);
}

// Lighten everything by peeling arc off the back side
translate([-10,75,0])
    cylinder(r=60,h=box+2*epsilon,center=true);

}


// Add mounting stem:
stemlen=20;
translate([-box/2-stemlen/2,0,0])
difference() {
    cube(center=true,size=[stemlen,inside,inside]);

    // Bolt clearance (tapped later)
    rotate([-90,0,0])
        translate([0,0,-box/2])
        cylinder(d=0.190*25.4,h=box);
    
    // Slot for 1/4" nylock
    nut_flats=0.439*25.4+clearance;
    nut_height=0.328*25.4+clearance;
    translate([-stemlen,-nut_height/2,-nut_flats/2])
        cube(size=[stemlen+nut_flats/(2*cos(30)),nut_height,nut_flats]);
}

