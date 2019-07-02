#ifndef BEAKER_WORLD_CONFIG_H
#define BEAKER_WORLD_CONFIG_H

#include "config/config.h"

EMP_BUILD_CONFIG( BeakerWorldConfig,
  GROUP(WORLD_STRUCTURE, "How should each organism's genome be setup?"),
  VALUE(WORLD_X,        double, 680.0, "How wide is the World?"),
  VALUE(WORLD_Y,        double, 480.0, "How tall is the World?"),
  VALUE(INIT_POP_SIZE,  size_t, 300, "How many organisms should we start with?"),  
  VALUE(MAX_POP_SIZE,   size_t, 1000, "What are the most organisms that should be allowed in pop?"),
  VALUE(MAX_GENS,       size_t, 10000, "How many generations should the runs go for?"),
  VALUE(SEED,           int,    1, "Random number seed (0 for based on time)"),
  VALUE(HM_SIZE,        size_t, 6, "Size of the heat map."),
		  
  GROUP(ENVIRONMENT, "How do environmental interactions work?"),
  VALUE(MIN_ORG_SIZE,          double, 1.0, "Minimum size for each organism."),
  VALUE(MAX_ORG_SIZE,          double, 10.0, "Maximum size of each organism."),
  VALUE(MIN_CONSUME_RATIO,     double, 0.25, "A predator cannot consume anything propotionately smaller than this."),
  VALUE(MAX_CONSUME_RATIO,     double, 0.0, "A predator cannot consume anything propotionately larger than this."),
  VALUE(INIT_ENERGY,           double, 200.0, "How much energy should organisms start with?"),  
  VALUE(MAX_ENERGY_RATIO,      double, 1.0, "Maximum energy that an organism can carry, based on size."),
  VALUE(MAX_SPEED_RATIO,       double, 1.0, "Maximum speed that an organism can reach, based on size."),
  VALUE(NUM_RESOURCE_SOURCES,  size_t, 200, "How many sources of resouces should there be?"),
  VALUE(RESOURCE_PATCH_SIZE,   double, 15.0, "How large should each resource patch be?"),
  VALUE(PROGRAM_MIN_RAD_VAL,   double, 4.0, "Minimum distance a radius can be for a given program"),
  VALUE(PROGRAM_MAX_RAD_VAL,   double, 8.0, "Maximum distance a radius can be for a given program"),

  GROUP(MUTATIONS, "Various mutation rates for SignalGP Brains"),
  VALUE(POINT_MUTATE_PROB, double, 0.001, "Probability of instructions being mutated"),
  VALUE(BIT_FLIP_PROB,     double, 0.00001, "Probability of each tag bit toggling"),

  GROUP(PROGRAM, "Various configuration options for SignalGP programs."),
  VALUE(PROGRAM_MIN_FUN_CNT,   size_t, 2,  "Minimum number of functions in a SignalGP program."),
  VALUE(PROGRAM_MAX_FUN_CNT,   size_t, 8,  "Maximum number of functions in a SignalGP program."),
  VALUE(PROGRAM_MIN_FUN_LEN,   size_t, 24,  "Minimum number of instructions in a SignalGP function."),
  VALUE(PROGRAM_MAX_FUN_LEN,   size_t, 32, "Maximum number of instructions in a SignalGP function."),
  VALUE(PROGRAM_MIN_ARG_VAL,   int,    0,  "Minimum argument value in a SignalGP program instruction."),
  VALUE(PROGRAM_MAX_ARG_VAL,   int,    16, "Maximum argument value in a SignalGP program instruction."),
  VALUE(ARG_SUB__PER_ARG,      double, 0.01, "Rate of argument substitution mutations (per arg)."),
  VALUE(INST_SUB__PER_INST,    double, 0.01, "Rate of instruction substitution mutations (per inst)."),
  VALUE(INST_INS__PER_INST,    double, 0.01, "Rate of instruction insertion mutations (per inst)."),
  VALUE(INST_DEL__PER_INST,    double, 0.01, "Rate of instruction deletion mutations (per inst)."),
  VALUE(SLIP__PER_FUNC,        double, 0.01,  "Rate of mulit-instruction duplication of deletion mutations (per function)."),
  VALUE(FUNC_DUP__PER_FUNC,    double, 0.01,  "Rate of whole function duplication mutations (per function)."),
  VALUE(FUNC_DEL__PER_FUNC,    double, 0.01,  "Rate of whole function deletion mutations (per function)."),
  VALUE(TAG_BIT_FLIP__PER_BIT, double, 0.005, "Rate of tag bit-flip mutations (per bit)."),
  VALUE(RADIUS_MUT, double, 0.009, "Rate of tag bit-flip mutations (per bit)."),

  GROUP(OUTPUT, "Output rates for BeakerWorld"),
  VALUE(PRINT_INTERVAL, size_t, 100, "How many updates between prints?")
)

#endif
