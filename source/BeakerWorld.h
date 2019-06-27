/// This is the world for BeakerOrgs

#ifndef BEAKER_WORLD_H
#define BEAKER_WORLD_H

#include "Evolve/World.h"
#include "geometry/Surface.h"
#include "hardware/signalgp_utils.h"
#include "tools/math.h"

#include "config.h"
#include "BeakerResource.h"
#include "BeakerOrg.h"

class BeakerWorld : public emp::World<BeakerOrg> {
private:

  /* Renaming type names of the world, organims, and web interface.*/

  static constexpr size_t TAG_WIDTH = 16;
  using hardware_t = BeakerOrg::hardware_t;
  using program_t = hardware_t::Program;
  using prog_fun_t = hardware_t::Function;
  using prog_tag_t = hardware_t::affinity_t;
  using event_lib_t = hardware_t::event_lib_t;
  using inst_t = hardware_t::inst_t;
  using inst_lib_t = hardware_t::inst_lib_t;
  using hw_state_t = hardware_t::State;
  using surface_t = emp::Surface<BeakerOrg, BeakerResource>;
  using mutator_t = emp::SignalGPMutator<TAG_WIDTH>;

  /* Configuration specific variables */

  BeakerWorldConfig & config;                               ///< Variable that holds all experiment configurations
  std::unordered_map<size_t, emp::Ptr<BeakerOrg>> id_map;   ///< Variable that holds all surface and org world ids
  emp::vector<BeakerResource> resources;                    ///< Variable that holds all surface resources
  size_t next_id;                                           ///< Variable that holds the id placement for id_map
  size_t hm_size;                                           ///< Variable that holds the size of the heat map


  /* Hardware variables */

  inst_lib_t inst_lib;          ///< Variable that holds instruction library
  event_lib_t event_lib;        ///< Variable that holds event library
  mutator_t signalgp_mutator;   ///< Variable mutates organism genoms
  size_t HEAT = 1;              ///< Heat type location for EventDrivenGP trait vector        

  /* Web Interface variables */

  surface_t surface;                    ///< Variable that holds the surface organisms are on

  /* Statistics variables */
  
  int death_stv = 0;       ///< Variable that holds number of deaths
  int death_eat = 0;
  int death_pop = 0;

  int blue_cnt = 0;        ///< Variables that holds number of colors
  int cyan_cnt = 0;
  int lime_cnt = 0;
  int yellow_cnt = 0;
  int red_cnt = 0;
  int white_cnt = 0;

public:  
  BeakerWorld(BeakerWorldConfig & _config)
    : config(_config), id_map(), next_id(1), 
      hm_size(config.HM_SIZE()), inst_lib(), event_lib(), 
      signalgp_mutator(), surface({config.WORLD_X(), config.WORLD_Y()})
  {
    random_ptr = emp::NewPtr<emp::Random>(config.SEED());
    Config_All();
  }

  ~BeakerWorld() { id_map.clear(); random_ptr.Delete();}

  surface_t & GetSurface() { return surface; }

  /* Functions dedicated to the initilization of the run! */

  void Config_All();
  void Config_World();           ///< Function dedicated to configuring the world
  void Config_Mut();             ///< Function dedicated to configuring the mutation operator
  void Config_Inst();            ///< Function dedicated to configuring the instructions and instrucion library
  void Config_Surf();            ///< Function dedicated to configuring the surface
  void Config_OnUp();            ///< Function dedicated to configuring the OnUpdate function
  void Initial_Inject();         ///< Function dedicated to injection the initial population or organisms and resources
  size_t Calc_Heat(double r);    ///< Function dedicated to finding what heat an organism should get

  /* Functions dedicated to returning statistics! */

  int GetStv() {return death_stv;}       ///< Function dedicated to keeping track of world deaths
  int GetEat() {return death_eat;}
  int GetPop() {return death_pop;}

  int GetBlue() {return blue_cnt;}       ///< Functions dedicated to returning population distributions
  int GetCyan() {return cyan_cnt;}
  int GetLime() {return lime_cnt;}
  int GetYellow() {return yellow_cnt;}
  int GetRed() {return red_cnt;}
  int GetWhite() {return white_cnt;}

  size_t GetResSize() {return resources.size();} ///< Functions dedicated to returning container sizes
  size_t GetIDSize() {return id_map.size();}


  /* Functions dedicated to calculating statistics! */
  
  void Col_Birth(size_t h);               ///< Will keep track of organisms in the system
  void Col_Death(size_t h);               ///< Will keep track of organisms in the system

  /* Functions dedicated to the physics of the system */

  bool PairCollision(BeakerOrg & body1, BeakerOrg & body2) ///< Function dedicated to dealing with organims collisions [TODO]
  {    
    return true;
  }

  /* Functions dedicated for experiment functionality */
  double MutRad(double r, BeakerOrg & org);

};

/* Functions dedicated to the initilization of the run */

void BeakerWorld::Config_All()
{
  Config_World();
  Config_Mut();
  Config_Surf();
  Config_Inst();
  Config_OnUp();
  Initial_Inject();
}

void BeakerWorld::Config_World() ///< Function dedicated to configuring the world
{
  SetPopStruct_Grow(false); // Don't automatically delete organism when new ones are born.

  // Setup organism to share parent's surface features.
  OnOffspringReady( [this](BeakerOrg & org, size_t parent_pos)
  {
    emp::Point parent_center = surface.GetCenter(GetOrg(parent_pos).GetSurfaceID());
    double parent_radius = surface.GetRadius(GetOrg(parent_pos).GetSurfaceID());
    double off_radius = MutRad(parent_radius, org);

    size_t heat = Calc_Heat(off_radius);

    // Mutate the offspring!
    DoMutationsOrg(org);

    size_t surface_id = surface.AddBody(&org, parent_center, off_radius, heat);
    org.SetSurfaceID(surface_id);
    org.GetBrain().SetTrait(HEAT, heat);

    Col_Birth(heat);
  });

  // Make sure that we are tracking organisms by their IDs once placed.
  OnPlacement( [this](size_t pos)
  {
    size_t id = next_id++;
    GetOrg(pos).GetBrain().SetTrait((size_t)BeakerOrg::Trait::ORG_ID, id);
    id_map[id] = &GetOrg(pos);
  });

  // Trigger for an organisms death.
  OnOrgDeath( [this](size_t pos) 
  {
    surface.RemoveBody(GetOrg(pos).GetSurfaceID());
    id_map.erase(GetOrg(pos).GetID());

    Col_Death((size_t) GetOrg(pos).GetBrain().GetTrait(HEAT));
  });
}

void BeakerWorld::Config_Mut() ///< Function dedicated to configuring the mutation operator
{
  // Setup SignalGP mutations.
  signalgp_mutator.SetProgMinFuncCnt(config.PROGRAM_MIN_FUN_CNT());
  signalgp_mutator.SetProgMaxFuncCnt(config.PROGRAM_MAX_FUN_CNT());
  signalgp_mutator.SetProgMinFuncLen(config.PROGRAM_MIN_FUN_LEN());
  signalgp_mutator.SetProgMaxFuncLen(config.PROGRAM_MAX_FUN_LEN());
  signalgp_mutator.SetProgMinArgVal(config.PROGRAM_MIN_ARG_VAL());
  signalgp_mutator.SetProgMaxArgVal(config.PROGRAM_MAX_ARG_VAL());
  signalgp_mutator.SetProgMaxTotalLen(config.PROGRAM_MAX_FUN_CNT() * config.PROGRAM_MAX_FUN_LEN());

  // Setup other SignalGP functions.
  signalgp_mutator.ARG_SUB__PER_ARG(config.ARG_SUB__PER_ARG());
  signalgp_mutator.INST_SUB__PER_INST(config.INST_SUB__PER_INST());
  signalgp_mutator.INST_INS__PER_INST(config.INST_INS__PER_INST());
  signalgp_mutator.INST_DEL__PER_INST(config.INST_DEL__PER_INST());
  signalgp_mutator.SLIP__PER_FUNC(config.SLIP__PER_FUNC());
  signalgp_mutator.FUNC_DUP__PER_FUNC(config.FUNC_DUP__PER_FUNC());
  signalgp_mutator.FUNC_DEL__PER_FUNC(config.FUNC_DEL__PER_FUNC());
  signalgp_mutator.TAG_BIT_FLIP__PER_BIT(config.TAG_BIT_FLIP__PER_BIT());

  // Setup a mutation function.
  SetMutFun( [this](BeakerOrg & org, emp::Random & random)
  {
    signalgp_mutator.ApplyMutations(org.GetBrain().GetProgram(), random);
    return 1;
  });
}

void BeakerWorld::Config_Inst() ///< Function dedicated to configuring instructions and instrucion library
{
  // Setup the default instruction set.
  inst_lib.AddInst("Inc", hardware_t::Inst_Inc, 1, "Increment value in local memory Arg1");
  inst_lib.AddInst("Dec", hardware_t::Inst_Dec, 1, "Decrement value in local memory Arg1");
  inst_lib.AddInst("Not", hardware_t::Inst_Not, 1, "Logically toggle value in local memory Arg1");
  inst_lib.AddInst("Add", hardware_t::Inst_Add, 3, "Local memory: Arg3 = Arg1 + Arg2");
  inst_lib.AddInst("Sub", hardware_t::Inst_Sub, 3, "Local memory: Arg3 = Arg1 - Arg2");
  inst_lib.AddInst("Mult", hardware_t::Inst_Mult, 3, "Local memory: Arg3 = Arg1 * Arg2");
  inst_lib.AddInst("Div", hardware_t::Inst_Div, 3, "Local memory: Arg3 = Arg1 / Arg2");
  inst_lib.AddInst("Mod", hardware_t::Inst_Mod, 3, "Local memory: Arg3 = Arg1 % Arg2");
  inst_lib.AddInst("TestEqu", hardware_t::Inst_TestEqu, 3, "Local memory: Arg3 = (Arg1 == Arg2)");
  inst_lib.AddInst("TestNEqu", hardware_t::Inst_TestNEqu, 3, "Local memory: Arg3 = (Arg1 != Arg2)");
  inst_lib.AddInst("TestLess", hardware_t::Inst_TestLess, 3, "Local memory: Arg3 = (Arg1 < Arg2)");
  inst_lib.AddInst("Call", hardware_t::Inst_Call, 0, "Call function that best matches call affinity.");
  inst_lib.AddInst("Return", hardware_t::Inst_Return, 0, "Return from current function if possible.");
  inst_lib.AddInst("SetMem", hardware_t::Inst_SetMem, 2, "Local memory: Arg1 = numerical value of Arg2");
  inst_lib.AddInst("CopyMem", hardware_t::Inst_CopyMem, 2, "Local memory: Arg1 = Arg2");
  inst_lib.AddInst("SwapMem", hardware_t::Inst_SwapMem, 2, "Local memory: Swap values of Arg1 and Arg2.");
  inst_lib.AddInst("Input", hardware_t::Inst_Input, 2, "Input memory Arg1 => Local memory Arg2.");
  inst_lib.AddInst("Output", hardware_t::Inst_Output, 2, "Local memory Arg1 => Output memory Arg2.");
  inst_lib.AddInst("Commit", hardware_t::Inst_Commit, 2, "Local memory Arg1 => Shared memory Arg2.");
  inst_lib.AddInst("Pull", hardware_t::Inst_Pull, 2, "Shared memory Arg1 => Shared memory Arg2.");
  inst_lib.AddInst("Nop", hardware_t::Inst_Nop, 0, "No operation.");
  inst_lib.AddInst("Fork", hardware_t::Inst_Fork, 0, "Fork a new thread. Local memory contents of callee are loaded into forked thread's input memory.");
  inst_lib.AddInst("Terminate", hardware_t::Inst_Terminate, 0, "Kill current thread.");
  // These next five instructions are 'block'-modifying instructions: they facilitate within-function flow control. 
  // The {"block_def"} property tells the SignalGP virtual hardware that this instruction defines a new 'execution block'. 
  // The {"block_close"} property tells the SignalGP virtual hardware that this instruction exits the current 'execution block'. 
  inst_lib.AddInst("If", hardware_t::Inst_If, 1, "Local memory: If Arg1 != 0, proceed; else, skip block.", emp::ScopeType::BASIC, 0, {"block_def"});
  inst_lib.AddInst("While", hardware_t::Inst_While, 1, "Local memory: If Arg1 != 0, loop; else, skip block.", emp::ScopeType::BASIC, 0, {"block_def"});
  inst_lib.AddInst("Countdown", hardware_t::Inst_Countdown, 1, "Local memory: Countdown Arg1 to zero.", emp::ScopeType::BASIC, 0, {"block_def"});
  inst_lib.AddInst("Close", hardware_t::Inst_Close, 0, "Close current block if there is a block to close.", emp::ScopeType::BASIC, 0, {"block_close"});
  inst_lib.AddInst("Break", hardware_t::Inst_Break, 0, "Break out of current block.");

  // Setup new instructions for the instruction set.
  inst_lib.AddInst("Vroom", [this](hardware_t & hw, const inst_t & inst) 
  {
    const size_t id = (size_t) hw.GetTrait((size_t) BeakerOrg::Trait::ORG_ID);
    emp::Ptr<BeakerOrg> org_ptr = id_map[id];
    emp::Angle facing = org_ptr->GetFacing();
    surface.TranslateWrap( org_ptr->GetSurfaceID(), facing.GetPoint(1.0) );
    }, 1, "Move forward.");

  inst_lib.AddInst("SpinRight", [this](hardware_t & hw, const inst_t & inst) mutable 
  {
    const size_t id = (size_t) hw.GetTrait((size_t) BeakerOrg::Trait::ORG_ID);
    emp::Ptr<BeakerOrg> org_ptr = id_map[id];
    org_ptr->RotateDegrees(-5.0);
  }, 1, "Rotate -5 degrees.");

  inst_lib.AddInst("SpinLeft", [this](hardware_t & hw, const inst_t & inst) mutable 
  {
    const size_t id = (size_t) hw.GetTrait((size_t) BeakerOrg::Trait::ORG_ID);
    emp::Ptr<BeakerOrg> org_ptr = id_map[id];
    org_ptr->RotateDegrees(5.0);
  }, 1, "Rotate 5 degrees.");

  inst_lib.AddInst("Consume", [this](hardware_t & hw, const inst_t & inst) mutable 
  {
    const size_t id = (size_t) hw.GetTrait((size_t) BeakerOrg::Trait::ORG_ID);
    emp::Ptr<BeakerOrg> org_ptr = id_map[id];
    surface.FindOverlap( org_ptr->GetSurfaceID() );  // Surface functions automatically try to eat on overlap!
  }, 1, "Consume a resource!");
}

void BeakerWorld::Config_Surf() ///< Function dedicated to configuring the surface
{
  // Setup surface functions to allow organisms to eat.
  surface.AddOverlapFun( [this](BeakerOrg & pred, BeakerOrg & prey) 
  {
    const size_t pred_id = pred.GetID();
    const size_t prey_id = prey.GetID();

    // If preditor is bigger than the prey!
    if(pred.GetBrain().GetTrait(HEAT) > prey.GetBrain().GetTrait(HEAT))
    {
      pred.AddEnergy(prey.GetEnergy() / 4.0);
      DoDeath(prey_id);
      death_eat++;
    }

    // If prey is bigger than preditor lol!
    if(pred.GetBrain().GetTrait(HEAT) < prey.GetBrain().GetTrait(HEAT))
    {
      prey.AddEnergy(pred.GetEnergy() / 4.0);
      DoDeath(pred_id);
      death_eat++;
    }
  });
  // 
  surface.AddOverlapFun( [this](BeakerOrg & org, BeakerResource & res) 
  {
    org.AddEnergy(1.0);
    double x = random_ptr->GetDouble(config.WORLD_X());
    double y = random_ptr->GetDouble(config.WORLD_Y());
    surface.SetCenter(res.surface_id, {x,y});
  });
  surface.AddOverlapFun( [](BeakerResource &, BeakerResource &) 
  {
    ///< Leave empty lol 
  });
  surface.AddOverlapFun( [this](BeakerResource & res, BeakerOrg & org) 
  {
    org.AddEnergy(1.0);
    double x = random_ptr->GetDouble(config.WORLD_X());
    double y = random_ptr->GetDouble(config.WORLD_Y());
    surface.SetCenter(res.surface_id, {x,y});
  });
}

void BeakerWorld::Config_OnUp() ///< Function dedicated to configuring the OnUpdate function
{
  // On each update, run organisms and make sure they stay on the surface.
  OnUpdate([this](size_t)
  {
    // Process all organisms.
    Process(5);

    // Update each organism.
    for (size_t pos = 0; pos < pop.size(); pos++) 
    {
      if (pop[pos].IsNull()) continue;
      auto & org = *pop[pos];

      // Provide additional resources toward reproduction.
      org.SubEnergy(0.01);

      // If an organism has enough energy to reproduce, do so.
      if (org.GetEnergy() > 200.0 && GetNumOrgs() < config.MAX_POP_SIZE()) 
      {
        // Remove energy for building offspring; cut rest in half, so it is effectively
        // split between parent and child when copied into child.
        org.SetEnergy((org.GetEnergy() / 2.0));
        DoBirth(org, pos);
        std::cerr << "Org NewBorn! " << std::endl;
      }

      if (org.GetEnergy() <= 0.0)
      {
        death_stv++;
        DoDeath(org.GetID());
        std::cerr << "Org Died! " << std::endl;
      }
    }
  });
}

void BeakerWorld::Initial_Inject() ///< Function dedicated to injection the initial population or organisms and resources
{
  // Initialize a populaton of random organisms.
  Inject(BeakerOrg(inst_lib, event_lib, random_ptr), config.INIT_POP_SIZE());
  for (size_t i = 0; i < config.INIT_POP_SIZE(); i++) 
  {
    double x = random_ptr->GetDouble(config.WORLD_X());
    double y = random_ptr->GetDouble(config.WORLD_Y());
    BeakerOrg & org = GetOrg(i);
    double rad = random_ptr->GetDouble(config.PROGRAM_MIN_RAD_VAL(), config.PROGRAM_MAX_RAD_VAL());
    size_t heat = Calc_Heat(rad);

    if(heat < 0)
    {
      std::cerr << "Heat < 0 " << std::endl;
      exit(-1);
    }
    if(heat > hm_size)
    {
      std::cerr << "Heat > hm_size " << std::endl;
      exit(-1);
    }

    Col_Birth(heat);
    size_t surface_id = surface.AddBody(&org, {x,y}, rad, heat);
    org.SetSurfaceID(surface_id);
    org.GetBrain().SetProgram(emp::GenRandSignalGPProgram(*random_ptr, inst_lib, config.PROGRAM_MIN_FUN_CNT(),\
      config.PROGRAM_MAX_FUN_CNT(), config.PROGRAM_MIN_FUN_LEN(), config.PROGRAM_MAX_FUN_LEN(), \
      config.PROGRAM_MIN_ARG_VAL(), config.PROGRAM_MAX_ARG_VAL()));
    org.GetBrain().SetTrait(HEAT, heat);
  }
  // Add in resources.
  resources.resize(config.NUM_RESOURCE_SOURCES());
  for (BeakerResource & res : resources) 
  {
    double x = random_ptr->GetDouble(config.WORLD_X());
    double y = random_ptr->GetDouble(config.WORLD_Y());
    res.surface_id = surface.AddBody(&res, {x,y}, 3.0, config.HM_SIZE());
  }
}

size_t BeakerWorld::Calc_Heat(double r) ///< Function dedicated to injection the initial population or organisms and resources
{
  double diff = config.PROGRAM_MAX_RAD_VAL() - config.PROGRAM_MIN_RAD_VAL();
  diff = diff / (double) hm_size;
  size_t pos = 0;
  double curr = config.PROGRAM_MIN_RAD_VAL();

  while(curr <= config.PROGRAM_MAX_RAD_VAL())
  {
    curr += diff;
    if(r <= curr )
    {
      // std::cerr << "Bound: " << curr << std::endl;
      return pos;
    }
    pos++;
  }

  // std::cerr << "Bound: " << curr << std::endl;
  return hm_size - 1;
}

void BeakerWorld::Col_Birth(size_t h)
{
  if(h == 0)
  {
    blue_cnt++;
    return;
  }
  if(h == 1)
  {
    cyan_cnt++;
    return;
  }
  if(h == 2)
  {
    lime_cnt++;
    return;
  }
  if(h == 3)
  {
    yellow_cnt++;
    return;
  }
  if(h == 4)
  {
    red_cnt++;
    return;
  }
  if(h == 5)
  {
    white_cnt++;
    return;
  }
  std::cout << "Col_Birth Not Found: " << h << std::endl;
}

void BeakerWorld::Col_Death(size_t h)
{
  if(h == 0)
  {
    blue_cnt--;
    return;
  }
  if(h == 1)
  {
    cyan_cnt--;
    return;
  }
  if(h == 2)
  {
    lime_cnt--;
    return;
  }
  if(h == 3)
  {
    yellow_cnt--;
    return;
  }
  if(h == 4)
  {
    red_cnt--;
    return;
  }
  if(h == 5)
  {
    white_cnt--;
    return;
  }
  std::cout << "Col_Death Not Found: " << h << std::endl;
}

/* Functions dedicated to experiment functionality */

double BeakerWorld::MutRad(double r, BeakerOrg & org)
{
  if(random_ptr->P(config.RADIUS_MUT()))
    {
      double radius = surface.GetRadius(org.GetSurfaceID());
      double diff = random_ptr->GetRandNormal(0, 1);
      double new_r = radius + diff;

      if(new_r > config.PROGRAM_MAX_RAD_VAL())
      {
        new_r = config.PROGRAM_MAX_RAD_VAL();
      }
      if(new_r < config.PROGRAM_MIN_RAD_VAL())
      {
        new_r = config.PROGRAM_MIN_RAD_VAL();
      }
      return new_r;
    }
    return r;
}

#endif
