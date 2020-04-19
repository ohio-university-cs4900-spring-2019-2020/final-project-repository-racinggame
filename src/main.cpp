//**********************************************************************************
// STEAMiE's Entry Point.
//**********************************************************************************

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include "RacingModule.h" //GLView subclass instantiated to drive this simulation
#include <irrKlang.h>

using namespace std;
using namespace Aftr;

/// Saves the in passed params argc and argv in a vector of strings.
vector<string> saveInputParams(int argc, char** argv);

/**
   This creates a GLView subclass instance and begins the GLView's main loop.
   Each iteration of this loop occurs when a reset request is received. A reset
   request causes the entire GLView to be destroyed (since its exits scope) and
   begin again (simStatus == -1). This loop exits when a request to exit the 
   application is received (simStatus == 0 ).
*/
int main(int argc, char* argv[]) {
   vector<string> args = saveInputParams(argc, argv); ///< Command line arguments passed via argc and argv, reserved to size of argc
   int simStatus = 0;

   do {
        unique_ptr<RacingModule> racingGame(RacingModule::New(args));
        simStatus = racingGame->startWorldSimulationLoop(); // Runs until simulation exits or requests a restart (values 0 or -1, respectively)
   }
   while( simStatus != 0 );

   cout << "Exited AfterBurner Engine Normally..." << endl;
   return 0;
}

vector<string> saveInputParams(int argc, char** argv) {
   vector<string> args(argc);
   for(int i = 0; i < argc; ++i) {
      string arg(argv[i]);
      args[i] = arg;
   }
   return args;
}
