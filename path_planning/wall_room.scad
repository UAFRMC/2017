


// Robot 2D outline (mirrored on both axes?)
module robot_2D() {
    square(size=[150,75],center=true);
}

// Robot's 3D configuration space outline
module robot_3D() {
    linear_extrude(height=360,twist=360,convexity=10,slices=45) robot_2D();
}

// Outline of room
module room_2D() {
    difference() {
        translate([-378/2,0,0])
            square(size=[378,738]);
        // projecting hole in room 
        //translate([0,300])
        //    square(size=[500,200]);
    }
}

// Difference between environment and room
module room_inside() {
    difference() {
        translate([-300,-100,0])
            square(size=[600,1000]);
        room_2D();
    }
}

// Disallowed region for robot center point
module robot_disallowed() {
    minkowski() {
        robot_3D();
        linear_extrude(height=1)
            room_inside();
    }
}

// Allowed region for robot center point
module robot_allowed() {
    difference() {
        linear_extrude(height=360,convexity=2) room_2D();
        robot_disallowed();
    }
}

robot_allowed();