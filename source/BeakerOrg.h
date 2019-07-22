/// These are OpenWorld organisms.

#ifndef BEAKER_ORG_H
#define BEAKER_ORG_H

#include "geometry/Point2D.h"
#include "hardware/EventDrivenGP.h"

class BeakerOrg {
public:
  static constexpr size_t TAG_WIDTH = 16;
  static constexpr size_t HW_MAX_THREADS = 16;     // Max execution threads/'cores' active at once.
  static constexpr size_t HW_MAX_CALL_DEPTH = 128; // Max active calls at once.
  static constexpr double HW_MIN_SIM_THRESH = 0.0; // Min similarity threshold for match. 

  using hardware_t = emp::EventDrivenGP_AW<TAG_WIDTH>;
  using program_t = hardware_t::program_t;
  using event_lib_t = hardware_t::event_lib_t;
  using inst_t = hardware_t::inst_t;
  using inst_lib_t = hardware_t::inst_lib_t;
  using hw_state_t = hardware_t::State;

  enum class Trait {HEAT_ID, MAP_ID, WRL_ID};

public:
  size_t id;                        ///< Organism personal ID
  size_t surface_id;                ///< Organism surface ID
  size_t map_id;                    ///< Oraganism map id
  size_t wrl_id;                    ///< Organism world id
  double radius;                     ///< Organism radius

  hardware_t brain;                 ///< Underlying represet
  emp::Angle facing;                ///< Direction the organism if facing!
  double energy;                    ///< Amount of energy the organims has
  size_t heat_id;                      ///< Stores heat_id of the organism

public:
  BeakerOrg(inst_lib_t & inst_lib, event_lib_t & event_lib, emp::Ptr<emp::Random> random_ptr)
    : id(0), brain(inst_lib, event_lib, random_ptr), facing(), energy(1000.0)
  {
    brain.SetMinBindThresh(HW_MIN_SIM_THRESH);
    brain.SetMaxCores(HW_MAX_THREADS);
    brain.SetMaxCallDepth(HW_MAX_CALL_DEPTH);  
  }
  BeakerOrg(const BeakerOrg &) = default;


  BeakerOrg(BeakerOrg &&) = default;
  ~BeakerOrg() { ; }

  BeakerOrg & operator=(const BeakerOrg &) = default;
  BeakerOrg & operator=(BeakerOrg &&) = default;

  size_t GetID() const { return id; }
  size_t GetSurfaceID() { return surface_id; }
  size_t GetWorldID() { return wrl_id; }
  size_t GetRadius() { return radius; }
  size_t GetMapID() {return map_id;}
  hardware_t & GetBrain() { return brain; }
  const hardware_t & GetBrain() const { return brain; }
  emp::Angle GetFacing() const { return facing; }
  double GetEnergy() const { return energy; }
  size_t GetHeatID() const { return heat_id; }


  ///< Set the ID of the organism!
  BeakerOrg & SetID(size_t _in) { id = _in; return *this; }
  ///< Set the Surface ID 
  BeakerOrg & SetSurfaceID(size_t _in) { surface_id = _in; return *this; }
  ///< Set the World ID 
  BeakerOrg & SetWorldID(size_t _in) { wrl_id = _in; return *this; }
  ///< Set the radius
  BeakerOrg & SetRadius(double _in) { radius = _in; return *this;}
  ///< Set the Map ID 
  BeakerOrg & SetMapID(size_t _in) { map_id = _in; return *this; }
  ///< Set the World ID 
  BeakerOrg & SetHeatID(size_t _in) { heat_id = _in; return *this; }
  ///< Set the direction the organims is facing!
  BeakerOrg & SetFacing(emp::Angle _in) { facing = _in; return *this; }
  ///< Set the energy variable!
  BeakerOrg & SetEnergy(double _in) { energy = _in; return *this; }



  ///< Subtract Energy to the organism and return this organism!
  BeakerOrg & SubEnergy(double _in) { energy -= _in; return *this;}
  ///< Rotate the direction that organism is facing!
  BeakerOrg & RotateDegrees(double degrees) { facing.RotateDegrees(degrees); return *this; }
  ///< Add Energy to the organism and return this organism!
  BeakerOrg & AddEnergy(double _in, double cap) 
  {
    (energy + _in > cap) ? energy = cap : energy += _in;
    return *this;
  }

  void Setup(emp::WorldPosition pos, emp::Random & random) 
  {
    RotateDegrees(random.GetDouble(360.0));     
  }

  void Process(size_t exe_count) 
  {
    brain.Process(exe_count);
  };

  double GetTrait(size_t pos)
  {
    return brain.GetTrait(pos);
  }

  void SetTrait(size_t id, double val)
  {
    brain.SetTrait(id, val);
  }

  void Load(std::istream & input)
  {
    brain.Load(input);
  }

  void PushInst(const std::string & name)
  {
    brain.PushInst(name);
  }
};

#endif
