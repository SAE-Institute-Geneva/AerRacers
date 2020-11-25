#include <iostream>
#include <material.h>

namespace neko::physics::px {
	PhysicsMaterial::PhysicsMaterial(physx::PxPhysics* physics)
	{
		material_ = physics->createMaterial(0.5f, 0.5f, 0.1f);
		if (!material_)
			std::cerr << "createMaterial failed!";
	}
}
