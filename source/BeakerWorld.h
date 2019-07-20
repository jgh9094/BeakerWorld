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
#include <unistd.h>
#include <iomanip>

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
    int next_id;                                           ///< Variable that holds the id placement for id_map
    size_t hm_size;                                           ///< Variable that holds the size of the heat map


    /* Hardware variables */

    inst_lib_t inst_lib;          ///< Variable that holds instruction library
    event_lib_t event_lib;        ///< Variable that holds event library
    mutator_t signalgp_mutator;   ///< Variable mutates organism genoms


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
    std::map<size_t, size_t> eaten_list;            ///< Variable that holds resources that have been eaten along with organims world-id. <res_id, org_wid>
    std::queue<event_t> events;                     ///< Queue to hold all events that happen in the world. <(size_t) trait, wid/mid>
    enum class Trait {CONSUME, KILLED, BIRTH};      ///< Different kind of events

    /* Debugging Variables */

    bool pred_inject = false;                       ///< Has the predetor organims been injected?

  public:  

    BeakerWorld(BeakerWorldConfig & _config)
      : config(_config), id_map(), next_id(0), 
        hm_size(config.HM_SIZE()), inst_lib(), event_lib(), 
        signalgp_mutator(), surface({config.WORLD_X(), config.WORLD_Y()})
    {
      random_ptr = emp::NewPtr<emp::Random>(config.SEED());
      ConfigAll();
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


    /* Functions dedicated to the initilization of the run! */

    void ConfigAll();             ///< Function will run all Config_* functions!
    void ConfigWorld();           ///< Function will configure the world
    void ConfigMut();             ///< Function will configure the mutation operator
    void ConfigInst();            ///< Function will configure the instructions and instrucion library
    void ConfigSurface();            ///< Function will configure the surface
    void ConfigOnUp();            ///< Function will configure the OnUpdate function
    void InitialInject();         ///< Function inject the initial population into the world
    size_t Calc_Heat(double r);    ///< Function will calculate an orgs heat signature


    /* Getter and setter functions for statistics! */

    int GetStv() {return death_stv;}            ///< Function dedicated to keeping track of world deaths
    int GetEat() {return death_eat;}
    int GetPop() {return death_pop;}

    int GetBlue() {return blue_cnt;}            ///< Functions dedicated to returning population distributions
    int GetCyan() {return cyan_cnt;}
    int GetLime() {return lime_cnt;}
    int GetYellow() {return yellow_cnt;}
    int GetRed() {return red_cnt;}
    int GetWhite() {return white_cnt;}

    size_t GetResSize() {return resources.size();}               ///< Functions dedicated to returning container sizes
    size_t GetIDSize() {return id_map.size();}
    size_t GetNextID() {return next_id;}

    bool GetRedraw() {return redraw;}                            ///< Will return the variable to determine if we need to redraw

    std::string GetAvgBlue() {return Precision(avg_blue);}       ///< Functions dedicated to returning population distributions
    std::string GetAvgCyan() {return Precision(avg_cyan);}
    std::string GetAvgLime() {return Precision(avg_lime);}
    std::string GetAvgYellow() {return Precision(avg_yellow);}
    std::string GetAvgRed() {return Precision(avg_red);}
    std::string GetAvgWhite() {return Precision(avg_white);}


    /* Functions dedicated to calculating statistics! */
     
    void Col_Birth(size_t h);                             ///< Will increment number of heat signatures
    void Col_Death(size_t h);                             ///< Will decrement number of heat signatures
    void Sum_Rad(size_t h, double radius);                ///< Will calculate average radius per heat signature
    void Calc_Rad();                                      ///< Divide each sum of radii by the color count
    void Reset_Avg();                                     ///< Resets Average before every run
    std::string Precision(double radius);                 ///< Will set double to 3 precision


    /* Functions dedicated to the physics of the system */

    bool PairCollision(BeakerOrg & body1, BeakerOrg & body2) {return true;}   ///< Function dedicated to dealing with organims collisions [TODO]
    void ProcessEvents();                                                     ///< Process all the events in order!
    void SetRedraw(bool b) {redraw = b;}                                      ///< Return redraws variable for UI
    surface_t & GetSurface() { return surface; }                              ///< Will return the surface that orgs/resources are!


    /* Functions dedicated for experiment functionality */

    double MutRad(double r, BeakerOrg & org);                                 ///< Function will mutate radius, if possible
    void InjectApex();                                                        ///< Will inject a preditor to the world...


    /* Functions dedicated to debugging the system */

    void PrintLists();                                         ///< Will print all the lists we have
    void PrintQueue(std::queue<event_t> copy_queue);           ///< Will print Events queue
};

/* Functions dedicated to the initilization of the run */

void BeakerWorld::ConfigAll()  ///< Function will run all Config_* functions!
{
    ConfigWorld();
    ConfigMut();
    ConfigInst();
    ConfigSurface();
    ConfigOnUp();
    InitialInject();
}

void BeakerWorld::ConfigWorld() ///< Function dedicated to configuring the world
{
  SetPopStruct_Grow(false); // Don't automatically delete organism when new ones are born.

  // Setup organism to share parent's surface features.
  OnOffspringReady([this](BeakerOrg & org, size_t parent_pos)
  {
    // Reset offspring's hardware so no issues arise
    org.GetBrain().ResetHardware();
    org.GetBrain().SpawnCore(0, memory_t(), true);
    org.SetEnergy(config.INIT_ENERGY());

    // Set parent attributes to offspring
    emp::Point parent_center = surface.GetCenter(GetOrg(parent_pos).GetSurfaceID());
    double parent_radius = surface.GetRadius(GetOrg(parent_pos).GetSurfaceID());

    // Mutate the offspring radius!
    double off_radius;
    (config.TESTING()) ? off_radius = parent_radius : off_radius = MutRad(parent_radius, org);

    size_t heat = Calc_Heat(off_radius);
    org.SetHeatID(heat);

    // Mutate the offspirng genome
    if(!config.TESTING()) {DoMutationsOrg(org);}

    // Add to the surface and set its surface id!
    size_t surf_id = surface.AddBody(&org, parent_center, off_radius, heat);
    org.SetSurfaceID(surf_id);
    org.SetTrait((size_t)BeakerOrg::Trait::HEAT_ID, heat);

    // Keep track of organism heat signature.
    Col_Birth(heat);
  });

  // Make sure that we are tracking organisms by their IDs once placed.
  OnPlacement([this](size_t pos)
  {
    // Set appropiate traits and store in map_id for future access
    size_t id = next_id++;
    GetOrg(pos).SetWorldID(pos);
    GetOrg(pos).SetMapID(id);
    std::cerr << "****" << GetOrg(pos).GetSurfaceID() << std::endl;
    std::cerr << "****" << GetOrg(pos).GetWorldID() << std::endl;;
    std::cerr << "****" << GetOrg(pos).GetMapID() << std::endl;;
    std::cerr << "****id" << id << std::endl;
    std::cerr << "****ps" << pos << std::endl;

    GetOrg(pos).SetTrait((size_t)BeakerOrg::Trait::MAP_ID, id);
    GetOrg(pos).SetTrait((size_t)BeakerOrg::Trait::WRL_ID, pos);
    id_map[id] = &GetOrg(pos);
  });

  // Trigger for an organisms death.
  OnOrgDeath( [this](size_t w_pos) 
  {
    // Remove id from these lists 
    birth_list.erase(w_pos);
    eater_list.erase(w_pos);
    kill_list.erase(w_pos);

    // Keep track of org deaths and remove from id_map and surface!
    Col_Death((size_t) GetOrg(w_pos).GetTrait((size_t)BeakerOrg::Trait::HEAT_ID));
    id_map.erase(GetOrg(w_pos).GetTrait((size_t)BeakerOrg::Trait::MAP_ID));
  });
}

void BeakerWorld::ConfigMut() ///< Function dedicated to configuring the mutation operator
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
    if(!config.TESTING()) {signalgp_mutator.ApplyMutations(org.GetBrain().GetProgram(), random);}
    return 1;
  });
}

void BeakerWorld::ConfigInst() ///< Function dedicated to configuring instructions and instrucion library
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

void BeakerWorld::ConfigSurface() ///< Function dedicated to configure the surface
{
    surface.AddOverlapFun( [this](BeakerOrg & pred, BeakerOrg & prey) 
    {
        // Get orgs surface id
        const size_t pred_sid = pred.GetSurfaceID();
        const size_t prey_sid = prey.GetSurfaceID();
        // Get org world id
        const size_t prey_wid = prey.GetWorldID();
        // Use surface id to get radius
        const double pred_rd = surface.GetRadius(pred_sid);
        const double prey_rd = surface.GetRadius(prey_sid);
        // Caluculate upper and lowerbounds
        const double lower_b = pred_rd * config.MIN_CONSUME_RATIO();
        const double upper_b = pred_rd + (pred_rd * config.MAX_CONSUME_RATIO());

        // If prey radius is within pred radius bound
        if(lower_b < prey_rd && prey_rd < upper_b)
        {
            if(kill_list.find(prey_wid) == kill_list.end())
            {
                // std::cerr << "ORG EATEN!" << std::endl;
                pred.AddEnergy(prey.GetEnergy() / config.EAT_ORG_ENERGRY_PROP(), config.MAX_ENERGY_CAP());
                kill_list.insert(prey_wid);
                events.push(std::make_pair((size_t)Trait::KILLED, prey_wid));
                death_eat++;
                redraw = true;
                std::cerr << "ORG-EATEN: " << lower_b << " < " << prey_rd << " < " << upper_b << "_(" << pred_rd << ")" << std::endl;        
            }
        }
    });
    surface.AddOverlapFun( [this](BeakerOrg & org, BeakerResource & res) 
    {
        // Get org values
        const size_t org_sid = org.GetSurfaceID();
        const size_t org_wid = org.GetWorldID();
        const double org_rd = surface.GetRadius(org_sid);
        // Get resoruce vector id for position tracking
        const size_t res_vid =  res.GetMapID();
        // Calcluate threshold
        const double thresh = ((config.MAX_RAD_VAL()-config.MIN_RAD_VAL()) * config.CONSUME_RES_THRESH()) + config.MIN_RAD_VAL();

        // If the resource has not been eaten yet and the size requirement is met
        if(eaten_list.find(res_vid) == eaten_list.end() && org_rd <= thresh)
        {
            // We store the resource id and the organism world_id that ate it. 
            eaten_list[res_vid] = org_wid;
            eater_list.insert(org_wid);
            events.push(std::make_pair((size_t)Trait::CONSUME, res_vid)); 
            std::cerr << "RES-EATEN: " << org_sid << ", " << org_wid << std::endl;
        }
    });
    surface.AddOverlapFun( [](BeakerResource &, BeakerResource &) {
      std::cerr << "ERROR: Resources should not try to eat other resources!" << std::endl;
    });
    surface.AddOverlapFun( [](BeakerResource &, BeakerOrg &) {
      std::cerr << "ERROR: Resources should not try to eat organisms!" << std::endl;
    });
}

void BeakerWorld::ConfigOnUp() ///< Function dedicated to configuring the OnUpdate function
{
  // On each update, run organisms and make sure they stay on the surface.
  OnUpdate([this](size_t)
  {
    // Process all organisms.
    Process(config.PROCESS_NUM());

    // Update each organism.
    for (size_t pos = 0; pos < pop.size(); pos++) 
    {
      // If position contains a NullPtr skippppppp
      if (pop[pos].IsNull()) {continue;}

      auto & org = *pop[pos];

      std::cerr << "ORG_INFO" << std::endl;
      std::cerr << "WRL_ID: " << org.GetWorldID() << std::endl;
      std::cerr << "MAP_ID: " << org.GetMapID() << std::endl;
      std::cerr << "MAP_ID: " << org.GetMapID() << std::endl;
      std::cerr << "SURFID: " << org.GetSurfaceID() << std::endl;
      std::cerr << "RADIUS: " << surface.GetRadius(org.GetSurfaceID()) << std::endl;



      // Subtract energy per update call
      org.SubEnergy(config.ENERGY_REDUCTION());

      // If an organism has enough energy to reproduce, store id.
      if (org.GetEnergy() > config.REPRODUCTION_THRESH()) 
      {
        birth_list.insert(org.GetWorldID());
        events.push(std::make_pair((size_t)Trait::BIRTH, org.GetWorldID()));
        redraw = true;
      }
      // If an organism starves to death, store id.
      if (org.GetEnergy() <= 0.0)
      {
        death_stv++;
        kill_list.insert(org.GetWorldID());
        events.push(std::make_pair((size_t)Trait::KILLED, org.GetWorldID()));
        redraw = true;
      }
    }
    ProcessEvents();
    if(GetUpdate() == 500) {InjectApex();}
  });
}

void BeakerWorld::InitialInject() ///< Function dedicated to injection the initial population or organisms and resources
{
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

    // Initialize a populaton of random organisms.
    Inject(BeakerOrg(inst_lib, event_lib, random_ptr), 1);
    for (size_t i = 0; i < 1; i++) 
    {
        // Random coordiantes for organism
        double x = random_ptr->GetDouble(config.WORLD_X());
        double y = random_ptr->GetDouble(config.WORLD_Y());

        // Get random radius and calculate heat color
        double rad = 6.00000;
        size_t heat = Calc_Heat(rad);
        Col_Birth(heat);

        // Get organism
        BeakerOrg & org = GetOrg(i);

        // Add instructions
        org.PushInst("Vroom"); org.PushInst("Consume"); org.PushInst("Vroom"); org.PushInst("Vroom"); org.PushInst("Consume"); org.PushInst("Vroom"); org.PushInst("Vroom");
        org.PushInst("Vroom"); org.PushInst("Consume"); org.PushInst("Vroom"); org.PushInst("Vroom"); org.PushInst("Consume"); org.PushInst("Vroom"); org.PushInst("Vroom");
        org.PushInst("Vroom"); org.PushInst("Consume"); org.PushInst("Vroom"); org.PushInst("Vroom"); org.PushInst("Consume"); org.PushInst("Vroom"); org.PushInst("Vroom");
        org.PushInst("Vroom"); org.PushInst("Consume"); org.PushInst("Vroom"); org.PushInst("Vroom"); org.PushInst("Consume"); org.PushInst("Vroom"); org.PushInst("Vroom");
        org.PushInst("Vroom"); org.PushInst("Consume"); org.PushInst("Vroom"); org.PushInst("Vroom"); org.PushInst("Consume"); org.PushInst("Vroom"); org.PushInst("Vroom");
        org.PushInst("Vroom"); org.PushInst("Consume"); org.PushInst("Vroom"); org.PushInst("Vroom"); org.PushInst("Consume"); org.PushInst("Vroom"); org.PushInst("Vroom");
        org.PushInst("Vroom"); org.PushInst("Consume"); org.PushInst("Vroom"); org.PushInst("Vroom"); org.PushInst("Consume"); org.PushInst("Vroom"); org.PushInst("Vroom");
        org.PushInst("Vroom"); org.PushInst("Consume"); org.PushInst("Vroom"); org.PushInst("Vroom"); org.PushInst("Consume"); org.PushInst("Vroom"); org.PushInst("Vroom");
        org.PushInst("Vroom"); org.PushInst("Consume"); org.PushInst("Vroom"); org.PushInst("Vroom"); org.PushInst("Consume"); org.PushInst("Vroom"); org.PushInst("Vroom");
        org.PushInst("Vroom"); org.PushInst("Consume"); org.PushInst("Vroom"); org.PushInst("Vroom"); org.PushInst("Consume"); org.PushInst("Vroom"); org.PushInst("Vroom");
        org.PushInst("Vroom"); org.PushInst("Consume"); org.PushInst("Vroom"); org.PushInst("Vroom"); org.PushInst("Consume"); org.PushInst("Vroom"); org.PushInst("Vroom");
        org.PushInst("Vroom"); org.PushInst("Consume"); org.PushInst("Vroom"); org.PushInst("Vroom"); org.PushInst("Consume"); org.PushInst("Vroom"); org.PushInst("Vroom");
        org.PushInst("Vroom"); org.PushInst("Consume"); org.PushInst("Vroom"); org.PushInst("Vroom"); org.PushInst("Consume"); org.PushInst("Vroom"); org.PushInst("Vroom");
        org.PushInst("Vroom"); org.PushInst("Consume"); org.PushInst("Vroom"); org.PushInst("Vroom"); org.PushInst("Consume"); org.PushInst("Vroom"); org.PushInst("Vroom");
        org.PushInst("Vroom"); org.PushInst("Consume"); org.PushInst("Vroom"); org.PushInst("Vroom"); org.PushInst("Consume"); org.PushInst("Vroom"); org.PushInst("Vroom");
        org.PushInst("Vroom"); org.PushInst("Consume"); org.PushInst("Vroom"); org.PushInst("Vroom"); org.PushInst("Consume"); org.PushInst("Vroom"); org.PushInst("Vroom");
        org.PushInst("Vroom"); org.PushInst("Consume"); org.PushInst("Vroom"); org.PushInst("Vroom"); org.PushInst("Consume"); org.PushInst("Vroom"); org.PushInst("Vroom");
        org.PushInst("Vroom"); org.PushInst("Consume"); org.PushInst("Vroom"); org.PushInst("Vroom"); org.PushInst("Consume"); org.PushInst("Vroom"); org.PushInst("Vroom");
        org.PushInst("Vroom"); org.PushInst("Consume"); org.PushInst("Vroom"); org.PushInst("Vroom"); org.PushInst("Consume"); org.PushInst("Vroom"); org.PushInst("Vroom");
        org.PushInst("Vroom"); org.PushInst("Consume"); org.PushInst("Vroom"); org.PushInst("Vroom"); org.PushInst("Consume"); org.PushInst("Vroom"); org.PushInst("Vroom");
        org.PushInst("Vroom"); org.PushInst("Consume"); org.PushInst("Vroom"); org.PushInst("Vroom"); org.PushInst("Consume"); org.PushInst("Vroom"); org.PushInst("Vroom");
        org.PushInst("SpinRight"); org.PushInst("SpinRight");
        org.SetEnergy(config.INIT_ENERGY());

        // Add organism to the surface and store its id
        size_t surf_id = surface.AddBody(&org, {x,y}, rad, heat);
        org.SetSurfaceID(surf_id);
        org.SetHeatID(heat);
        Sum_Rad(heat, surface.GetRadius(org.GetSurfaceID()));
    }
    
    Calc_Rad();
}


/* Functions dedicated to calculating statistics! */

void BeakerWorld::Reset_Avg()  ///< Resets Average before every run
{
  avg_blue = 0.0;
  avg_cyan = 0.0;
  avg_lime = 0.0;
  avg_yellow = 0.0;
  avg_red = 0.0;
  avg_white = 0.0;
}

void BeakerWorld::ProcessEvents() ///< Process all the events in order!
{
  while(!events.empty())
  {
    size_t event = (size_t) events.front().first;
    size_t id = (size_t) events.front().second;

    // Death Events (Eaten/Starved)
    if(event == (size_t) Trait::KILLED)
    {
      DoDeath(id);
    }

    // If consume resource event
    else if(event == (size_t) Trait::CONSUME)
    {
      size_t org_wid = eaten_list[id];

      if(eater_list.find(org_wid) != eater_list.end())
      {
        auto & org = *pop[eaten_list[id]];
        org.AddEnergy(config.RESOURCE_POWERUP(), config.MAX_ENERGY_CAP());
        double x = random_ptr->GetDouble(config.WORLD_X());
        double y = random_ptr->GetDouble(config.WORLD_Y());
        surface.SetCenter(resources[id].GetSurfaceID(), {x,y});
        eater_list.erase(org_wid);
        eaten_list.erase(id);
      }
    }
    // If birth event
    else if(event == (size_t) Trait::BIRTH)
    {
        // Check if we can add new org to pop.
        if(GetNumOrgs() < config.MAX_POP_SIZE())
        {
            // If org is still in the birth_list
            if(birth_list.find(id) != birth_list.end())
            {
                // Split energy for building offspring by half and spawn new organism.
                auto & org = GetOrg(id);
                org.SubEnergy(org.GetEnergy() / config.REPRODUCTION_PENALTY());
                DoBirth(GetOrg(id), GetOrg(id).GetWorldID());
                birth_list.erase(org.GetTrait((size_t)BeakerOrg::Trait::WRL_ID));
            }
        }
    }
    // Error
    else
    {
      std::cerr << "EVENT-ID NOT FOUND" << std::endl;
      exit(-1);
    }
    events.pop();
  }

  // Clear list for next update call
  birth_list.clear();
  kill_list.clear();
  eater_list.clear();
  eaten_list.clear();
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

void BeakerWorld::Col_Birth(size_t h)  ///< Will increment number of heat signatures
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

void BeakerWorld::Col_Death(size_t h)  ///< Will decrement number of heat signatures
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

void BeakerWorld::Sum_Rad(size_t h, double radius)  ///< Will calculate average radius per heat signature
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

void BeakerWorld::Calc_Rad()  ///< Divide each sum of radii by the color count
{
  (blue_cnt == 0.0) ? avg_blue = 0.0 : avg_blue /= blue_cnt;
  (cyan_cnt == 0.0) ? avg_cyan = 0.0 : avg_cyan /= cyan_cnt;
  (lime_cnt == 0.0) ? avg_lime = 0.0 : avg_lime /= lime_cnt;
  (yellow_cnt == 0.0) ? avg_yellow = 0.0 : avg_yellow /= yellow_cnt;
  (red_cnt == 0.0) ? avg_red = 0.0 : avg_red /= red_cnt;
  (white_cnt == 0.0) ? avg_white = 0.0 : avg_white /= white_cnt;
}

std::string BeakerWorld::Precision(double radius)  ///< Will set double to 3 precision
{
  std::ostringstream os;
  os << std::fixed;
  os << std::setprecision(3);
  os << radius;
  std::string pre = os.str();
  return pre;
}

/* Functions dedicated to experiment functionality */

double BeakerWorld::MutRad(double r, BeakerOrg & org)  ///< Function will mutate radius, if possible
{
  if(random_ptr->P(config.RADIUS_MUT()))
    {
      double radius = surface.GetRadius(org.GetSurfaceID());
      double diff = random_ptr->GetRandNormal(0, .3);
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

void BeakerWorld::InjectApex() ///< Will inject a preditor to the world...
{
  std::cerr << "INJECTING-APEX" << std::endl;
  BeakerOrg org(inst_lib, event_lib, random_ptr);
  org.GetBrain().SpawnCore(0, memory_t(), true);

  // Random coordiantes for organism
  double x = random_ptr->GetDouble(config.WORLD_X());
  double y = random_ptr->GetDouble(config.WORLD_Y());

  // Get random radius and calculate heat color
  double rad = 7.00000;
  size_t heat = Calc_Heat(rad);
  Col_Birth(heat);

  // Add instructions
  org.PushInst("Vroom"); org.PushInst("Consume"); org.PushInst("Vroom"); org.PushInst("Vroom"); org.PushInst("Consume"); org.PushInst("Vroom"); org.PushInst("Vroom");
  org.PushInst("Vroom"); org.PushInst("Consume"); org.PushInst("Vroom"); org.PushInst("Vroom"); org.PushInst("Consume"); org.PushInst("Vroom"); org.PushInst("Vroom");
  org.PushInst("Vroom"); org.PushInst("Consume"); org.PushInst("Vroom"); org.PushInst("Vroom"); org.PushInst("Consume"); org.PushInst("Vroom"); org.PushInst("Vroom");
  org.PushInst("Vroom"); org.PushInst("Consume"); org.PushInst("Vroom"); org.PushInst("Vroom"); org.PushInst("Consume"); org.PushInst("Vroom"); org.PushInst("Vroom");
  org.PushInst("Vroom"); org.PushInst("Consume"); org.PushInst("Vroom"); org.PushInst("Vroom"); org.PushInst("Consume"); org.PushInst("Vroom"); org.PushInst("Vroom");
  org.PushInst("Vroom"); org.PushInst("Consume"); org.PushInst("Vroom"); org.PushInst("Vroom"); org.PushInst("Consume"); org.PushInst("Vroom"); org.PushInst("Vroom");
  org.PushInst("Vroom"); org.PushInst("Consume"); org.PushInst("Vroom"); org.PushInst("Vroom"); org.PushInst("Consume"); org.PushInst("Vroom"); org.PushInst("Vroom");
  org.PushInst("Vroom"); org.PushInst("Consume"); org.PushInst("Vroom"); org.PushInst("Vroom"); org.PushInst("Consume"); org.PushInst("Vroom"); org.PushInst("Vroom");
  org.PushInst("Vroom"); org.PushInst("Consume"); org.PushInst("Vroom"); org.PushInst("Vroom"); org.PushInst("Consume"); org.PushInst("Vroom"); org.PushInst("Vroom");
  org.PushInst("Vroom"); org.PushInst("Consume"); org.PushInst("Vroom"); org.PushInst("Vroom"); org.PushInst("Consume"); org.PushInst("Vroom"); org.PushInst("Vroom");
  org.PushInst("Vroom"); org.PushInst("Consume"); org.PushInst("Vroom"); org.PushInst("Vroom"); org.PushInst("Consume"); org.PushInst("Vroom"); org.PushInst("Vroom");
  org.PushInst("Vroom"); org.PushInst("Consume"); org.PushInst("Vroom"); org.PushInst("Vroom"); org.PushInst("Consume"); org.PushInst("Vroom"); org.PushInst("Vroom");
  org.PushInst("Vroom"); org.PushInst("Consume"); org.PushInst("Vroom"); org.PushInst("Vroom"); org.PushInst("Consume"); org.PushInst("Vroom"); org.PushInst("Vroom");
  org.PushInst("Vroom"); org.PushInst("Consume"); org.PushInst("Vroom"); org.PushInst("Vroom"); org.PushInst("Consume"); org.PushInst("Vroom"); org.PushInst("Vroom");
  org.PushInst("Vroom"); org.PushInst("Consume"); org.PushInst("Vroom"); org.PushInst("Vroom"); org.PushInst("Consume"); org.PushInst("Vroom"); org.PushInst("Vroom");
  org.PushInst("Vroom"); org.PushInst("Consume"); org.PushInst("Vroom"); org.PushInst("Vroom"); org.PushInst("Consume"); org.PushInst("Vroom"); org.PushInst("Vroom");
  org.PushInst("Vroom"); org.PushInst("Consume"); org.PushInst("Vroom"); org.PushInst("Vroom"); org.PushInst("Consume"); org.PushInst("Vroom"); org.PushInst("Vroom");
  org.PushInst("Vroom"); org.PushInst("Consume"); org.PushInst("Vroom"); org.PushInst("Vroom"); org.PushInst("Consume"); org.PushInst("Vroom"); org.PushInst("Vroom");
  org.PushInst("Vroom"); org.PushInst("Consume"); org.PushInst("Vroom"); org.PushInst("Vroom"); org.PushInst("Consume"); org.PushInst("Vroom"); org.PushInst("Vroom");
  org.PushInst("Vroom"); org.PushInst("Consume"); org.PushInst("Vroom"); org.PushInst("Vroom"); org.PushInst("Consume"); org.PushInst("Vroom"); org.PushInst("Vroom");
  org.PushInst("Vroom"); org.PushInst("Consume"); org.PushInst("Vroom"); org.PushInst("Vroom"); org.PushInst("Consume"); org.PushInst("Vroom"); org.PushInst("Vroom");
  org.PushInst("SpinLeft"); org.PushInst("SpinLeft");
  org.SetEnergy(config.INIT_ENERGY());

  // Add organism to the surface and store its id
  size_t surf_id = surface.AddBody(&org, {x,y}, rad, heat);
  std::cerr << "SURFID=" << surf_id << std::endl;
  org.SetSurfaceID(surf_id);
  org.SetHeatID(heat);
  Sum_Rad(heat, surface.GetRadius(org.GetSurfaceID()));
  std::cerr << "BEFORE-INJECT" << std::endl;
  Inject(org);
  std::cerr << "AFTER-INJECT\n" << std::endl;
  std::cerr << "ORG_SURFID=" << org.GetSurfaceID() << std::endl;
  std::cerr << "ORG_WRLLID=" << org.GetWorldID() << std::endl;  
  std::cerr << "ORG_MAPPID=" << org.GetMapID() << std::endl;

  for(size_t i = 0; i < pop.size(); ++i)
  {
     auto & o = GetOrg(i);
     if(surface.GetRadius(o.GetSurfaceID()) == 7.0)
     {
       std::cerr << "i=" << i << std::endl;
       GetOrg(i).SetWorldID(i);
       GetOrg(i).SetMapID(i);
       break;
     }
  }
  std::cerr << "ORG_SURFID=" << org.GetSurfaceID() << std::endl;
  std::cerr << "ORG_WRLLID=" << org.GetWorldID() << std::endl;  
  std::cerr << "ORG_MAPPID=" << org.GetMapID() << std::endl;

  // exit(-1);
}

#endif