#include "WOPhysx.h"

#ifdef AFTR_CONFIG_USE_BOOST

using namespace Aftr;
using namespace physx;

WOPhysx* WOPhysx::New(const std::string& modelFileName, Vector scale, MESH_SHADING_TYPE shadingType) {
	WOPhysx* wo = new WOPhysx();
	wo->onCreate(modelFileName, scale, shadingType);
	return wo;
}

WOPhysx::WOPhysx() : IFace(this), WO() {
    this->actor = nullptr;
}

void WOPhysx::onCreate(const std::string& modelFileName, Vector scale, MESH_SHADING_TYPE shadingType) {
	WO::onCreate(modelFileName, scale, shadingType);
    this->actor = IPhysicsManager::createConvexMesh(this);
}

void WOPhysx::toPhysx(NetMessengerClient* client) {
    PxMat44 pose = PxMat44(this->actor->getGlobalPose()); // Get the physx pose matrix from actor's transform
    float convert[16] = {   // Collect the new pose into an aftr-acceptable state
        pose(0, 0), pose(0, 1), pose(0, 2), pose(3, 0),
        pose(1, 0), pose(1, 1), pose(1, 2), pose(3, 1),
        pose(2, 0), pose(2, 1), pose(2, 2), pose(3, 2),
        pose(0, 3), pose(1, 3), pose(2, 3), pose(3, 3)
    };
    
    Mat4 mat(convert);
    // Send orientation net message to update an item's orientation
    if (client != nullptr && client->isTCPSocketOpen()) {
        NetMsgWO msg;
        msg.pos = Vector(mat[12], mat[13], mat[14]);
        msg.pose = mat;
       // msg.index = worldObjects[this];
        client->sendNetMsgSynchronousTCP(msg);
    }
    // Apply the new pose
    this->setDisplayMatrix(mat);
    this->setPosition(mat[12], mat[13], mat[14]);
}

void WOPhysx::setPosition(Vector pos) {
    WO::setPosition(pos);
    if (this->actor != nullptr) {
        //PxMat44 mat = IPhysicsManager::mat4ToMat44(this->getDisplayMatrix());
        //mat.setPosition(IPhysicsManager::toPxVec3(pos));
        //this->actor->setGlobalPose(PxTransform(mat));
    }
}

void WOPhysx::setPosition(float x, float y, float z) {
    this->setPosition(Vector(x, y, z));
}

#endif