/**
  Rock-blocking sides for our 3" ABS scoops
*/
$fs=0.1;
$fa=3;

OD=89;
ID=77;

edgeZ=0.8; // 2 layers, for gluing
floorZ=1.3; // sidewall
ribZ=4; // height of reinforcing ribs




// Demo rock
// translate([ID/2-20/2,0]) circle(d=20);

/*
  Revolve a 3D ridge in the shape of a circle.
  The tip of the ridge has diameter dia.
  The ridge sticks up and out by distance edge.
*/
module diamond_circle(dia,edge) {
	rotate_extrude() {
			translate([dia/2,0,0])
				polygon([[-edge,0],[0,+edge],[+edge,0]]);
	}
}

wall_center=[-15,-20];
wall_OD=70;
wall_edge=ribZ;

module scoop_outline(height=floorZ,extra_inside=0.0) {
	linear_extrude(height=height) intersection() {
		circle(d=OD);
		translate(wall_center) circle(d=wall_OD+extra_inside*2);
	}
}

// Region denoting where we swap from one ridge to the other
module scoop_swap() 
{
	rotate([0,0,53])
		translate([-150-17.2,0]) cylinder(d=300,h=20);
}

for (flip=[0,1])
translate([flip?-OD+10:0,0,0])
scale([flip?-1:1,1,1])
rotate([0,0,35])
intersection() {
	union() { // solid part
		intersection() {  // outside rib
			diamond_circle(ID,ribZ); 
			scoop_swap(); 
		}

		difference() { // inside rib
			translate(wall_center) 
				diamond_circle(wall_OD-wall_edge,wall_edge);
			scoop_swap();
		}
		
		// Main sidewalls
		scoop_outline(floorZ,0.0);
		
		// Reinforcing at tips
		translate([-ID/2,0]) rotate([0,0,-30]) scale([0.8,1,1]) cylinder(d1=4*ribZ,d2=0.0,h=2*ribZ);
	}
	
	// Trim outside of scoops
	difference() {
		scoop_outline(10,ribZ);
		
		// Trim down to be glued onto 3" ABS pipe
		intersection() {
			translate([0,0,edgeZ])
			linear_extrude(height=100,convexity=4) intersection() {
				difference() {
					circle(d=OD+10); circle(d=ID);
				}
				rotate([0,0,0])
				translate([0,-100]) square([200,200],center=true);
			}
		}
	}


}