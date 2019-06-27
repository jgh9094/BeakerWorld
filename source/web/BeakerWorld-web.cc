#include "../WebInterface.h"
#include "../BeakerWorld.h"
#include "../BeakerResource.h"
#include "../BeakerOrg.h"

#include <emscripten.h>

WebInterface webi;


int main(int argc, char* argv[])
{
  webi.Redraw();
  return 0;
}