/// These are resources floating around the OpenWorld.

#ifndef BEAKER_RESOURCE_H
#define BEAKER_RESOURCE_H

struct BeakerResource 
{
  private:
    size_t surface_id;        ///< Which surface object represents this resource?
    size_t map_id;            ///< Where is the resource in the resource vector?
    size_t exp;               ///< Counts the number of updates the resources has persisted
    // size_t exp_max;           ///< The maxiumum number of updates a resouce can persist
    bool init;                ///< Has this resource been initialized

  public:
    /* Constructor & Identifiers */

    BeakerResource() : exp(0), init(false) {;}


    /* Setters */

    void SetSurfaceID(const size_t _in) {surface_id = _in;}     ///< Set SurfaceID
    void SetMapID(const size_t _in) {map_id = _in;}             ///< Set MapID
    void SetInit(const bool b) {init = b;}                      ///< Set Initialzied to true for debugging!


    /* Getters */
    
    const size_t GetSurfaceID() const {return surface_id;}      
    const size_t GetMapID() const {return map_id;}
    const size_t GetExp() const {return exp;}
    const bool GetInit() const {return init;}


    /* Expired Fucntions */

    const bool Expired(size_t max) const {return exp == max;}
    void IncExp() {exp++;}


    /* Debugging Functions */

    void PrintStats();
};


/* Debugging Functions */

void BeakerResource::PrintStats()
{
  std::cout << "SurfaceID=" << GetSurfaceID() << std::endl;
  std::cout << "MapID=" << GetMapID() << std::endl;
  std::cout << "Init=" << GetInit() << std::endl;
}

#endif
