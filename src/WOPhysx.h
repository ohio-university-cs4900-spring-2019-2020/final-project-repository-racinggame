#pragma once

#include "AftrAPI.h"
#include "NetMsgWO.h"
#include "IPhysicsManager.h"
#include "PxPhysicsAPI.h"

#ifdef AFTR_CONFIG_USE_BOOST

namespace Aftr {
	class WOPhysx : public WO {
		public:
			WOMacroDeclaration(WOPhysx, WO);

			static WOPhysx* New(
				const std::string& modelFileName = "../mm/models/toilet/model.dae",
				Vector scale = Vector(15, 15, 15), MESH_SHADING_TYPE shadingType = MESH_SHADING_TYPE::mstAUTO
			);
			virtual void onCreate(const std::string& modelFileName, Vector scale, MESH_SHADING_TYPE shadingType);
			void setDisplayMatrix(Mat4 matrix) { WO::getModel()->setDisplayMatrix(matrix); }
			void toPhysx(NetMessengerClient* client);

			virtual void setPosition(Vector pos);
			virtual void setPosition(float x, float y, float z);

			physx::PxRigidDynamic* actor;
		protected:
			WOPhysx();
	};
}

#endif