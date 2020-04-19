#pragma once

#include "PxPhysicsAPI.h"
#include "AftrAPI.h"
#include "WOPhysx.h"
#include <map>
#include <iostream>

namespace Aftr {
	class WOPhysx;

	class IPhysicsManager {
		public:
			static void init();
			static void drop();

			static physx::PxPhysics* physics;
			static physx::PxScene* scene;
			
			static void simulate(NetMessengerClient* client);
			static physx::PxMaterial* createMaterial(physx::PxReal staticFriction, physx::PxReal dynamicFriction, physx::PxReal restitution);
			static physx::PxShape* createShape(const physx::PxGeometry& geometry, const physx::PxMaterial& material, bool isExclusive = false,
				physx::PxShapeFlags shapeFlags = physx::PxShapeFlag::eVISUALIZATION | physx::PxShapeFlag::eSCENE_QUERY_SHAPE | physx::PxShapeFlag::eSIMULATION_SHAPE);

			static void addActor(physx::PxActor* actor) { scene->addActor(*actor); };

			static physx::PxRigidDynamic* createConvexMesh(WO* wo);
			static physx::PxRigidStatic* createTriangleMesh(WO* wo);
			static physx::PxRigidStatic* createRigidStatic(physx::PxTransform transform) { return physics->createRigidStatic(transform); };
			static physx::PxTriangleMesh* createTriangleMesh(physx::PxDefaultMemoryInputData readBuffer) { return physics->createTriangleMesh(readBuffer); };

			static physx::PxFoundation* getFoundation() { return foundation; };
			static physx::PxCooking* getCooking() { return gCooking; };
			static physx::PxPvd* getPvd() { return gPvd; };

			static physx::PxVec3 toPxVec3(Vector vec);
			static physx::PxMat44 mat4ToMat44(Mat4 mat);

		protected:
			static physx::PxFoundation* foundation;
			static physx::PxU32 version;
			static physx::PxDefaultErrorCallback gErrorCallback;
			static physx::PxDefaultAllocator gAllocator;
			static physx::PxPvd* gPvd;
			static physx::PxCooking* gCooking;
			static physx::PxDefaultCpuDispatcher* gDispatcher;
			static physx::PxMaterial* gMaterial;
			static physx::PxPvdTransport* transport;

			static std::map<ModelDataSharedID, physx::PxShape*> convexShapes;
	};
}


