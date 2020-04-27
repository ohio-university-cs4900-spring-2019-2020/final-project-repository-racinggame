#include "RacingModule.h"

#include "WorldList.h" //This is where we place all of our WOs
#include "ManagerOpenGLState.h" //We can change OpenGL State attributes with this
#include "Axes.h" //We can set Axes to on/off with this
#include "PhysicsEngineODE.h"
#include "AftrGLRendererBase.h"
#include "NetMessengerServer.h"

using namespace Aftr;
using namespace irrklang;
using namespace physx;

const std::string sharedMM = ManagerEnvironmentConfiguration::getSMM();

RacingModule* RacingModule::New(const std::vector<std::string>& args) {
    RacingModule* rm = new RacingModule(args);
    rm->init(Aftr::GRAVITY, Vector(0.0f, 0.0f, -1.0f), "aftr.conf", PHYSICS_ENGINE_TYPE::petODE);
    rm->onCreate();
    return rm;
}

RacingModule::RacingModule(const std::vector<std::string>& args) : GLView(args) {
    this->car = nullptr;
    this->racetrack = nullptr;
    this->client = nullptr;
    this->pauseMenu = nullptr;
    this->camera = nullptr;
    this->laps = nullptr;
    this->lapNumber = 1;
    this->finishLine = nullptr;
    this->checkpoint1 = nullptr;
    this->checkpoint2 = nullptr;
    this->checkpoint3 = nullptr;
    this->currentCheckpoint = 0;
}

void RacingModule::init(float gravityScalar, Vector gravityNormalizedVector, std::string configFileName, const PHYSICS_ENGINE_TYPE& physicsEngineType) {
    // setup sound and physics managers
    ISoundManager::init();
    IPhysicsManager::init();
    GLView::init(gravityScalar, gravityNormalizedVector, configFileName, physicsEngineType);
}

void RacingModule::onCreate(){
    //GLViewModule::onCreate() is invoked after this module's LoadMap() is completed.
    if( this->pe != NULL ){
        //optionally, change gravity direction and magnitude here. The user could load these values from the module's aftr.conf
        this->pe->setGravityNormalizedVector(Vector(0.f, 0.f, -1.f));
        this->pe->setGravityScalar(GRAVITY); 
    }
    this->setActorChaseType(CHASEACTORLOOK4); //Default is STANDARDEZNAV mode
    // setup Cam
    this->camera = Cam::New(this->cam);
    //this->worldLst->push_back(this->camera->getLocation());

    this->racetrack = Racetrack::New();
    this->worldLst->push_back(this->racetrack);

    // make pause menu
    this->pauseMenu = PauseMenu::New();
    this->worldLst->push_back(this->pauseMenu->exit);
    this->worldLst->push_back(this->pauseMenu->resume);

    // make laps banner
    this->laps = GUI::New(nullptr, 0.3f, 0.1f, "../mm/images/banner.png");
    this->laps->setPosition(Vector(0.5f, 0.9f, 0.f));
    this->laps->setTextFont(sharedMM + "/fonts/arial.ttf");
    this->laps->setText("Lap 1");
    this->laps->setLabel("Laps");
    this->laps->getGUILabel()->setFontSize(24.f);
    this->laps->isFocusable(false);
    this->laps->receivesEvents(false);
    this->worldLst->push_back(this->laps);

    // add car to world
    this->car = WOCar::New();
    this->worldLst->push_back(this->car);
    this->car->setDriver(this->cam);
    this->cam->setActorToWatch(this->car);

    // setup checkpoints 
    this->makeCheckpoints();
    this->car->setToCheckpoint(0);

    // setup messenger client
    std::string port = ManagerEnvironmentConfiguration::getVariableValue("NetServerListenPort");
    this->client = port == "12683" ? NetMessengerClient::New("127.0.0.1", "12682") : NetMessengerClient::New("127.0.0.1", "12683");
}

void RacingModule::updateWorld(){
    if (!this->pauseMenu->isPaused()) {
        GLView::updateWorld();
        ISoundManager::setListenerPosition(this->cam->getPosition(), this->cam->getLookDirection(), Vector(0.f, 0.f, 0.f), this->cam->getNormalDirection());
        // update warthog
        if (this->isDriving()) {
            this->car->update();
            if (this->client->isTCPSocketOpen()) {
                NetMsgWO netMsg = NetMsgWO::makeNetMsgWO(this->car->getPosition(), this->car->getDisplayMatrix(), this->getIndex(this->car));
                this->client->sendNetMsgSynchronousTCP(netMsg);
            }
        }
        this->camera->update();
        // update physics
        IPhysicsManager::simulate(this->client);
    }
}

void RacingModule::removeWO(WO* wo) {
    this->worldLst->eraseViaWOptr(wo);
}

void RacingModule::toggleCar() {
    if (this->car == nullptr) {
        this->car = WOCar::New();
        this->worldLst->push_back(this->car);
        this->car->setDriver(this->cam);
        this->finishLine->getActivators()->push_back(this->car);
        this->checkpoint1->getActivators()->push_back(this->car);
        this->checkpoint2->getActivators()->push_back(this->car);
        this->checkpoint3->getActivators()->push_back(this->car);
    } else {
        this->car->setDriver(nullptr);
        this->car->actor->release();
        this->removeWO(this->car);
        this->car = nullptr;
    }
}

void RacingModule::toggleLaps(bool toggle) {
    if (toggle) {
        this->laps->setWidthGUI(0.3f);
        this->laps->setText("Lap " + std::to_string(this->lapNumber));
    } else {
        this->laps->setWidthGUI(0.0f);
        this->laps->setText("");
    }
}

void RacingModule::onKeyDown(const SDL_KeyboardEvent& key){
    GLView::onKeyDown(key);
    if (this->isDriving()) this->car->onKeyDown(key);
    else if (this->isMovementKey(key.keysym.sym)) this->camera->onKeyDown(key);
    this->processKeyPress(key.keysym.sym);
}

void RacingModule::onKeyUp(const SDL_KeyboardEvent& key) {
    GLView::onKeyUp(key);
    if (this->isDriving()) this->car->onKeyUp(key);
    if (this->isMovementKey(key.keysym.sym)) this->camera->onKeyUp(key);
}

void RacingModule::onMouseDown(const SDL_MouseButtonEvent& e) {
    GLView::onMouseDown(e);
    if (this->pauseMenu->isPaused()) {
        if (this->pauseMenu->exit->pixelResidesInBoundingRect(e.x, e.y)) {
            exit(1);
        }
        if (this->pauseMenu->resume->pixelResidesInBoundingRect(e.x, e.y)) {
            this->pauseMenu->resumeGame();
            if (this->client != nullptr && this->client->isTCPSocketOpen()) {
                NetMsgPause netMsgPause;
                netMsgPause.paused = false;
                this->client->sendNetMsgSynchronousTCP(netMsgPause);
            }
            if (this->isDriving()) {
                this->car->pause(false);
            }
        }
    }
    if (this->isDriving()) { this->car->onMouseDown(e); }
}

void RacingModule::onMouseUp(const SDL_MouseButtonEvent& e) {
    GLView::onMouseUp(e);
    if (this->isDriving()) { this->car->onMouseUp(e); }
}

void RacingModule::onMouseMove(const SDL_MouseMotionEvent& e) {
    GLView::onMouseMove(e);
    if (this->isDriving()) { this->car->onMouseMove(e); }
}

int RacingModule::handleWindowEvent(SDL_WindowEvent& e) {
    return GLView::handleWindowEvent(e);
}

int RacingModule::handleEvent(SDL_Event& sdlEvent) {
    Uint32 type = sdlEvent.type;
    if (type == SDL_KEYDOWN && sdlEvent.key.keysym.sym == SDLK_ESCAPE) {
        NetMsgPause netMsgPause;
        if (this->pauseMenu->isPaused()) {
            this->pauseMenu->resumeGame();
            netMsgPause.paused = false;
        } else {
            this->pauseMenu->pauseGame();
            netMsgPause.paused = true;
        }
        if (this->client != nullptr && this->client->isTCPSocketOpen()) {
            this->client->sendNetMsgSynchronousTCP(netMsgPause);
        }
        if (this->isDriving()) {
            this->car->pause(true);
        }
        this->camera->clear();
        return 1;
    }
    if(this->pauseMenu->isPaused()){
        if (type == SDL_QUIT || type == 256) {
            return 0;
        } else if (type != SDL_MOUSEBUTTONDOWN) {
            return 1;
        } 
    }
    return GLView::handleEvent(sdlEvent);
}

void RacingModule::onMouseWheelScroll(const SDL_MouseWheelEvent& e) {
    GLView::onMouseWheelScroll(e);
}

void RacingModule::loadMap(){
    this->worldLst = new WorldList(); //WorldList is a 'smart' vector that is used to store WO*'s
    this->actorLst = new WorldList();
    this->netLst = new WorldList();

    ManagerOpenGLState::GL_CLIPPING_PLANE = 10000.f;
    ManagerOpenGLState::GL_NEAR_PLANE = 0.1f;
    ManagerOpenGLState::enableFrustumCulling = false;
    Axes::isVisible = false;
    this->glRenderer->isUsingShadowMapping(false); //set to TRUE to enable shadow mapping, must be using GL 3.2+
    this->cam->setPosition(0.f, 0.f, 0.f);  
    
    float ga = 0.1f; //Global Ambient Light level for this module
    ManagerLight::setGlobalAmbientLight(aftrColor4f(ga, ga, ga, 1.f));
    WOLight* light = WOLight::New();
    light->isDirectionalLight(true);
    light->setPosition(Vector(0.f, 0.f, 100.f));
    //Set the light's display matrix such that it casts light in a direction parallel to the -z axis (ie, downwards as though it was "high noon")
    //for shadow mapping to work, this->glRenderer->isUsingShadowMapping(true), must be invoked.
    light->getModel()->setDisplayMatrix(Mat4::rotateIdentityMat({0.f, 1.f, 0.f}, 90.f * Aftr::DEGtoRAD));
    light->setLabel("Light");
    worldLst->push_back(light);
    
    //Create the SkyBox
    WO* wo = WOSkyBox::New(sharedMM + "/images/skyboxes/sky_mountains+6.jpg", this->getCameraPtrPtr());
    wo->setPosition(Vector(0.f, 0.f, 0.f));
    wo->setLabel("Sky Box");
    wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
    worldLst->push_back(wo);
}

void RacingModule::makeCheckpoints() {
    std::vector<WO*> activators;
    activators.push_back(this->car);
    WayPointParametersBase params(this, activators, wpatREGULAR, 5000.f);
    // make checkpoints
    this->finishLine = Checkpoint::New(params, 15.f, 0);
    this->checkpoint1 = Checkpoint::New(params, 15.f, 1);
    this->checkpoint2 = Checkpoint::New(params, 15.f, 2);
    this->checkpoint3 = Checkpoint::New(params, 15.f, 3);
    // set their positions on the track
    this->finishLine->setPosition(Vector(-43.4f, -65.6f, -21.f));
    this->checkpoint1->setPosition(Vector(0.f, -215.f, -11.f));
    this->checkpoint2->setPosition(Vector(-3.f, -11.f, -7.f));
    this->checkpoint3->setPosition(Vector(22.6f, 184.4f, -9.f));
    // add them to the world
    worldLst->push_back(this->finishLine);
    worldLst->push_back(this->checkpoint1);
    worldLst->push_back(this->checkpoint2);
    worldLst->push_back(this->checkpoint3);
}

void RacingModule::checkpointReached(int checkpoint) {
    if (checkpoint == 0 && this->currentCheckpoint == 3) {
        this->currentCheckpoint = 0;
        ++this->lapNumber;
        this->laps->setText("Lap " + std::to_string(this->lapNumber));
    } else if (checkpoint == (this->currentCheckpoint + 1)) {
        ++this->currentCheckpoint;
    }
}

int RacingModule::getIndex(WO* wo) { return this->worldLst->getIndexOfWO(wo); }

void RacingModule::processKeyPress(const SDL_Keycode& key) {
    //if (key == SDLK_f) this->toggleCar();
    if (key == SDLK_r && this->isDriving()) this->car->setToCheckpoint(this->currentCheckpoint);
}

bool RacingModule::isDriving() { return this->car != nullptr && this->car->hasDriver(); }

WO* RacingModule::getFromLabel(std::string label) {
    for (int i = 0; i < this->worldLst->size(); ++i) {
        WO* wo = this->worldLst->at(i);
        if (wo != nullptr && wo->getLabel() == label) {
            return wo;
        }
    }
    return nullptr;
}

int RacingModule::getIndexFromLabel(std::string label) {
    for (int i = 0; i < this->worldLst->size(); ++i) {
        WO* wo = this->worldLst->at(i);
        if (wo != nullptr && wo->getLabel() == label) {
            return i;
        }
    }
    return -1;
}

bool RacingModule::isMovementKey(SDL_Keycode key) {
    return (key == SDLK_UP || key == SDLK_LEFT || key == SDLK_DOWN || key == SDLK_RIGHT ||
        key == SDLK_w || key == SDLK_a || key == SDLK_s || key == SDLK_d || key == SDLK_LSHIFT);
}
