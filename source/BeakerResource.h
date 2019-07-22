/// These are resources floating around the OpenWorld.

#ifndef BEAKER_RESOURCE_H
#define BEAKER_RESOURCE_H

struct BeakerResource 
{
  private:
    size_t surface_id;     // Which surface object represents this resource?
    size_t map_id;         // Where is the resource in the resource vector?

    double x_low;          // Bounds for a specific resource
    double x_high;
    double y_low;
    double y_high;


    size_t sec_x;          // Sector for a given organism
    size_t sec_y; 

    bool init = false;

  public:
    /* Setters */
    void SetSurfaceID(const size_t _in) {surface_id = _in;}     ///< Set SurfaceID
    void SetMapID(const size_t _in) {map_id = _in;}             ///< Set MapID
    void SetSector(const size_t x, const size_t y)              ///< Set Sector coordinates
          {sec_x = x; sec_y = y;}             
    void SetBounds(const double x_l, const double x_h, const double y_l, const double y_h)
          {x_low = x_l; x_high = x_h; y_low = y_l; y_high = y_h; }
    void SetInit(const bool b) {init = b;}


    /* Getters */ 
    const size_t GetSurfaceID() const {return surface_id;}      
    const size_t GetMapID() const {return map_id;}
    const size_t GetSecX() const {return sec_x;}
    const size_t GetSecY() const {return sec_y;}
    const bool GetInit() const {return init;}
    const double GetLowerX() const {return x_low;}
    const double GetUpperX() const {return x_high;}
    const double GetLowerY() const {return y_low;}
    const double GetUpperY() const {return y_high;}

};

#endif
