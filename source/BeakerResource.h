/// These are resources floating around the OpenWorld.

#ifndef BEAKER_RESOURCE_H
#define BEAKER_RESOURCE_H

struct BeakerResource 
{
  private:
    size_t surface_id;     // Which surface object represents this resource?
    size_t map_id;         // Where is the resource in the resource vector?

  public:
    /* Setters */
    void SetSurfaceID(const size_t _in) {surface_id = _in;}     ///< Set SurfaceID
    void SetMapID(const size_t _in) {map_id = _in;}             ///< Set SurfaceID

    /* Getters */ 
    const size_t GetSurfaceID() const {return surface_id;}      
    const size_t GetMapID() const {return map_id;}
};

#endif
