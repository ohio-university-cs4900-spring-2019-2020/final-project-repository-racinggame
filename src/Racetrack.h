#pragma once

#include "AftrAPI.h"
#include "IPhysicsManager.h"
#include "PxPhysicsAPI.h"

#ifdef AFTR_CONFIG_USE_BOOST

namespace Aftr {

	class Racetrack : public WO {
		public:
			WOMacroDeclaration(Racetrack, WO);

			static Racetrack* New(
				const std::string& trackModel = "../mm/models/test/figure8.dae",
				Vector scale = Vector(1.f, 1.f, 1.f),
				MESH_SHADING_TYPE shadingType = MESH_SHADING_TYPE::mstAUTO
			);
			virtual void onCreate(
				const std::string& trackModel, 
				Vector scale = Vector(1.f, 1.f, 1.f), 
				MESH_SHADING_TYPE shadingType = MESH_SHADING_TYPE::mstAUTO
			);
			virtual void setPosition(Vector pos);
			virtual void setPosition(float x, float y, float z) { setPosition(Vector(x, y, z)); }

			physx::PxRigidStatic* actor;

		protected:
			float* vertexListCopy;
			unsigned int* indicesCopy;
			Racetrack();
	};
}

#endif