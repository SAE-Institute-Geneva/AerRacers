#include <physics/collider.h>
#include <engine\log.h>


namespace neko::physics {
BoxColliderManager::BoxColliderManager(EntityManager& entityManager)
    : ComponentManager(entityManager) {}

void BoxColliderManager::FixedUpdate(seconds dt) {}

CircleColliderManager::CircleColliderManager(EntityManager& entityManager)
    : ComponentManager(entityManager) {}

void CircleColliderManager::FixedUpdate(seconds dt) {}
}
