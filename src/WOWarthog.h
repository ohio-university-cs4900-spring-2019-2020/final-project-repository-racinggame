#pragma once

#include <irrKlang.h>
#include "IPhysicsManager.h"
#include "PxPhysicsAPI.h"
#include "WOText.h"
#include "NetMsgWO.h"
#include "AftrAPI.h"

#ifdef AFTR_CONFIG_USE_BOOST

namespace Aftr {
	class NetMessengerClient;
	class IPhysicsManager;

	class WOWarthog : public WO {
		public:
			WOMacroDeclaration(WOWarthog, WO);

			static WOWarthog* New();
			virtual ~WOWarthog() {};
			virtual void onCreate(const std::string& modelFileName, Vector scale = Vector(1, 1, 1), MESH_SHADING_TYPE shadingType = MESH_SHADING_TYPE::mstAUTO);

			//User Keyboard Input Specific
			//-----------------------------------
			virtual void onKeyDown(const SDL_KeyboardEvent& key);
			virtual void onKeyUp(const SDL_KeyboardEvent& key);
			virtual void onMouseUp(const SDL_MouseButtonEvent& e);
			virtual void onMouseMove(const SDL_MouseMotionEvent& e);

			virtual void setPosition(Vector position);
			virtual void setPosition(float x, float y, float z) { setPosition(Vector(x, y, z)); };
			virtual Model* getModel() { return WO::getModel(); }
			Mat4 getTextLabelMatrix() { return textLabel->getDisplayMatrix(); }
			virtual Mat4 getDisplayMatrix() { return WO::getDisplayMatrix(); }
			virtual void setDisplayMatrix(Mat4 mat) { getModel()->setDisplayMatrix(mat); }
			virtual void setLookDirection(Vector dir) { getModel()->setLookDirection(dir); }

			// Proccess the keys pressed
			void update();
			void updateLabel();
			void pauseWarthog(bool pause);
			void fromNetMsg(NetMsgWO* netMsg);
			void setDriver(Camera* newDriver);
			Camera* getDriver() { return driver; }
			bool isMoving();
			bool hasDriver() { return driver != nullptr; };
			float toRads(float deg);
			float toDeg(float rad);

			WOText* getTextLabel() { return textLabel; };
			WOGUILabel* getSpeedometer() { return speedometer; };
			
			physx::PxRigidDynamic* actor;

		protected:
			WOWarthog();

			Camera* driver;
			WOText* textLabel;
			WOGUILabel* speedometer;
			std::set<SDL_Keycode> keysPressed;
			irrklang::ISound* drivingSound;
			irrklang::ISound* horn;
			bool honking;

			bool isMovementKey(SDL_Keycode key);
			void fromPhysx();
			void toPhysx();
			void toPhysxWithPos(Vector p);
			void brake();
			void accelerate();
			void reverse();
	};

}

#endif

