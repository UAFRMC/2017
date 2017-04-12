/*
  Gridnav library: simple test driver program.
*/
#include "gridnav.h"

int main() 
{
  typedef gridnav::gridnavigator<38,74, 64, 10, 15> navigator_t;
  navigator_t navigator;
  typedef navigator_t::fposition fposition;
  
  fposition start(200,500,0);
  fposition target(190,50,90);
  
  navigator_t::planner plan(navigator,start,target,true);
  for (const fposition &p : plan.path)
    std::cout<<"Plan position: "<<p<<"\n";
  
  return 0;
}

