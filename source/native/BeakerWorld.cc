///< C++ includes
#include <iostream>
#include <fstream>

///<  Empirical inlcudes
#include "base/vector.h"
#include "config/command_line.h"
#include "config/ArgManager.h"

///< Experiemnt includes
#include "../config.h"
#include "../BeakerWorld.h"

int main(int argc, char* argv[])
{
  BeakerWorldConfig config;
  config.Read("BeakerWorld.cfg", false);
  auto args = emp::cl::ArgManager(argc, argv);
  if (args.ProcessConfigOptions(config, std::cout, "BeakerWorld.cfg", "BeakerWorld-macros.h") == false) exit(0);
  if (args.TestUnknown() == false) exit(0);  // If there are leftover args, throw an error.

  std::ofstream file; 
  file.open("timeseries.csv");
  file << "updates,primary,apex" << std::endl;

  BeakerWorld world(config);

  for (size_t ud = 0; ud < config.MAX_UPS(); ud++) {
    world.Update();
    std::cout <<  "***********************************\n" << std::endl;
    std::cout << "UD: " << ud
              << "  NumOrgs=" << world.GetNumOrgs()
              << "  ID_MAP=" << world.GetIDSize()
              << "  LimeOrgs=" << world.GetLime()
              << "  RedOrgs=" << world.GetRed()
              << std::endl;

    file << world.GetUpdate() << "," << world.GetLime() << "," << world.GetRed() << std::endl;
  }
}
