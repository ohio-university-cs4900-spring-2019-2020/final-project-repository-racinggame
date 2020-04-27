#pragma once

#include <irrKlang.h>
#include "IPhysicsManager.h"
#include "PxPhysicsAPI.h"
#include "NetMsgWO.h"
#include "AftrAPI.h"

#ifdef AFTR_CONFIG_USE_BOOST

namespace Aftr {
	class NetMessengerClient;
	class IPhysicsManager;

	class WOCar : public WO {
		public:
			WOMacroDeclaration(WOCar, WO);

			static WOCar* New();
			virtual ~WOCar() {};
			virtual void onCreate(
				const std::string& modelFileName, 
				Vector scale = Vector(1, 1, 1), 
				MESH_SHADING_TYPE shadingType = MESH_SHADING_TYPE::mstAUTO
			);

			// User Keyboard Input Specific
			virtual void onKeyDown(const SDL_KeyboardEvent& key);
			virtual void onKeyUp(const SDL_KeyboardEvent& key);
			virtual void onMouseUp(const SDL_MouseButtonEvent& e) {};
			virtual void onMouseMove(const SDL_MouseMotionEvent& e) {};

			virtual void setPosition(Vector position);
			virtual void setPosition(float x, float y, float z) { setPosition(Vector(x, y, z)); };
			virtual Model* getModel() { return WO::getModel(); }
			virtual Mat4 getDisplayMatrix() { return WO::getDisplayMatrix(); }
			virtual void setDisplayMatrix(Mat4 mat) { getModel()->setDisplayMatrix(mat); }
			virtual void setLookDirection(Vector dir) { getModel()->setLookDirection(dir); }

			// Proccess the keys pressed
			void update();
			void pause(bool pause);
			void fromNetMsg(NetMsgWO* netMsg);
			void setToCheckpoint(int checkpoint);

			void setDriver(Camera* newDriver);
			Camera* getDriver() { return driver; }
			bool hasDriver() { return driver != nullptr; };
			bool isMoving();
			
			physx::PxRigidDynamic* actor;

		protected:
			WOCar();

			Camera* driver;
			std::set<SDL_Keycode> keysPressed;
			irrklang::ISound* drivingSound;
			irrklang::ISound* horn;
			bool honking;

			bool isMovementKey(SDL_Keycode key);
			void brake();
			void accelerate();
			void reverse();
	};

}

#endif

