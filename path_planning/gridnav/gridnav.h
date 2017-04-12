/*
Simple grid-based robot navigation package:

The grid has 3 dimensions:
  2D XY location in a fixed-size field.
  Z axis angular rotation, with wraparound.

The path planner walks through this grid in a lazy fashion
using the A* algorithm to expand the next closest node to the target.

Aurora Robotics 2017
*/
#ifndef AURORA_GRIDNAV_H
#define AURORA_GRIDNAV_H

#include <iostream>
#include <deque>
#include <queue>
#include <math.h>
#include "osl/vec2.h"

namespace gridnav {

/**
 Create a navigation framework for a space with:
    GRIDX by GRIDY 2D XY grid cells, of size GRIDSIZE centimeters each.
    GRIDA angular orientations representing angles from [0,360).
    The robot is approximately ROBOTGRID cells across.
*/
template <int GRIDX,int GRIDY,int GRIDA,int GRIDSIZE,int ROBOTGRID>
class gridnavigator {
public:
  typedef gridnavigator<GRIDX,GRIDY,GRIDA,GRIDSIZE,ROBOTGRID> navigator_t;

  // forward declarations
  class fposition;

  // This is a robot's position in the grid (integer coordinates)
  class gridposition {
  public:
    int x; // 0 .. GRIDX-1
    int y; // 0 .. GRIDY-1
    int a; // 0 .. GRIDA-1
    
    // Return true if this is a valid (in bounds) position.
    bool valid() const {
      if (x<0 || x>=GRIDX) return false;
      if (y<0 || y>=GRIDY) return false;
      if (a<0 || a>=GRIDA) return false;
      return true;
    }
    
    gridposition(const fposition &p) {
      x=(int)floor(p.v.x*(1.0/GRIDSIZE)+0.5);
      y=(int)floor(p.v.y*(1.0/GRIDSIZE)+0.5);
      a=(int)floor(p.a+0.5);
    }
    
    bool operator==(const gridposition &p) const { 
      return x==p.x && y==p.y && a==p.a;
    }
  };
  
  // fmod has a bug for negative numbers.
  // This version always returns a value on [0,mod)
  static double fmodplus(double v,double mod) {
    double vm=fmod(v,mod);
    if (vm<0.0) vm+=mod;
    return vm;
  }
  
  // This is a robot's floating-point position in space (float coordinates)
  class fposition {
  public:
    vec2 v; // centimeters
    float a; // angle: 0 .. GRIDA
    
    // Return our angle, in degrees, from 0-360.   
    //    0 degrees is facing in the +X direction
    //    90 degrees is facing in the +Y direction
    float get_degrees(void) const { return a*(360.0/GRIDA); }

    // Set our angle, from degrees.
    void set_degrees(float deg) {
      deg=fmodplus(deg,360.0);  // wrap to 0..360
      a=deg*(GRIDA/360.0);
    }
    
    fposition() :v(-999.9,-999.9), a(-999.9) {}    

    // Library-internal interface: vector and GRIDA angle
    fposition(const vec2 &v_,float a_) :v(v_), a(a_) {  }

    // External interface: x,y, and degrees
    fposition(float x,float y,float deg_) :v(x,y) { set_degrees(deg_); }
    
    friend std::ostream &operator<<(std::ostream &o,const fposition &p) 
    {
      o.precision(3);
      o<<" {xy "<<p.v.x<<" "<<p.v.y<<"  ang "<<p.get_degrees()<<" } ";
      return o;
    }
  };

  // This is a 2D data grid for one slice.
  template <class T>
  class grid2D {
    T data[GRIDY][GRIDX];
  public:
    void clear(const T& clearvalue) {
      for (int y=0;y<GRIDY;y++)
      for (int x=0;x<GRIDX;x++)
        data[y][x]=clearvalue;
    }
    
    // Accessors, without bounds check
    T &at(int x,int y) { return data[y][x]; }
    const T &at(int x,int y) const { return data[y][x]; }
  };
  
  // This represents everything we know about one slice of our domain
  class gridslice {
  public:
    // This is our robot orientation angle in degrees
    float ang_deg;

    // This is our robot orientation angle in radians
    float ang_rad;

    // In this slice, the robot can drive in this direction
    //   This is a unit vector = vec2(cos(ang_rad),sin(ang_rad))
    vec2 drive;
    
    // Nonzero bits here mark persistent obstacles
    grid2D<unsigned char> obstacle;
    
    // Nonzero bits here mark that we've visited this cell
    grid2D<unsigned char> visit;
  };
  
  // The whole grid is just a list of slices, indexed by angle.
  gridslice slice[GRIDA];
  
  // Set up our slices, assuming no obstacles and a point robot
  gridnavigator() {
    for (int ia=0;ia<GRIDA;ia++) {
      gridslice &s=slice[ia];
      float ang=ia*(2.0*M_PI/GRIDA);
      s.ang_deg=ia*(360.0/GRIDA);
      s.ang_rad=ang;
      s.drive=vec2(cos(ang),sin(ang));
      s.obstacle.clear(0);
      s.visit.clear(0);
    }
  }
  
  // FIXME: mark obstacles based on robot geometry (somehow)
  

  // Convert discrete angles to grid cell equivalent.
  //   This scale factor is derived figuring the tip velocity of a turning robot.
  static constexpr double TURN_COST_TO_GRID_COST=((2.0*M_PI/GRIDA)*(ROBOTGRID*0.5));

  // This class represents an intermediate position in a search.
  class searchposition {
  public:
    double cost; // cost to get here
    fposition p; // position of the robot at this point
    const fposition &target; // target of search (for A* cost)
    const searchposition *last; // preceding search position, or NULL if none.
    
    searchposition(double cost_,const fposition &p_,const fposition &target_,const searchposition *last_)
      :cost(cost_), p(p_), target(target_), last(last_)
    {}
    
    // Return the approximate cost to reach the target
    double total_cost(void) const {
      double drive_dist=length(p.v-target.v); // in grid cells
      double turn_ang=fabs(p.a-target.a); // in discrete angle units
      return cost + drive_dist + turn_ang*TURN_COST_TO_GRID_COST;
    }
  };
  
  // Keeps a reference to a searchposition.
  //   Needed because std::priority_queue<searchposition &> fails for reasons that are not clear.
  class searchposition_ref {
  public:
    const searchposition *r;
    searchposition_ref(const searchposition &r_) :r(&r_) {}
    
    // Sort so we always grab the best search position first
    bool operator<(const searchposition_ref &p) const {
      return r->total_cost()>p.r->total_cost();
    }
  };
  
  // Build the sequence of steps needed to move the robot from origin to target.
  class planner {
    navigator_t &nav;
    
    // This is our search target configuration
    fposition target;
    
    // If true, print debug info to the console
    bool verbose;
    
    // This is the active list of searched positions, sorted by distance to target.
    std::priority_queue<searchposition_ref> search;
    
    // SUBTLE: to allow backtracking, we need to persistently store search positions.
    //  std::deque has the nice property of not invalidating references.
    std::deque<searchposition> pool; // safely stores our search positions
    void add_search(double cost,const fposition &origin,const searchposition *last) {
      pool.emplace_back(cost,origin,target,last);
      search.push(pool.back());
    }
    
  public:
    // This bool marks if we found a path
    bool valid;
    // This is the sequence of steps from origin to target
    std::deque<fposition> path;
    
    planner(navigator_t &nav_,const fposition &origin_,const fposition &target_,bool verbose_) 
      :nav(nav_), target(target_), verbose(verbose_)
    {
      // Clear all previous visit marks
      for (int ia=0;ia<GRIDA;ia++) {
        nav.slice[ia].visit.clear(0);
      }
      
      // Start search at origin
      add_search(0.0,origin_,0);
    
      // Repeatedly visit positions with the cheapest net cost
      while (!search.empty()) {
        const searchposition &cur=*(search.top().r); search.pop();
        
        if (verbose) std::cout<<"At "<<cur.p<<" cost "<<cur.cost<<": ";
        
        // Mark this cell as visited
        gridposition icur(cur.p);
        if (!icur.valid()) {
          if (verbose) std::cout<<"out of bounds, skip it\n";
          continue; // out of bounds, don't bother
        }
        gridslice &s=nav.slice[icur.a];
        unsigned char &vis=s.visit.at(icur.x,icur.y);
        if (vis==1) {
          if (verbose) std::cout<<"already visited\n";
          continue; // already visited here
        }
        vis=1; // mark as visited
        
        // Check for obstacles
        const unsigned char &obs=s.obstacle.at(icur.x,icur.y);
        if (obs!=0) {
          if (verbose) std::cout<<"blocked by obstacle\n";
          continue; // can't go this way, obstacle found
        }
        if (verbose) std::cout<<"pushing children\n";
        
        // Check if we're done:
        if (gridposition(cur.p)==gridposition(target)) 
        { // we're done!  Follow the chain of "last" pointers back to the origin.
          if (verbose) std::cout<<"Target reached!  Reverse path:\n";
          for (const searchposition *p=&cur;p!=NULL;p=p->last) {
            path.push_front(p->p);
            std::cout<<p->p<<"\n";
          }
          valid=true;
          return;
        }
        
        // Check all nearby cells
        for (float drive=-1.0;drive<=+1.0;drive+=2.0) {
          double distance=GRIDSIZE; // grid cell distance to drive per step
          double cost=cur.cost+distance;
          fposition next(cur.p.v+distance*drive*s.drive,cur.p.a);
          add_search(cost,next,&cur);
        }
        for (float turn=-1.0;turn<=+1.0;turn+=2.0) {
          double cost=cur.cost+GRIDSIZE*TURN_COST_TO_GRID_COST;
          fposition next(cur.p.v,fmodplus(cur.p.a+turn,GRIDA)); // angles wrap around
          add_search(cost,next,&cur);
        }
      }
      
      // If we get here, we ran out of options before reaching the target.
      if (verbose) std::cout<<"Out of search options!\n";
      valid=false;
    } // end planner constructor
  }; // end planner class

};

};


#endif





