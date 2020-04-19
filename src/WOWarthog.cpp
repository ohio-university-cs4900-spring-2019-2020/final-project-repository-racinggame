#include "WOWarthog.h"

#include <complex> 
#include "ISoundManager.h"
#include "PxPhysicsAPI.h"

#ifdef AFTR_CONFIG_USE_BOOST

using namespace Aftr;
using namespace physx;

WOMacroDeclaration(WOWarthog, WO);

WOWarthog* WOWarthog::New() {
	WOWarthog* wo = new WOWarthog();
	wo->onCreate("../mm/models/warthog/warthog.obj", Vector(1.f, 1.f, 1.f), MESH_SHADING_TYPE::mstSMOOTH);
	return wo;
}

WOWarthog::WOWarthog() : IFace(this), WO() {
	this->driver = nullptr;
	this->actor = nullptr;
	this->drivingSound = ISoundManager::engine->play3D("../mm/sounds/warthog/drive.wav", ISoundManager::toVec3df(Vector(0.f, 0.f, 0.f)), true, true, true);
	this->drivingSound->setMinDistance(30);
	this->horn = ISoundManager::engine->play3D("../mm/sounds/warthog/horn.wav", ISoundManager::toVec3df(Vector(0.f, 0.f, 0.f)), true, true, true);
	this->horn->setMinDistance(30);
	this->honking = false;
	this->speedometer = WOGUILabel::New(nullptr);
	this->textLabel = WOText::New(ManagerEnvironmentConfiguration::getSMM() + "/fonts/arial.ttf", 24, "Warthog");
}

void WOWarthog::onCreate(const std::string& modelFileName, Vector scale, MESH_SHADING_TYPE shadingType) {
	WO::onCreate(modelFileName, scale, shadingType);
	this->setLabel("Warthog");
	this->textLabel->setLabel("Warthog Label");
	this->speedometer->setLabel("Warthog Speedometer");
	this->speedometer->setText("0");
	this->speedometer->setColor(255, 0, 0, 255);
	this->speedometer->setFontSize(30.f); //font size is correlated with world size
	this->speedometer->setPosition(Vector(0.03f, 0.02f, 0.f));
	this->speedometer->setFontPath(ManagerEnvironmentConfiguration::getSMM() + "/fonts/arial.ttf");

	float mat[16] = {
		0.f, -1.f, 0.f, 0.f,
		0.f,  0.f, 1.f, 0.f,
		-1.f, 0.f, 0.f, 0.f,
		0.f,  0.f, 0.f, 1.f
	};
	this->textLabel->setDisplayMatrix(Mat4(mat));
	this->textLabel->setScale(Vector(10.f, 10.f, 10.f));
	this->actor = IPhysicsManager::createConvexMesh(this);
}

void WOWarthog::setDriver(Camera* driver) {
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

void WOWarthog::onMouseUp(const SDL_MouseButtonEvent& e) {}
void WOWarthog::onMouseMove(const SDL_MouseMotionEvent& e) {}

void WOWarthog::setPosition(Vector position) {
	WO::setPosition(position);
}

void WOWarthog::update() {
	for (SDL_Keycode key : this->keysPressed) {
		switch (key) {
			case SDLK_UP:
			case SDLK_w:
				this->accelerate();
				break;
			case SDLK_LEFT:
			case SDLK_a: 
				this->actor->setAngularVelocity(PxVec3(0.f, 0.f, -1.f));
				break;
			case SDLK_DOWN:
			case SDLK_s:
				this->reverse();
				break;
			case SDLK_RIGHT:
			case SDLK_d:
				this->actor->setAngularVelocity(PxVec3(0, 0, 1));
				break;
			case SDLK_LSHIFT:
				this->brake();
				break;
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
	this->driver->setPosition(this->getPosition() + Vector(-7.75f, 0.f, 3.5)); 
	Mat4 mat = this->getDisplayMatrix();
	
	this->speedometer->setText(std::to_string((int) this->actor->getLinearVelocity().x));
	//this->textLabel->setPosition(this->getPosition() + Vector(0, 0, 10));
}

void WOWarthog::brake(){
	this->actor->addForce(this->actor->getLinearVelocity() * -1, PxForceMode::eACCELERATION);
}

void WOWarthog::accelerate(){
	this->actor->addForce(IPhysicsManager::toPxVec3(this->getDisplayMatrix().getX() * 25), PxForceMode::eACCELERATION);
}

void WOWarthog::reverse() {
	this->actor->addForce(IPhysicsManager::toPxVec3(this->getDisplayMatrix().getX() * -25), PxForceMode::eACCELERATION);
}

void WOWarthog::pauseWarthog(bool pause) {
	if (pause) {
		this->speedometer->setText("");
		if (this->drivingSound != nullptr && !this->drivingSound->getIsPaused()) {
			this->drivingSound->setIsPaused(true);
		}
		if (this->horn != nullptr && !this->horn->getIsPaused()) {
			this->horn->setIsPaused(true);
		}
		this->keysPressed.clear();
	} else {
		this->speedometer->setText(std::to_string((int)this->actor->getLinearVelocity().x));
	}
}

void WOWarthog::fromNetMsg(NetMsgWO* netMsg) {
	this->setPosition(netMsg->pos);
	this->setDisplayMatrix(netMsg->pose);
}

void WOWarthog::onKeyDown(const SDL_KeyboardEvent& key) {
	SDL_Keycode keyDown = key.keysym.sym;
	if (isMovementKey(keyDown)) {
		std::set<SDL_Keycode>::iterator found = keysPressed.find(keyDown);
		if (found == keysPressed.end()) {
			keysPressed.insert(keyDown);
		}
	}
	if (keyDown == SDLK_SPACE && this->horn != nullptr) {
		this->actor->setLinearVelocity(PxVec3(0.0f, 0.0f, 50.0f));
		if (!this->honking) {
			this->horn->setIsPaused(false);
			this->honking = true;
		}
	}
	if (keyDown == SDLK_8) {
		this->actor->setAngularVelocity(PxVec3(0, 0, 20));
	}
}

void WOWarthog::onKeyUp(const SDL_KeyboardEvent& key) {
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

bool WOWarthog::isMovementKey(SDL_Keycode key) {
	return (key == SDLK_UP || key == SDLK_LEFT || key == SDLK_DOWN || key == SDLK_RIGHT ||
		key == SDLK_w || key == SDLK_a || key == SDLK_s || key == SDLK_d || key == SDLK_LSHIFT);
}

bool WOWarthog::isMoving() {
	return this->keysPressed.find(SDLK_UP) != this->keysPressed.end()
		|| this->keysPressed.find(SDLK_w) != this->keysPressed.end()
		|| this->keysPressed.find(SDLK_DOWN) != this->keysPressed.end()
		|| this->keysPressed.find(SDLK_s) != this->keysPressed.end();
}

float WOWarthog::toRads(float deg) { return std::tan((deg * Aftr::PI) / 180); }
float WOWarthog::toDeg(float rad) { return rad * (180 / Aftr::PI); }

void WOWarthog::fromPhysx() {
	if (this->actor != nullptr) {
		PxTransform t = this->actor->getGlobalPose();
		PxMat44 m = PxMat44(t);
		PxVec3 p = t.p;

		Mat4 mat;
		for (unsigned int i = 0; i < 3; ++i) {
			for (unsigned int j = 0; j < 3; ++j) {
				mat[i * 4 + j] = m[i][j];
			}
		}
		this->setDisplayMatrix(mat);
		this->setPosition(p.x, p.y, p.z);
	}
}

void WOWarthog::toPhysx() {
	if (this->actor != nullptr) {
		Mat4 mat = this->getDisplayMatrix();
		Vector p = this->getPosition();

		PxMat44 m;
		for (unsigned int i = 0; i < 3; ++i) {
			for (unsigned int j = 0; j < 3; ++j) {
				m[i][j] = mat[i * 4 + j];
			}
		}
		m[3] = PxVec4(p.x, p.y, p.z, 1.0f);
		this->actor->setGlobalPose(PxTransform(m));
	}
}

void WOWarthog::toPhysxWithPos(Vector p) {
	if (this->actor != nullptr) {
		Mat4 mat = this->getDisplayMatrix();

		PxMat44 m;
		for (unsigned int i = 0; i < 3; ++i) {
			for (unsigned int j = 0; j < 3; ++j) {
				m[i][j] = mat[i * 4 + j];
			}
		}
		m[3] = PxVec4(p.x, p.y, p.z, 1.0f);
		this->actor->setGlobalPose(PxTransform(m));
	}
}

void WOWarthog::updateLabel() {
	this->textLabel->setPosition(this->getPosition() + Vector(0, 0, 7));

	// w[0] = t[8]
	// w[1] = t[9]
	// -w[4] = t[0]
	// -w[5] = t[1]
	Mat4 w = this->getDisplayMatrix();
	float t[16] = {
		-w[4], -w[5], 0, 0,
		0, 0, 1, 0,
		w[0], w[1], 0, 0,
		0, 0, 0, 1
	};
	this->textLabel->setDisplayMatrix(Mat4(t));
}

#endif