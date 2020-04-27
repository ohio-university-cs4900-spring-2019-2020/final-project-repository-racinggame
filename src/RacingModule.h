#pragma once

#include "GLView.h"
#include "AftrAPI.h"
#include "GameModuleAPI.h"
#include "ISoundManager.h"
#include "PxPhysicsAPI.h"
#include <irrKlang.h>

namespace Aftr {
    class Camera;
    class NetMessengerClient;
    class IPhysicsManager;

    class RacingModule : public GLView {
        public:
            static RacingModule* New(const std::vector< std::string >& outArgs);
            virtual ~RacingModule() { ISoundManager::drop(); IPhysicsManager::drop(); };
            virtual void init(float gravityScalar, Vector gravityNormalizedVector, std::string confFileName, const PHYSICS_ENGINE_TYPE& physicsEngineType);
            virtual void updateWorld(); ///< Called once per frame
            virtual void loadMap(); ///< Called once at startup to build this module's scene
            virtual void makeCheckpoints();
            virtual void onResizeWindow(GLsizei width, GLsizei height) { GLView::onResizeWindow(width, height); };
            virtual void onMouseDown(const SDL_MouseButtonEvent& e);
            virtual void onMouseUp(const SDL_MouseButtonEvent& e);
            virtual void onMouseMove(const SDL_MouseMotionEvent& e);
            virtual void onMouseWheelScroll(const SDL_MouseWheelEvent& e);
            virtual void onKeyDown(const SDL_KeyboardEvent& key);
            virtual void onKeyUp(const SDL_KeyboardEvent& key);
            virtual int handleWindowEvent(SDL_WindowEvent& e);
            virtual int handleEvent(SDL_Event& sdlEvent);

            void processKeyPress(const SDL_Keycode& key);

            void removeWO(WO* wo);
            void toggleCar();
            void toggleLaps(bool toggle);
            void checkpointReached(int checkpoint);

            int getIndex(WO* wo);
            int getIndexFromLabel(std::string label);
            WO* getFromLabel(std::string label);
            Camera* getCam() { return camera->getCamera(); };

            Racetrack* racetrack;
            WOCar* car;
            GUI* laps;
            PauseMenu* pauseMenu;

        protected:
            RacingModule(const std::vector<std::string>& args);
            virtual void onCreate();
            bool isDriving();
            bool isMovementKey(SDL_Keycode key);

            int lapNumber;
            int currentCheckpoint;

            Checkpoint* finishLine;
            Checkpoint* checkpoint1;
            Checkpoint* checkpoint2;
            Checkpoint* checkpoint3;
            
            Cam* camera;
            NetMessengerClient* client;
            std::set<SDL_Keycode> keysPressed;
    };
}

