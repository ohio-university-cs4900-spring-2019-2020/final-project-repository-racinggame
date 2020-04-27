#include "Checkpoint.h"

#include <vector>
#include <iostream>
#include "RacingModule.h"

using namespace Aftr;

Checkpoint* Checkpoint::New(const WayPointParametersBase& params, float radius, int num){
    Checkpoint* ptr = new Checkpoint(params, radius, num);
    ptr->onCreate();
    ptr->setLabel("Waypoint 1");
    return ptr;
}

Checkpoint::Checkpoint(const WayPointParametersBase& params, float radius, int num) : WOWayPointSpherical(params, radius), IFace(this) {
    this->checkpointNum = num;
}

Checkpoint::~Checkpoint() {}

void Checkpoint::onTrigger(){
    if (this->checkpointNum == 0) {
        std::cout << "Finish Line Checkpoint Triggered!" << std::endl << std::endl;
    } else {
        std::cout << "Checkpoint #" << this->checkpointNum << " Triggered!" << std::endl << std::endl;
    }
    ((RacingModule*) ManagerGLView::getGLView())->checkpointReached(this->checkpointNum);
} 