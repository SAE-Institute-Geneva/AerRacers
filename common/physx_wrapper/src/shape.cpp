#include <shape.h>
#include <physics_engine.h>


namespace neko::physics::px {

void SpherePhysicsShape::Init(physx::PxPhysics* physics)
{
	material_ = physics->createMaterial(material.staticFriction, material.dynamicFriction, material.bouciness);
	if (!material_)
		std::cerr << "createMaterial failed!";
	shape_ = physics->createShape(physx::PxSphereGeometry(radius), *material_);
}
void BoxPhysicsShape::Init(physx::PxPhysics* physics)
{
	material_ = physics->createMaterial(material.staticFriction, material.dynamicFriction, material.bouciness);
	if (!material_)
		std::cerr << "createMaterial failed!";
	shape_ = physics->createShape(physx::PxBoxGeometry(size.x, size.y, size.z), *material_);
}

BoxPhysicsShapeManager::BoxPhysicsShapeManager(EntityManager& entityManager)
    : ComponentManager<BoxPhysicsShape, EntityMask(ComponentType::BOX_COLLIDER)>(entityManager) { }

void BoxPhysicsShapeManager::FixedUpdate(seconds dt)
{
    
}

SpherePhysicsShapeManager::SpherePhysicsShapeManager(
    EntityManager& entityManager)
    : ComponentManager<SpherePhysicsShape, EntityMask(ComponentType::SPHERE_COLLIDER)>(entityManager) { }

void SpherePhysicsShapeManager::FixedUpdate(seconds dt)
{
    
}

}
