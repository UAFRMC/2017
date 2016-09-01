/*
 50mm spacing track grouser, with UAF logo.
*/

$fa=4; $fs=0.1; // for smooth final result

centerline=25;

module sidez() {
for (side=[-1,+1])
translate([side*centerline,0,0])
children();
}

module lozenge(OD,ht,slope=1.0) {

hull()
sidez()
{
	cylinder(d1=OD,d2=OD-2*ht*slope,h=ht);
}

}

panheadz=3.5;
panheadd=12;
overallz=panheadz+2;
difference() {
	lozenge(33,overallz);
	
	// Inside cavity for bolt heads
	translate([0,0,overallz+0.1]) scale([1,1,-1])
		lozenge(panheadd+2*panheadz,panheadz);
	
	
	// Thru holes for bolts:
	translate([0,0,-0.1])
	sidez() cylinder(d=5.7,h=overallz);
}


linear_extrude(height=overallz-0.8,convexity=10)
	scale([1,1.4,1])
	text("UAF",font="Arial Black",size=13,halign="center",valign="center");

