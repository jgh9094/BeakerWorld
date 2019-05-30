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
  using event_lib_t = hardware_t::event_lib_t;
  using inst_t = hardware_t::inst_t;
  using inst_lib_t = hardware_t::inst_lib_t;
  using hw_state_t = hardware_t::State;

  enum class Trait { ORG_ID };

private:
  size_t id;
  size_t surface_id;
  hardware_t brain;

  emp::Angle facing;

  double energy;
  double time_born;
  double time_dead;

  /// Values calculated from the above:
  double base_cost;   /// How much energy is spent each update?

public:
  BeakerOrg(inst_lib_t & inst_lib, event_lib_t & event_lib, emp::Ptr<emp::Random> random_ptr)
    : id(0), brain(inst_lib, event_lib, random_ptr)
    , facing(), energy(0.0), time_born(0.0), time_dead(-1.0), base_cost(0.01)
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
  size_t GetSurfaceID() const { return surface_id; }
  hardware_t & GetBrain() { return brain; }
  const hardware_t & GetBrain() const { return brain; }
  emp::Angle GetFacing() const { return facing; }
  double GetEnergy() const { return energy; }
  double GetTimeBorn() const { return time_born; }
  double GetTimeDead() const { return time_dead; }
  double GetBaseCost() const { return base_cost; }

  BeakerOrg & SetID(size_t _in) { id = _in; return *this; }
  BeakerOrg & SetSurfaceID(size_t _in) { surface_id = _in; return *this; }
  // BeakerOrg & SetBrain(const hardware_t & _in) { brain = _in; return *this; }
  BeakerOrg & SetFacing(emp::Angle _in) { facing = _in; return *this; }
  BeakerOrg & SetEnergy(double _in) { energy = _in; return *this; }
  BeakerOrg & SetTimeBorn(double _in) { time_born = _in; return *this; }
  BeakerOrg & SetTimeDead(double _in) { time_dead = _in; return *this; }
  BeakerOrg & SetBaseCost(double _in) { base_cost = _in; return *this; }

  BeakerOrg & AdjustEnergy(double _in) { energy += _in; return *this; }

  BeakerOrg & RotateDegrees(double degrees) { facing.RotateDegrees(degrees); return *this; }

  void Setup(emp::WorldPosition pos, emp::Random & random) {
    // std::cerr << "Seting up at pos " << pos.GetIndex() << std::endl;
    RotateDegrees(random.GetDouble(360.0));
  }

  void Process(size_t exe_count) {
    brain.Process(exe_count);
  };
};

#endif
