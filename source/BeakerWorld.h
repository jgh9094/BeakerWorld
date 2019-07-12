/// This is the world for BeakerOrgs

#ifndef BEAKER_WORLD_H
#define BEAKER_WORLD_H

///< Includes from Empirical
#include "Evolve/World.h"
#include "geometry/Surface.h"
#include "hardware/signalgp_utils.h"
#include "tools/math.h"

///< Experiment headers 
#include "config.h"
#include "BeakerResource.h"
#include "BeakerOrg.h"

///< Standard C++ includes
#include <queue>
#include <set>
#include <utility>
#include <sstream>

class BeakerWorld : public emp::World<BeakerOrg> 
{
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
    using memory_t = hardware_t::memory_t;

    // type for event pairing
    using event_t = std::pair<size_t, size_t>;

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
    size_t HEAT = 2;              ///< Heat type location for EventDrivenGP trait vector        

    /* Web Interface variables */

    surface_t surface;                    ///< Variable that holds the surface organisms are on
    bool redraw = true;                   ///< Variable to tell if charts need to be redraw

    /* Statistics variables */
    
    int death_stv = 0;       ///< Variables that holds number of deaths
    int death_eat = 0;
    int death_pop = 0;

    int blue_cnt = 0;        ///< Variables that holds number of colors
    int cyan_cnt = 0;
    int lime_cnt = 0;
    int yellow_cnt = 0;
    int red_cnt = 0;
    int white_cnt = 0;

    double avg_blue = 0.0;    ///< Variables that store average radius of each color
    double avg_cyan = 0.0;
    double avg_lime = 0.0;
    double avg_yellow = 0.0;
    double avg_red = 0.0;
    double avg_white = 0.0;


    /* World Event Tracker/Queue */

    std::set<size_t> kill_list;                     ///< Holds org ids that have been eaten. <org_wid>
    std::set<size_t> birth_list;                    ///< Holds org ids that can give birth. <org_wid>
    std::set<size_t> eater_list;                    ///< Holds org ids that have eaten a resource <org_wid>
    std::map<size_t, size_t> eaten_list;            ///< Variable that holds resources that have been eaten along with organims world-id. <res_id, org_id>
    std::queue<event_t> events;                     ///< Queue to hold all events that happen in the world. <(size_t) trait, wid/mid>
    enum class Trait {CONSUME, KILLED, BIRTH};        ///< Different kind of events

  public:  
    BeakerWorld(BeakerWorldConfig & _config)
      : config(_config), id_map(), next_id(1), 
        hm_size(config.HM_SIZE()), inst_lib(), event_lib(), 
        signalgp_mutator(), surface({config.WORLD_X(), config.WORLD_Y()})
    {
      random_ptr = emp::NewPtr<emp::Random>(config.SEED());
      Config_All();
    }

    ~BeakerWorld() 
    { 
      id_map.clear(); 
      resources.clear();
      kill_list.clear();
      birth_list.clear();
      eaten_list.clear();
      while(!events.empty()) {events.pop();}
      random_ptr.Delete();
    }

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

    /* Getter and setter functions for statistics! */

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
    bool GetRedraw() {return redraw;}

    std::string GetAvgBlue() {return Precision(avg_blue);}       ///< Functions dedicated to returning population distributions
    std::string GetAvgCyan() {return Precision(avg_cyan);}
    std::string GetAvgLime() {return Precision(avg_lime);}
    std::string GetAvgYellow() {return Precision(avg_yellow);}
    std::string GetAvgRed() {return Precision(avg_red);}
    std::string GetAvgWhite() {return Precision(avg_white);}

    void SetRedraw(bool b) {redraw = b;}


    /* Functions dedicated to calculating statistics! */
    
    void Col_Birth(size_t h);               ///< Will keep track of organisms in the system
    void Col_Death(size_t h);               ///< Will keep track of organisms in the system

    void Sum_Rad(size_t h, double radius);                ///< Will calculate average radius per heat signature
    void Calc_Rad();                                      ///< Divide each sum of radii by the color count
    void Reset_Avg();                                     ///< Resets Average before every run
    std::string Precision(double radius);

    void Print_Lists();                     ///< Will print all the lists we have
    void Print_Queue(std::queue<event_t> copy_queue);                     ///< Will print Events queue

    /* Functions dedicated to the physics of the system */

    bool PairCollision(BeakerOrg & body1, BeakerOrg & body2) {return true;} ///< Function dedicated to dealing with organims collisions [TODO]

    void ProcessEvents();  ///< Process all the events in order!


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
    // Reset offspring's hardware so no issues arise
    org.GetBrain().ResetHardware();
    org.GetBrain().SpawnCore(0, memory_t(), true);
    org.SetEnergy(config.INIT_ENERGY());

    // Set parent attributes to offspring and possibly mutate radius
    emp::Point parent_center = surface.GetCenter(GetOrg(parent_pos).GetSurfaceID());
    double parent_radius = surface.GetRadius(GetOrg(parent_pos).GetSurfaceID());
    double off_radius = MutRad(parent_radius, org);
    size_t heat = Calc_Heat(off_radius);
    org.SetHeat(heat);

    // Mutate the offspring!
    DoMutationsOrg(org);

    // Add to the surface and set its surface id!
    size_t surface_id = surface.AddBody(&org, parent_center, off_radius, heat);
    org.SetSurfaceID(surface_id);
    org.SetTrait(HEAT, heat);

    // Keep track of organism heat signature.
    Col_Birth(heat);
  });

  // Make sure that we are tracking organisms by their IDs once placed.
  OnPlacement( [this](size_t pos)
  {
    // Set appropiate traits and store in map_id for future access
    size_t id = next_id++;
    GetOrg(pos).SetTrait((size_t)BeakerOrg::Trait::MAP_ID, id);
    GetOrg(pos).SetTrait((size_t)BeakerOrg::Trait::WRL_ID, pos);
    id_map[id] = &GetOrg(pos);
  });

  // Trigger for an organisms death.
  OnOrgDeath( [this](size_t pos) 
  {
    // Remove id from these lists 
    birth_list.erase(pos);
    eater_list.erase(pos);
    kill_list.erase(pos);

    // Keep track of org deaths and remove from id_map and surface!
    Col_Death((size_t) GetOrg(pos).GetTrait(HEAT));
    surface.RemoveBody(GetOrg(pos).GetSurfaceID());
    id_map.erase(GetOrg(pos).GetTrait((size_t)BeakerOrg::Trait::MAP_ID));
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
    const size_t id = (size_t) hw.GetTrait((size_t) BeakerOrg::Trait::MAP_ID);
    emp::Ptr<BeakerOrg> org_ptr = id_map[id];
    emp::Angle facing = org_ptr->GetFacing();
    surface.TranslateWrap( org_ptr->GetSurfaceID(), facing.GetPoint(1.0) );
    }, 1, "Move forward.");

  inst_lib.AddInst("SpinRight", [this](hardware_t & hw, const inst_t & inst) mutable 
  {
    const size_t id = (size_t) hw.GetTrait((size_t) BeakerOrg::Trait::MAP_ID);
    emp::Ptr<BeakerOrg> org_ptr = id_map[id];
    org_ptr->RotateDegrees(-5.0);
  }, 1, "Rotate -5 degrees.");

  inst_lib.AddInst("SpinLeft", [this](hardware_t & hw, const inst_t & inst) mutable 
  {
    const size_t id = (size_t) hw.GetTrait((size_t) BeakerOrg::Trait::MAP_ID);
    emp::Ptr<BeakerOrg> org_ptr = id_map[id];
    org_ptr->RotateDegrees(5.0);
  }, 1, "Rotate 5 degrees.");

  inst_lib.AddInst("Consume", [this](hardware_t & hw, const inst_t & inst) mutable 
  {
    const size_t id = (size_t) hw.GetTrait((size_t) BeakerOrg::Trait::MAP_ID);
    emp::Ptr<BeakerOrg> org_ptr = id_map[id];
    surface.FindOverlap( org_ptr->GetSurfaceID() );  // Surface functions automatically try to eat on overlap!
  }, 1, "Consume a resource!");
}

void BeakerWorld::Config_Surf() ///< Function dedicated to configuring the surface
{
  // Setup surface functions to allow organisms to eat.
  surface.AddOverlapFun([this](BeakerOrg & pred, BeakerOrg & prey) 
  {
    double pred_rd = surface.GetRadius(pred.GetSurfaceID());
    double prey_rd = surface.GetRadius(prey.GetSurfaceID());
    double lower_b = pred_rd - (pred_rd * config.MIN_CONSUME_RATIO());
    double upper_b = pred_rd + (pred_rd * config.MAX_CONSUME_RATIO());

    // If prey radius is within pred radius bound
    if(lower_b < prey_rd && prey_rd < upper_b)
    {
      // If this is the first time the organism is about to be killed
      size_t prey_id = prey.GetTrait((size_t)BeakerOrg::Trait::WRL_ID);

      if(kill_list.find(prey_id) == kill_list.end())
      {
        // std::cerr << "ORG EATEN!" << std::endl;
        pred.AddEnergy(prey.GetEnergy() / 2.0, config.MAX_ENERGY_CAP());
        kill_list.insert(prey_id);
        events.push(std::make_pair((size_t)Trait::KILLED, prey_id));
        death_eat++;
        redraw = true;
      }
    }
  });
  //< Overlap function for organism to eat a resource
  surface.AddOverlapFun( [this](BeakerOrg & org, BeakerResource & res) 
  {
    // IDs to locate organims
    const size_t org_wid = org.GetTrait((size_t)BeakerOrg::Trait::WRL_ID);
    const size_t res_mid = res.GetMapID();
    // Minimum radius size to consume resource
    double thresh = ((config.MAX_RAD_VAL()-config.MIN_RAD_VAL()) * config.CONSUME_RES_THRESH()) + config.MIN_RAD_VAL();;
    const double org_rd = surface.GetRadius(org.GetSurfaceID());

    // If the resource has not been eaten yet and the size requirement is met?
    if(eaten_list.find(res_mid) == eaten_list.end() && org_rd <= thresh)
    {
      // We store the resource id and the organism world_id that ate it. 
      eaten_list[res_mid] = org_wid;
      eater_list.insert(org_wid);
      events.push(std::make_pair((size_t)Trait::CONSUME, res_mid)); 
    }
  });
  surface.AddOverlapFun( [](BeakerResource &, BeakerResource &) 
  {
    ///< Leave empty lol. Resources can't eat other resources. 
  });
  surface.AddOverlapFun( [this](BeakerResource & res, BeakerOrg & org) 
  {
    ///< Leave empty lol. Resources shouldn't eat organisms. 
  });
}

void BeakerWorld::Config_OnUp() ///< Function dedicated to configuring the OnUpdate function
{
  // On each update, run organisms and make sure they stay on the surface.
  OnUpdate([this](size_t)
  {
    // std::cerr << "START UP" << std::endl;
    // Process all organisms.
    Process(config.PROCESS_NUM());
    // std::cerr << "PASS PROCESS" << std::endl;

    // Update each organism.
    for (size_t pos = 0; pos < pop.size(); pos++) 
    {
      // If position contains a NullPtr skippppppp
      if (pop[pos].IsNull()) {continue;}

      auto & org = *pop[pos];

      // Subtract energy per update call
      org.SubEnergy(config.ENERGY_REDUCTION());

      // If an organism has enough energy to reproduce, store id.
      if (org.GetEnergy() > config.REPRODUCTION_THRESH()) 
      {
        birth_list.insert(pos);
        events.push(std::make_pair((size_t)Trait::BIRTH, pos));
        redraw = true;
      }
      // If an organism starves to death, store id.
      if (org.GetEnergy() <= 0.0)
      {
        death_stv++;
        kill_list.insert(pos);
        events.push(std::make_pair((size_t)Trait::KILLED, pos));
        redraw = true;
      }
      Sum_Rad(org.GetHeat(), surface.GetRadius(org.GetSurfaceID()));
    }
    Calc_Rad();

    // std::cerr << "PASS FOR" << std::endl;
    ProcessEvents();
    // std::cerr << "FINISH UP" << std::endl;
    // std::cerr << "********************************************\n" << std::endl;
  });
}

void BeakerWorld::Reset_Avg()
{
  avg_blue = 0.0;
  avg_cyan = 0.0;
  avg_lime = 0.0;
  avg_yellow = 0.0;
  avg_red = 0.0;
  avg_white = 0.0;
}

void BeakerWorld::ProcessEvents()
{
  // Print_Lists();
  // std::cerr << "PROCESS EVENTS" << std::endl;
  while(!events.empty())
  {
    size_t event = (size_t) events.front().first;
    size_t or_wid = (size_t) events.front().second;

    // Death Events (Eaten/Starved)
    if(event == (size_t) Trait::KILLED)
    {
      // std::cerr << "START DEATH" << std::endl;
      DoDeath(or_wid);
      // std::cerr << "END DEATH" << std::endl;
    }

    // If consume resource event
    else if(event == (size_t) Trait::CONSUME)
    {
      // std::cerr << "START CONSUME" << std::endl;
      size_t org_id = eaten_list[or_wid];
      // If organism is still able to eat
      if(eater_list.find(org_id) != eater_list.end())
      {
        auto & org = *pop[eaten_list[or_wid]];
        org.AddEnergy(config.RESOURCE_POWERUP(), config.MAX_ENERGY_CAP());
        double x = random_ptr->GetDouble(config.WORLD_X());
        double y = random_ptr->GetDouble(config.WORLD_Y());
        surface.SetCenter(resources[or_wid].GetSurfaceID(), {x,y});
      }
      // std::cerr << "END CONSUME" << std::endl;      
    }
    // If birth event
    else if(event == (size_t) Trait::BIRTH)
    {
      // std::cerr << "START BIRTH" << std::endl;
      // Check if we can add new org to pop.
      if(GetNumOrgs() < config.MAX_POP_SIZE())
      {
        // If org is still in the birth_list
        if(birth_list.find(or_wid) != birth_list.end())
        {
          // Split energy for building offspring by half and spawn new organism.
          auto & org = GetOrg(or_wid);
          // std::cerr << "ENERGY: " << org.GetEnergy() << std::endl;
          org.SubEnergy(org.GetEnergy() / config.REPRODUCTION_PENALTY());
          DoBirth(GetOrg(or_wid), or_wid);
          birth_list.erase(org.GetTrait((size_t)BeakerOrg::Trait::WRL_ID));
        }
      }
      // std::cerr << "END BIRTH" << std::endl;
    }
    // Error
    else
    {
      std::cerr << "EVENT-ID NOT FOUND" << std::endl;
      exit(-1);
    }
    events.pop();
  }

  // std::cerr << std::endl;
  // Print_Lists();

  // Clear list for next update call
  birth_list.clear();
  kill_list.clear();
  eater_list.clear();
  eaten_list.clear();

  // std::cerr << "FINISH PROCESS" << std::endl;
}

void BeakerWorld::Initial_Inject() ///< Function dedicated to injection the initial population or organisms and resources
{
  // Initialize a populaton of random organisms.
  Inject(BeakerOrg(inst_lib, event_lib, random_ptr), config.INIT_POP_SIZE());
  for (size_t i = 0; i < config.INIT_POP_SIZE(); i++) 
  {
    // Random coordiantes for organism
    double x = random_ptr->GetDouble(config.WORLD_X());
    double y = random_ptr->GetDouble(config.WORLD_Y());
    
    // Get random radius and calculate heat color
    double rad = random_ptr->GetDouble(config.MIN_RAD_VAL(), config.MAX_RAD_VAL());
    size_t heat = Calc_Heat(rad);
    Col_Birth(heat);

    // Get and Set organism
    BeakerOrg & org = GetOrg(i);
    org.GetBrain().SetProgram(emp::GenRandSignalGPProgram(*random_ptr, inst_lib, config.PROGRAM_MIN_FUN_CNT(),\
      config.PROGRAM_MAX_FUN_CNT(), config.PROGRAM_MIN_FUN_LEN(), config.PROGRAM_MAX_FUN_LEN(), \
      config.PROGRAM_MIN_ARG_VAL(), config.PROGRAM_MAX_ARG_VAL()));
    org.SetTrait(HEAT, heat);
    org.SetEnergy(config.INIT_ENERGY());

    // Add organism to the surface and store its id
    size_t surface_id = surface.AddBody(&org, {x,y}, rad, heat);
    org.SetSurfaceID(surface_id);
    org.SetHeat(heat);
    Sum_Rad(heat, surface.GetRadius(org.GetSurfaceID()));
  }
  // Add in resources.
  resources.resize(config.NUM_RESOURCE_SOURCES());

  for(size_t i = 0; i < resources.size(); ++i)
  {
    //Place them randomly throughout the canvas and store their map_id
    double x = random_ptr->GetDouble(config.WORLD_X());
    double y = random_ptr->GetDouble(config.WORLD_Y());
    resources[i].SetMapID(i);
    resources[i].SetSurfaceID(surface.AddBody(&resources[i], {x,y}, 3.0, config.HM_SIZE()));
  }
  Calc_Rad();
}

size_t BeakerWorld::Calc_Heat(double r) ///< Function dedicated to injection the initial population or organisms and resources
{
  double diff = config.MAX_RAD_VAL() - config.MIN_RAD_VAL();
  diff = diff / (double) hm_size;
  size_t pos = 0;
  double curr = config.MIN_RAD_VAL();

  while(curr <= config.MAX_RAD_VAL())
  {
    curr += diff;
    if(r <= curr )
    {
      return pos;
    }
    pos++;
  }
  return hm_size - 1;
}

void BeakerWorld::Col_Birth(size_t h)
{
  switch(h)
  {
  case 0:
    blue_cnt++;
    return;

  case 1:
    cyan_cnt++;
    return;
  
  case 2:
    lime_cnt++;
    return;
  
  case 3:
    yellow_cnt++;
    return;
  
  case 4:
    red_cnt++;
    return;
  
  case 5:
    white_cnt++;
    return;
  }
  std::cout << "Col_Birth Not Found: " << h << std::endl;
}

void BeakerWorld::Col_Death(size_t h)
{
  switch(h)
  {
  case 0:
    blue_cnt--;
    return;

  case 1:
    cyan_cnt--;
    return;
  
  case 2:
    lime_cnt--;
    return;
  
  case 3:
    yellow_cnt--;
    return;
  
  case 4:
    red_cnt--;
    return;
  
  case 5:
    white_cnt--;
    return;
  }
  std::cout << "Col_Death Not Found: " << h << std::endl;
}

void BeakerWorld::Sum_Rad(size_t h, double radius)
{
  switch(h)
  {
    case 0:
      avg_blue += radius;
      return;

    case 1:
      avg_cyan += radius;
      return;
    
    case 2:
      avg_lime += radius;
      return;
    
    case 3:
      avg_yellow += radius;
      return;
    
    case 4:
      avg_red += radius;
      return;
    
    case 5:
      avg_white += radius;
      return;
  }
}
void BeakerWorld::Calc_Rad()
{
  (blue_cnt == 0.0) ? avg_blue = 0.0 : avg_blue /= blue_cnt;
  (cyan_cnt == 0.0) ? avg_cyan = 0.0 : avg_cyan /= cyan_cnt;
  (lime_cnt == 0.0) ? avg_lime = 0.0 : avg_lime /= lime_cnt;
  (yellow_cnt == 0.0) ? avg_yellow = 0.0 : avg_yellow /= yellow_cnt;
  (red_cnt == 0.0) ? avg_red = 0.0 : avg_red /= red_cnt;
  (white_cnt == 0.0) ? avg_white = 0.0 : avg_white /= white_cnt;
}

std::string BeakerWorld::Precision(double radius)
{
  std::string rad = std::to_string(radius);
  std::stringstream stream;
  stream << std::fixed << std::setprecision(3) << rad;
  return stream.str();
}

void BeakerWorld::Print_Lists()
{
  std::cerr << "Kill_List: ";
  for (const size_t id : kill_list)
  {
    std::cerr << id << ", ";
  }
  std::cerr << std::endl;

  std::cerr << "Birth_List: ";
  for (const size_t id : birth_list)
  {
    std::cerr << id << ", ";
  }
  std::cerr << std::endl;

  std::cerr << "Eater_List: ";
  for (const size_t id : eater_list)
  {
    std::cerr << id << ", ";
  }
  std::cerr << std::endl;

  std::cerr << "Eaten_List: ";
  for (auto id : eaten_list)
  {
    std::cerr << "(" << id.first << ", " << id.second << "), ";
  }
  std::cerr << std::endl;

  std::cerr << "Events: ";
  Print_Queue(events);
}

void BeakerWorld::Print_Queue(std::queue<event_t> copy_queue)
{
  while (!copy_queue.empty())
  {
    std::cerr << "(" << copy_queue.front().first << ", " << copy_queue.front().second << "), ";
    copy_queue.pop();
  }
  std::cerr << std::endl;
}

/* Functions dedicated to experiment functionality */

double BeakerWorld::MutRad(double r, BeakerOrg & org)
{
  if(random_ptr->P(config.RADIUS_MUT()))
    {
      double radius = surface.GetRadius(org.GetSurfaceID());
      double diff = random_ptr->GetRandNormal(0, .5);
      double new_r = radius + diff;

      if(new_r > config.MAX_RAD_VAL())
      {
        new_r = config.MAX_RAD_VAL();
      }
      if(new_r < config.MIN_RAD_VAL())
      {
        new_r = config.MIN_RAD_VAL();
      }

      std::cerr << "(" << r << ")RADMUT(" << new_r << ")" << std::endl; 

      return new_r;
    }
    return r;
}

#endif
