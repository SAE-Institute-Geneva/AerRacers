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

    struct PhysicsMaterial {
        float dynamicFriction = 0.6f;
        float staticFriction = 0.6f;
        float bouciness = 0.6f;
    };

    struct Collider
    {
        Collider() = default;
        ~Collider() = default;
        bool isTrigger = false;
        PhysicsMaterial material = PhysicsMaterial();
        Vec3f center = Vec3f::zero;
        Vec3f size = Vec3f::one;
    };

    class ColliderManager :
        public ComponentManager<Collider, EntityMask(ComponentType::COLLIDER)>,
        public SystemInterface
    {
    public:
        explicit ColliderManager(
            EntityManager& entityManager,
            Transform3dManager& transformManager) :
            ComponentManager(entityManager),
            transformManager_(transformManager)
        {}

        void Update(seconds dt) override;
    protected:
        Transform3dManager& transformManager_;
    };
}
