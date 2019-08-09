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
#include "../ResourceManager.h"

int main(int argc, char* argv[])
{
  BeakerConfig config;
  config.Read("BeakerWorld.cfg", false);
  auto args = emp::cl::ArgManager(argc, argv);
  if (args.ProcessConfigOptions(config, std::cout, "BeakerWorld.cfg", "BeakerWorld-macros.h") == false) exit(0);
  if (args.TestUnknown() == false) exit(0);  // If there are leftover args, throw an error.

  std::cout << "Begining Run" << std::endl;
  for(size_t i = 0; i < config.MAX_UPS(); ++i)
  {
    std::cout << "i=" << i << std::endl;
  }
  std::cout << "Finished Run" << std::endl;

}
