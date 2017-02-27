#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include "libfreenect.h"

#include <pthread.h>

#include "osl/vec4.h"

#include <math.h>

#include "ros/ros.h"

#define VEC3_TO_XYZ(v) (v).x,(v).y,(v).z

vec3 upVec; // accelerometer-derived "up vector" estimate, kinect coords

/*
  To use less CPU time, we decimate the depth output from the Kinect
  by this factor.  This is needed because the Kinect output 
  resolution seems to be fixed.
*/
enum {decimate=4};
enum {KINECT_w=640/decimate};
enum {KINECT_h=480/decimate};

//Video output size
enum {KINECT_vw=640};
enum {KINECT_vh=480};

pthread_t freenect_thread;
volatile int die = 0;

// back: owned by libfreenect (implicit for depth)
// mid: owned by callbacks, "latest frame ready"
// front: owned by GL, "currently being drawn"
uint8_t *depth_mid, *depth_front;

freenect_context *f_ctx;
freenect_device *f_dev;
int freenect_angle = 0;
int freenect_led;


/***************** The GOOD STUFF ****************/

enum {KINECT_bad=0x7fe};

/**
  This class is used to get 3D positions from kinect depths.
  
  Coordinate system:
    Origin is Kinect's IR receiver
	+X faces to the left (from Kinect's point of view)
    +Y is up
	+Z is away from Kinect
*/
class kinect_depth_image {
public:
	const uint16_t *depthi;
	enum {NATIVE_KINECT_w=640};
	int w,h; /* dimensions of image */
	/* Unit-depth field of view offset per X or Y pixel */
	float pixelFOV;
	
	kinect_depth_image(const uint16_t *d_,int w_,int h_) 
		:depthi(d_), w(w_), h(h_) 
	{
		pixelFOV=tan(0.5 * (M_PI / 180.0) * 57.8)/(NATIVE_KINECT_w*0.5)*decimate;
	}

	/* Return raw data number at this pixel */
	int raw(int x,int y) const {
		return depthi[y*decimate*NATIVE_KINECT_w+x*decimate];
	}
	
	/* Return depth, in meters, at this pixel */
	float depth(int x,int y) const {
		uint16_t disp=depthi[y*decimate*NATIVE_KINECT_w+x*decimate];
		if (disp>KINECT_bad) return 0.0;
		return disp_to_depth(disp);
	}
	
	/* Given stereo disparity number, return depth in meters */
	float disp_to_depth(uint16_t disp) const {
		//From Stephane Magnenat's depth-to-distance conversion function:
		return 0.1236 * tan(disp / 2842.5 + 1.1863) - 0.037; // (meters)
	}
	
	/* Return 3D direction pointing from the sensor out through this pixel 
	   (not a unit vector, due to Kinect's projection) */
	vec3 dir(int x,int y) const {
		return vec3((w*0.5-x)*pixelFOV,(h*0.5-y)*pixelFOV,1.0);
	}
	
	/* Return 3D location, in meters, at this pixel */
	vec3 loc(int x,int y) const {
		// Project view ray out for that pixel
		return dir(x,y)*depth(x,y);
	}
};

/**
 This class classifies pixels as matching our target, or not.
*/
class kinectPixelWatcher {
public:
	kinect_depth_image &img;
	vec3 up;
	kinectPixelWatcher(kinect_depth_image &img_,vec3 up_) 
		:img(img_), up(normalize(up_)) 
	{}
	
	// Debug outputs for this pixel.
	class debug_t {
	public:
		unsigned char r,g,b; // onscreen color
		vec3 N; // unit surface normal (Kinect coords, estimated)
		vec3 P; // position (Kinect coords)
	};
	
	// Classify this pixel: 0 for bad, small number for close, >=10 for match.
	int classify_pixel(int x,int y,debug_t &debug) const;
};

int kinectPixelWatcher::classify_pixel(int x,int y,debug_t &debug) const
{

	const float min_up=-0.8; // meters along up vector to start search (below ground)
	const float max_up=0.1; // meters along up vector to end search (too high)
	
	const float max_distance=9.0; // meters to farthest possible target
	const float min_distance=0.5; // meters to closest possible target
	
	const float normal_Y_max=0.90; // surface normal Y component (above here is floor)
	
	const float top_shiftY=-0.7; // meters to look on top
	const float top_shiftZ=0.3; // meters shift to demand (clear space behind top)
	const float bot_shiftY=+0.6; // meters to look below
	const float bot_shiftZ=-0.2; // meters shift to demand
	
	/* The bump threshold (small==smoother).  Measured in data numbers, not depth.
	   Not only is this faster (fewer conversions), it automatically distance-adapts. */
	const int bump_thresh=15;
	
	const int delx=9/decimate,dely=9/decimate; // pixel shifts for neighbor search	
	
	int raw=img.raw(x,y);
	if (raw>=KINECT_bad) return 0; // out of bounds

	vec3 loc=img.loc(x,y); // meters
	debug.P=loc;
	float toFloor=loc.dot(up); // m to floor
	float m=loc.mag(); // m range
	debug.b=0; // m*256; // Range, in meters
	debug.r=toFloor*10.0*256; // Up, in 10cm wrap

	if ( m<min_distance || m>max_distance) return 1; // too close or far

	//if (toFloor<min_up || toFloor>max_up) return 2; // bad up vector distance

	/* This pixel passes the "up" Z-range filter. Check neighbors. */
	if (!(x-delx>=0 && y-dely>=0 && x+delx<img.w && y+dely<img.h)) return 3; // neighbors out of bounds

	// Check normal--easy way to get rid of floor
	vec3 N=normalize( // take cross product of nearby diagonals
	(img.loc(x+delx,y-dely)-
	 img.loc(x-delx,y+dely))
	.cross(
	(img.loc(x-delx,y-dely)-
	 img.loc(x+delx,y+dely))
	));
	debug.N=N;

	if (dot(N,up)>normal_Y_max) {
		debug.b=255;
		return 4; // that's the floor (pointing upward)
	}

	return 3;
}


void depth_cb(freenect_device *dev, void *v_depth, uint32_t timestamp)
{
	int i;
	uint16_t *depth = (uint16_t*)v_depth;

	kinect_depth_image img(depth,KINECT_w,KINECT_h);
	vec3 up(upVec.x,upVec.y,upVec.z); 
	// Kinect accelerometer calibration fixes (possibly device dependent)
	//  I picked these to make the floor be *flat*
	//up.x+=0.03; // correct a tilt to the left
	//up.z+=0.02; // correct weak back tilt
	up=normalize(up);
	kinectPixelWatcher watch(img,up);
	

// #pragma omp parallel for schedule(dynamic,2) // <- very fast, but breaks region grower.
	for (int y=0;y<img.h;y+=decimate)
	for (int x=0;x<img.w;x+=decimate)
	{
		int i=x+img.w*y;

		kinectPixelWatcher::debug_t debug;
		debug.r=debug.b=0;
		watch.classify_pixel(x,y,debug);

		depth_mid[3*i+0]=debug.r;
		depth_mid[3*i+1]=debug.g;
		depth_mid[3*i+2]=debug.b;
	}
}

/*********************** Back to verbatim libfreenect/examples/glview.c code ****************/

void *freenect_threadfunc(void *arg)
{
	int accelCount = 0;

	freenect_set_led(f_dev,LED_RED);
	freenect_set_depth_callback(f_dev, depth_cb);
	freenect_set_depth_mode(f_dev, freenect_find_depth_mode(FREENECT_RESOLUTION_MEDIUM, FREENECT_DEPTH_11BIT));

	freenect_start_depth(f_dev);

	while (!die && freenect_process_events(f_ctx) >= 0) {
		//Throttle the text output
		if (accelCount++ >= 10)
		{
			accelCount = 0;
			freenect_raw_tilt_state* state;
			freenect_update_tilt_state(f_dev);
			state = freenect_get_tilt_state(f_dev);
			double dx,dy,dz; // OSL: made global
			freenect_get_mks_accel(state, &dx, &dy, &dz);
			upVec=normalize(upVec+0.2*normalize(vec3(dx,dy,dz)));
			
			fflush(stdout);
		}
		usleep(1000);
	}

	printf("\nshutting down streams...\n");

	freenect_stop_depth(f_dev);

	freenect_close_device(f_dev);
	freenect_shutdown(f_ctx);

	printf("-- done!\n");
	return NULL;
}

int main(int argc, char **argv)
{
	int res;

	depth_mid = (uint8_t*)malloc(KINECT_w*KINECT_h*3);
	depth_front = (uint8_t*)malloc(KINECT_w*KINECT_h*3);

	printf("Kinect camera test\n");

	if (freenect_init(&f_ctx, NULL) < 0) {
		printf("freenect_init() failed\n");
		return 1;
	}

	freenect_set_log_level(f_ctx, FREENECT_LOG_DEBUG);
	freenect_select_subdevices(f_ctx, (freenect_device_flags)(FREENECT_DEVICE_MOTOR | FREENECT_DEVICE_CAMERA));

	int nr_devices = freenect_num_devices (f_ctx);
	printf ("Number of devices found: %d\n", nr_devices);

	int user_device_number = 0;
	if (argc > 1)
		user_device_number = atoi(argv[1]);

	if (nr_devices < 1)
		return 1;

	if (freenect_open_device(f_ctx, &f_dev, user_device_number) < 0) {
		printf("Could not open device\n");
		return 1;
	}

	res = pthread_create(&freenect_thread, NULL, freenect_threadfunc, NULL);
	if (res) {
		printf("pthread_create failed\n");
		return 1;
	}

	// Need some ROS loop here to process occupancy grid and publish results
	ros::init(argc, argv, "kinect_grid_node");
	ros::NodeHandle n;

	return 0;
}