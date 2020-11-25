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

#include <iostream>

#include "PxPhysicsAPI.h"
namespace neko::physics::px {
class PxAllocatorCallback
{
public:
    virtual ~PxAllocatorCallback() {}
    virtual void* allocate(size_t size, const char* typeName, const char* filename,
        int line) = 0;
    virtual void deallocate(void* ptr) = 0;
};

class UserErrorCallback : public physx::PxErrorCallback
{
public:
    virtual void reportError(
        physx::PxErrorCode::Enum code, const char* message, const char* file,
        int line)
    {
        // error processing implementation
    }
};

class PxPhysicsEngine
{
public:
    void Start();

    void Update(float dt);

    void Destroy();

//private:
    void CreateScene();

    physx::PxFoundation* mFoundation_ = nullptr;
    physx::PxPhysics* mPhysics_ = nullptr;
    physx::PxPvd* mPvd_ = nullptr;
    physx::PxCooking* mCooking_ = nullptr;
    physx::PxDefaultCpuDispatcher* mCpuDispatcher_ = nullptr;
    physx::PxScene* scene_ = nullptr;
};

}
