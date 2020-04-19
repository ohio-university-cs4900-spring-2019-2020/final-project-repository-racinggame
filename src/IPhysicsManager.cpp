#include "IPhysicsManager.h"

using namespace Aftr;
using namespace physx;

PxU32 IPhysicsManager::version = PX_PHYSICS_VERSION;
PxDefaultErrorCallback IPhysicsManager::gErrorCallback;
PxDefaultAllocator IPhysicsManager::gAllocator;
PxFoundation* IPhysicsManager::foundation = PxCreateFoundation(version, gAllocator, gErrorCallback);
PxPvd* IPhysicsManager::gPvd = PxCreatePvd(*foundation);
PxPvdTransport* IPhysicsManager::transport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 100);
PxPhysics* IPhysicsManager::physics = PxCreateBasePhysics(version, *foundation, PxTolerancesScale(), true, gPvd);
PxCooking* IPhysicsManager::gCooking = PxCreateCooking(version, *foundation, PxCookingParams(physics->getTolerancesScale()));
PxDefaultCpuDispatcher* IPhysicsManager::gDispatcher = PxDefaultCpuDispatcherCreate(4);
PxMaterial* IPhysicsManager::gMaterial = physics->createMaterial(0.5f, 0.5f, 0.6f);
PxScene* IPhysicsManager::scene;
std::map<ModelDataSharedID, physx::PxShape*> IPhysicsManager::convexShapes;

void IPhysicsManager::init() {
    gPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);
    if (gPvd->isConnected()) {
        std::cout << "PVD Connection Made!" << std::endl << std::endl;
    } else {
        std::cout << "PVD Connection Failed!" << std::endl << std::endl;
    }
    
    physx::PxSceneDesc gSceneDesc(physics->getTolerancesScale());
    gSceneDesc.gravity = (PxVec3(0.f, 0.f, -9.81f));
    gSceneDesc.flags = PxSceneFlag::eENABLE_ACTIVE_ACTORS;
    gSceneDesc.cpuDispatcher = gDispatcher;
    gSceneDesc.filterShader = PxDefaultSimulationFilterShader;
    scene = physics->createScene(gSceneDesc);
    if (scene->getScenePvdClient()) {
        scene->getScenePvdClient()->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
        scene->getScenePvdClient()->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
        scene->getScenePvdClient()->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
    }
}

void IPhysicsManager::drop() {
    scene->release();
    foundation->release();
    physics->release();
    gPvd->release();
    transport->release();
    gCooking->release();
}

void IPhysicsManager::simulate(NetMessengerClient* client) {
    //float time = (float) ManagerSDLTime::getTimeSinceLastPhysicsIteration();
    //if (time < 20) {
    //    scene->simulate(time / 1000.f);
    //} else {
    //    scene->simulate(16.5f / 1000.f);
    //}
    scene->simulate(1.f / 60.f);
    scene->fetchResults(true);

    PxU32 num_transforms = 0;
    PxActor** activeActors = scene->getActiveActors(num_transforms);
    for (PxU32 i = 0; i < num_transforms; i++) {
        WOPhysx* wo = static_cast<WOPhysx*>(activeActors[i]->userData); // Get the pair
        if (wo != nullptr && wo->actor != nullptr) {
            wo->toPhysx(client);
        }
    }
}

PxMaterial* IPhysicsManager::createMaterial(PxReal staticFriction, PxReal dynamicFriction, PxReal restitution) {
    return physics->createMaterial(staticFriction, dynamicFriction, restitution);
}

PxShape* IPhysicsManager::createShape(const PxGeometry& geometry, const PxMaterial& material, bool isExclusive, PxShapeFlags shapeFlags) {
    return physics->createShape(geometry, material, isExclusive, shapeFlags);
}

PxRigidDynamic* IPhysicsManager::createConvexMesh(WO* wo) {
    Model* model = wo->getModel();
    ModelDataShared* modelData = model->getModelDataShared();
    ModelDataSharedID modelID(modelData->getFileName(), modelData->getInitialScaleFactor());
    PxShape* shape;
    
    std::map<ModelDataSharedID, PxShape*>::iterator it = convexShapes.find(modelID);
    if (it == convexShapes.end()) {
        const std::vector<Vector>& verts = model->getCompositeVertexList();
        const std::vector<unsigned int>& inds = model->getCompositeIndexList();

        // describe convex mesh geometry
        PxConvexMeshDesc desc;
        desc.points.count = PxU32(verts.size());
        desc.points.stride = sizeof(Vector);
        desc.points.data = &verts.front();
        desc.flags = PxConvexFlag::eCOMPUTE_CONVEX;

        // cook geometry into convex mesh
        PxDefaultMemoryOutputStream buf;
        if (!gCooking->cookConvexMesh(desc, buf)) {
            exit(-1);
        }
        PxDefaultMemoryInputData stream(buf.getData(), buf.getSize());
        PxConvexMesh* triangleMesh = physics->createConvexMesh(stream);

        // create shape and add it to map
        shape = physics->createShape(PxConvexMeshGeometry(triangleMesh), *createMaterial(0.5f, 0.5f, 0.6f));
        convexShapes.insert(std::make_pair(modelID, shape));
    } else {
        // reuse existing shape
        shape = it->second;
    }

    // create actor and add it to scene
    PxRigidDynamic* actor = PxCreateDynamic(*physics, PxTransform(PxVec3(0.f, 0.f, 0.f)), *shape, PxReal(2.f));
    scene->addActor(*actor);
    actor->userData = wo;
    return actor;
}

PxRigidStatic* IPhysicsManager::createTriangleMesh(WO* wo) {
    ModelDataShared* modelData = wo->getModel()->getModelDataShared();
    //make copy of vertex list and index list
    size_t vertexListSize = modelData->getCompositeVertexList().size();
    size_t indexListSize = modelData->getCompositeIndexList().size();

    float* vertexListCopy = new float[vertexListSize * 3];
    unsigned int* indicesCopy = new unsigned int[indexListSize];

    for (size_t i = 0; i < vertexListSize; i++) {
        vertexListCopy[i * 3 + 0] = modelData->getCompositeVertexList().at(i).x;
        vertexListCopy[i * 3 + 1] = modelData->getCompositeVertexList().at(i).y;
        vertexListCopy[i * 3 + 2] = modelData->getCompositeVertexList().at(i).z;
    }
    for (size_t i = 0; i < indexListSize; i++) {
        indicesCopy[i] = modelData->getCompositeIndexList().at(i);
    }

    PxTriangleMeshDesc meshDesc;
    meshDesc.points.count = PxU32(vertexListSize);
    meshDesc.points.stride = PxU32(sizeof(float) * 3);
    meshDesc.points.data = vertexListCopy;

    meshDesc.triangles.count = PxU32(indexListSize / 3);
    meshDesc.triangles.stride = PxU32(3 * sizeof(unsigned int));
    meshDesc.triangles.data = indicesCopy;
    PxDefaultMemoryOutputStream writeBuffer;
    PxTriangleMeshCookingResult::Enum result;
    bool status = gCooking->cookTriangleMesh(meshDesc, writeBuffer, &result);
    if (!status) {
        std::cout << "Failed to create Triangular mesh" << std::endl;
        std::cin.get();
    }
    PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
    physx::PxTriangleMesh* mesh = physics->createTriangleMesh(readBuffer);

    PxMaterial* gMaterial = createMaterial(0.5f, 0.5f, 0.6f);
    PxShape* shape = createShape(PxTriangleMeshGeometry(mesh), *gMaterial, true);

    PxRigidStatic* actor = physics->createRigidStatic(PxTransform({0.f, 0.f, 0.f}));
    bool b = actor->attachShape(*shape);

    actor->userData = wo;
    addActor(actor);
    return actor;
}

PxVec3 IPhysicsManager::toPxVec3(Vector vec) {
    return PxVec3(vec.x, vec.y, vec.z);
}

PxMat44 IPhysicsManager::mat4ToMat44(Mat4 mat) {
    PxMat44 matrix = PxMat44();

    matrix[0][0] = mat[0];  matrix[0][1] = mat[1];  matrix[0][2] = mat[2];  matrix[0][3] = mat[3];
    matrix[1][0] = mat[4];  matrix[1][1] = mat[5];  matrix[1][2] = mat[6];  matrix[1][3] = mat[7];
    matrix[2][0] = mat[8];  matrix[2][1] = mat[9];  matrix[2][2] = mat[10]; matrix[2][3] = mat[11];
    matrix[3][0] = mat[12]; matrix[3][1] = mat[13]; matrix[3][2] = mat[14]; matrix[3][3] = mat[15];

    return matrix;
}