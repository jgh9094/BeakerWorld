#ifndef BEAKER_WORLD_CONFIG_H
#define BEAKER_WORLD_CONFIG_H

#include "config/config.h"

EMP_BUILD_CONFIG( BeakerConfig,
  GROUP(WORLD_STRUCTURE, "How should each organism's genome be setup?"),
  VALUE(WORLD_X,        double,     1400.0,       "How wide is the World?"),
  VALUE(WORLD_Y,        double,     900.0,        "How tall is the World?"),
  VALUE(INIT_POP_SIZE,  size_t,     500,          "How many organisms should we start with?"),  
  VALUE(MAX_POP_SIZE,   size_t,     3000,         "What are the most organisms that should be allowed in pop?"),
  VALUE(MAX_UPS,        size_t,     1,            "How many generations should the runs go for?"),
  VALUE(SEED,           int,        2,            "Random number seed (0 for based on time)"),
  VALUE(HM_SIZE,        size_t,     6,            "Size of the heat map."),
  VALUE(PROCESS_NUM,    size_t,     7,            "Number of steps an organism runs on update."),
  VALUE(PRED_INJECT,    size_t,     1000,         "Update to inject the preditor org"),

  GROUP(RESOURCE, "How are the resouces set up?"),
  VALUE(NUMBER_RESOURCES,     size_t,    500,      "How many sources of resouces should there be?"),
  VALUE(RESOURCE_POWERUP,     double,    300.0,    "Energy gained from eating a resource"),
  VALUE(RESOURCE_UPS_MAX,      size_t,    100,      "How many sectors are we making (per axis)?" ),

  GROUP(ORGANISM, "How are the organisms set up?"),
  VALUE(INIT_ENERGY,           double,    1000.0,   "How many resources should be in the eviornment?"), 
  VALUE(MAX_ENERGY_CAP,        double,    3000.0,   "Maximum amount of energy an organism could get!"),
  VALUE(REPRODUCTION_THRESH,   double,    2000.0,   "Energy needed to produce an offspring."),
  VALUE(MIN_RAD_VAL,           double,    4.0,      "Minimum distance a radius can be for a given program"),
  VALUE(MAX_RAD_VAL,           double,    8.0,      "Maximum distance a radius can be for a given program"),
  
  GROUP(ENVIRONMENT, "How do environmental interactions work?"),
  VALUE(MIN_CONSUME_RATIO,     double,    0.80,     "A predator cannot consume anything propotionately smaller than this relative to its size."),
  VALUE(MAX_CONSUME_RATIO,     double,    0.0,      "A predator cannot consume anything propotionately larger than this."),
  VALUE(MAX_SPEED_RATIO,       double,    1.0,      "Maximum speed that an organism can reach, based on size."),
  VALUE(ENERGY_REDUCTION,      double,    1.0,      "Organism energy reduction per update call."),
  VALUE(REPRODUCTION_PENALTY,  double,    2.0,      "Energy of org is divided by this value"),
  VALUE(CONSUME_RES_THRESH,    double,    0.5,      "Consume resource if radius <= (MAX_CONSUME_RATIO-MIN_CONSUME_RATIO)*CONSUME_RES_THRESH+MIN_CONSUME_RATIO"),
  VALUE(EAT_ORG_ENERGRY_PROP,  double,    0.05,     "Proportion of energry gained from eating a prey org!"),

  GROUP(MUTATIONS, "Various mutation rates for SignalGP Brains"),
  VALUE(POINT_MUTATE_PROB,     double,    0.001,    "Probability of instructions being mutated"),
  VALUE(BIT_FLIP_PROB,         double,    0.00001,  "Probability of each tag bit toggling"),
  VALUE(ARG_SUB__PER_ARG,      double,    0.001,    "Rate of argument substitution mutations (per arg)."),
  VALUE(INST_SUB__PER_INST,    double,    0.001,    "Rate of instruction substitution mutations (per inst)."),
  VALUE(INST_INS__PER_INST,    double,    0.001,    "Rate of instruction insertion mutations (per inst)."),
  VALUE(INST_DEL__PER_INST,    double,    0.001,    "Rate of instruction deletion mutations (per inst)."),
  VALUE(SLIP__PER_FUNC,        double,    0.001,    "Rate of mulit-instruction duplication of deletion mutations (per function)."),
  VALUE(FUNC_DUP__PER_FUNC,    double,    0.001,    "Rate of whole function duplication mutations (per function)."),
  VALUE(FUNC_DEL__PER_FUNC,    double,    0.001,    "Rate of whole function deletion mutations (per function)."),
  VALUE(TAG_BIT_FLIP__PER_BIT, double,    0.001,    "Rate of tag bit-flip mutations (per bit)."),
  VALUE(RADIUS_MUT,            double,    0.001,    "Rate of tag bit-flip mutations (per bit)."),

  GROUP(PROGRAM, "Various configuration options for SignalGP programs."),
  VALUE(PROGRAM_MIN_FUN_CNT,   size_t,    1,       "Minimum number of functions in a SignalGP program."),
  VALUE(PROGRAM_MAX_FUN_CNT,   size_t,    8,       "Maximum number of functions in a SignalGP program."),
  VALUE(PROGRAM_MIN_FUN_LEN,   size_t,    24,      "Minimum number of instructions in a SignalGP function."),
  VALUE(PROGRAM_MAX_FUN_LEN,   size_t,    32,      "Maximum number of instructions in a SignalGP function."),
  VALUE(PROGRAM_MIN_ARG_VAL,   int,       0,       "Minimum argument value in a SignalGP program instruction."),
  VALUE(PROGRAM_MAX_ARG_VAL,   int,       16,      "Maximum argument value in a SignalGP program instruction."),

  GROUP(OUTPUT, "Output rates for BeakerWorld"),
  VALUE(PRINT_INTERVAL,         size_t,     100,      "How many updates between prints?"),
  VALUE(TESTING,                bool,       true,     "Are we testing/debugging?")
)

#endif
