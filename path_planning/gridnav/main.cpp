/*
  Gridnav library: simple test driver program.
*/
#include "gridnav.h"

int main() 
{
  float gridscale=0.1; // meters per grid cell
  typedef gridnav::gridnavigator<38,74, 64, 15> navigator_t;
  navigator_t navigator;
  typedef navigator_t::fposition fposition;
  
  fposition start(20,50,50);
  fposition target(19,10,16);
  
  std::deque<fposition> plan=navigator.plan(start,target,true);
  for (const fposition &p : plan)
    std::cout<<"Plan position: "<<p<<"\n";
  
  return 0;
}

