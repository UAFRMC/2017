/*
  Gridnav library: simple test driver program.
*/
#include "gridnav.h"

class rmc_robot_geometry : public gridnav::robot_geometry {
public:
    //    (0,0) is the robot's origin, at the center of turning.
    //    +x is the direction the robot naturally drives forward
  virtual int clearance_height(float x,float y) const 
  {
  // front-back:
    if (x>40.0 || x<-35.0) return gridnav::OPEN;
    if (y<0) y=-y; // apply Y symmetry
    if (y>75.0) return gridnav::OPEN; // beyond the tracks
    if (y>45.0) return 0; // tracks
    return 20; // open area under mining head
  }
};

int main() 
{
  // RMC-plausible navigation grid dimensions:
  typedef gridnav::gridnavigator<38,74, 64, 10, 15> navigator_t;
  navigator_t navigator;
  
  // Discretize the robot geometry
  rmc_robot_geometry geo;
  navigator_t::robot_grid_geometry robot(geo,true);
  
  // Mark impassable edges
  navigator.mark_edges(robot);
  // Make some obstacles on left side
  for (int x=0;x<10;x++)
  navigator.mark_obstacle(x,30,40,robot);
  // Make a straddleable obstacle in middle
  for (int x=20;x<23;x++)
  navigator.mark_obstacle(x,30,30,robot);
  
  // Plan a path
  typedef navigator_t::fposition fposition;
  
  fposition start(70,600,20);
  fposition target(190,40,90);
  
  navigator_t::planner plan(navigator,start,target,true);
  for (const fposition &p : plan.path)
    std::cout<<"Plan position: "<<p<<"\n";
  
  navigator.lastpath.print();
  
  return 0;
}

