#include "WOCar.h"

#include <complex> 
#include "ISoundManager.h"
#include "PxPhysicsAPI.h"

#ifdef AFTR_CONFIG_USE_BOOST

using namespace Aftr;
using namespace physx;

WOMacroDeclaration(WOCar, WO);

WOCar* WOCar::New() {
	WOCar* wo = new WOCar();
	wo->onCreate("../mm/models/warthog/warthog.obj", Vector(1.f, 1.f, 1.f), MESH_SHADING_TYPE::mstSMOOTH);
	return wo;
}

WOCar::WOCar() : IFace(this), WO() {
	this->driver = nullptr;
	this->actor = nullptr;
	this->drivingSound = ISoundManager::engine->play3D("../mm/sounds/warthog/drive.wav", irrklang::vec3df(0.f, 0.f, 0.f), true, true, true);
	this->drivingSound->setMinDistance(30);
	this->horn = ISoundManager::engine->play3D("../mm/sounds/warthog/horn.wav", irrklang::vec3df(0.f, 0.f, 0.f), true, true, true);
	this->horn->setMinDistance(30);
	this->honking = false;
}

void WOCar::onCreate(const std::string& modelFileName, Vector scale, MESH_SHADING_TYPE shadingType) {
	WO::onCreate(modelFileName, scale, shadingType);
	this->setLabel("Car");
	this->actor = IPhysicsManager::createConvexMesh(this);
	this->setToCheckpoint(0);
}

void WOCar::setDriver(Camera* driver) {
	this->driver = driver;
	if (driver != nullptr) {
		this->driver->setParentWorldObject(this);
	} else {
		this->keysPressed.clear();
		if (this->drivingSound != nullptr && !this->drivingSound->getIsPaused()) {
			this->drivingSound->setIsPaused(true);
		}
		if (this->horn != nullptr && !this->horn->getIsPaused()) {
			this->horn->setIsPaused(true);
		}
	}
}

void WOCar::setPosition(Vector position) {
	this->actor->setGlobalPose(PxTransform(IPhysicsManager::toPxVec3(position), this->actor->getGlobalPose().q));
	WO::setPosition(position);
}

void WOCar::setToCheckpoint(int checkpoint) {
	this->actor->setLinearVelocity(PxVec3());
	this->actor->setAngularVelocity(PxVec3());
	if (checkpoint == 0) {
		this->actor->setGlobalPose(PxTransform(PxVec3(-27.673f, -36.363f, -21.8f), PxQuat(0.00943527f, -0.00189444f, 0.858966f, 0.512028f)));
	}
	if (checkpoint == 1) {
		this->actor->setGlobalPose(PxTransform(PxVec3(-1.3f, -217.3f, -13.95f), PxQuat(-0.0005f, -0.03f, -0.008f, 1.f)));
	}
	if (checkpoint == 2) {
		this->actor->setGlobalPose(PxTransform(PxVec3(-5.4f, 15.f, -9.6f), PxQuat(-0.0009f, -0.0005f, -0.82f, 0.58f)));
	}
	if (checkpoint == 3) {
		this->actor->setGlobalPose(PxTransform(PxVec3(6.16f, 190.1f, -10.1f), PxQuat(0.005f, 0.024f, 0.14f, 0.99f)));
	}
}

void WOCar::update() {
	for (SDL_Keycode key : this->keysPressed) {
		switch (key) {
			// Move Forward
			case SDLK_UP:
			case SDLK_w: this->accelerate(); break;
			// Turn Left
			case SDLK_LEFT:
			case SDLK_a: this->actor->setAngularVelocity(PxVec3(0.f, 0.f, -1.f)); break;
			// Move Backwards
			case SDLK_DOWN:
			case SDLK_s: this->reverse(); break;
			// Turn Right
			case SDLK_RIGHT:
			case SDLK_d: this->actor->setAngularVelocity(PxVec3(0, 0, 1)); break;
			// Engage Brake
			case SDLK_LSHIFT: this->brake(); break;
		}
	}
	if (this->drivingSound != nullptr) {
		if (this->isMoving()) {
			if (this->drivingSound->getIsPaused()) {
				this->drivingSound->setIsPaused(false);
			}
		} else {
			if (!this->drivingSound->getIsPaused()) {
				this->drivingSound->setIsPaused(true);
			}
		}
		this->drivingSound->setPosition(ISoundManager::toVec3df(this->getPosition()));
	}
	if (this->horn != nullptr) {
		this->horn->setPosition(ISoundManager::toVec3df(this->getPosition()));
	}
}

void WOCar::brake(){
	this->actor->addForce(this->actor->getLinearVelocity() * -1, PxForceMode::eACCELERATION);
}

void WOCar::accelerate(){
	this->actor->addForce(IPhysicsManager::toPxVec3(this->getDisplayMatrix().getX() * 25), PxForceMode::eACCELERATION);
}

void WOCar::reverse() {
	this->actor->addForce(IPhysicsManager::toPxVec3(this->getDisplayMatrix().getX() * -25), PxForceMode::eACCELERATION);
}

void WOCar::pause(bool pause) {
	if (pause) {
		if (this->horn != nullptr && !this->horn->getIsPaused()) {
			this->horn->setIsPaused(true);
		}
		this->keysPressed.clear();
	} 
}

void WOCar::fromNetMsg(NetMsgWO* netMsg) {
	this->setPosition(netMsg->pos);
	this->setDisplayMatrix(netMsg->pose);
}

void WOCar::onKeyDown(const SDL_KeyboardEvent& key) {
	SDL_Keycode keyDown = key.keysym.sym;
	if (isMovementKey(keyDown)) {
		std::set<SDL_Keycode>::iterator found = keysPressed.find(keyDown);
		if (found == keysPressed.end()) {
			keysPressed.insert(keyDown);
		}
	}
	if (keyDown == SDLK_SPACE && this->horn != nullptr) {
		if (!this->honking) {
			this->horn->setIsPaused(false);
			this->honking = true;
		}
	}
	if (keyDown == SDLK_l) {
		PxQuat quat = this->actor->getGlobalPose().q;
		std::cout << "(" << quat.x << ", " << quat.y << ", " << quat.z << ", " << quat.w << ")" << std::endl;
		std::cout << this->getPosition() << std::endl;
	}
}

void WOCar::onKeyUp(const SDL_KeyboardEvent& key) {
	SDL_Keycode keyUp = key.keysym.sym;
	if (isMovementKey(keyUp)) {
		if (keyUp == SDLK_a || keyUp == SDLK_d) {
			this->actor->setAngularVelocity(PxVec3(0, 0, 0));
		}
		std::set<SDL_Keycode>::iterator found = keysPressed.find(keyUp);
		if (found != keysPressed.end()) {
			keysPressed.erase(found);
		}
	}
	if (keyUp == SDLK_SPACE && this->horn != nullptr) {
		if (honking) {
			this->horn->setIsPaused(true);
			honking = false;
		}
	}
}

bool WOCar::isMovementKey(SDL_Keycode key) {
	return	key == SDLK_UP || key == SDLK_LEFT || 
			key == SDLK_DOWN || key == SDLK_RIGHT ||
			key == SDLK_w || key == SDLK_a || 
			key == SDLK_s || key == SDLK_d || 
			key == SDLK_LSHIFT;
}

bool WOCar::isMoving() {
	return this->keysPressed.find(SDLK_UP) != this->keysPressed.end()
		|| this->keysPressed.find(SDLK_w) != this->keysPressed.end()
		|| this->keysPressed.find(SDLK_DOWN) != this->keysPressed.end()
		|| this->keysPressed.find(SDLK_s) != this->keysPressed.end();
}

#endif