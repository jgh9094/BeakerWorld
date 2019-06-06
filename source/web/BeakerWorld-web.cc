#include "../WebInterface.h"
#include "../BeakerWorld.h"
#include "../BeakerResource.h"
#include "../BeakerOrg.h"

#include <emscripten.h>

WebInterface webi;

// using namespace emscripten;

extern "C"
{
  int GetBlue() {return webi.GetBlue();} 
}
// int GetBlue() {return webi.GetBlue();}     ///< Functions dedicated to returning population distributions

int main(int argc, char* argv[])
{
  webi.Redraw();
  return 0;
}