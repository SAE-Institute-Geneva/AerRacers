/* ----------------------------------------------------
 MIT License

 Copyright (c) 2020 SAE Institute Switzerland AG

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.

 Author : Floreau Luca
 Co-Author :
 Date : 22.11.2020
---------------------------------------------------------- */

#include <engine/component.h>

namespace neko::physics
{
    class Transform3dManager;
    class ColliderManager;

    struct RigidBody
    {
        RigidBody() = default;
        ~RigidBody() = default;
        Vec3f velocity = Vec3f::zero;
        Vec3f angularVelocity = Vec3f::zero;
        float drag = 0.0f;
        float angularDrag = 0.0f;
        float mass = 1.0f;
        bool useGravity = false;
        bool isKinematic = false;
        Vec3<bool> freezePosition = Vec3<bool>(false);
        Vec3<bool> freezeRotation = Vec3<bool>(true);
    };

    class RigidBodyManager :
        public ComponentManager<RigidBody, EntityMask(ComponentType::RIGIDBODY)>,
        public SystemInterface
    {
    public:
        explicit RigidBodyManager(
            EntityManager& entityManager,
            Transform3dManager& transformManager,
            ColliderManager& colliderManager) :
            ComponentManager(entityManager),
            transformManager_(transformManager),
            colliderManager_(colliderManager)
        {}

        void Update(seconds dt) override;
        void AddForceAtPosition(Vec3f force, Vec3f position);
        void AddForce(Vec3f force);
    protected:
        Transform3dManager& transformManager_;
        ColliderManager& colliderManager_;
    };
}