#pragma once
/*
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
 */
#include <mathematics/vector.h>
#include <graphics/shape.h>

namespace neko::gl
{
struct VertexArrayObject
{
    unsigned int VAO = 0;
    unsigned int EBO = 0;
    unsigned int VBO[4]{};
};

class RenderCircle : public neko::RenderCircle, public VertexArrayObject
{
public:
    using neko::RenderCircle::RenderCircle;
    void Init() override;

    void Draw() const override;

    void Destroy() override;
};

class RenderQuad : public neko::RenderQuad, public VertexArrayObject
{
public:
    using neko::RenderQuad::RenderQuad;
    void Init() override;

    void Draw() const override;

    void Destroy() override;

    void SetValues(const Vec2f& newSize, const Vec3f& newOffset);
    void SetSize(const Vec2f& newSize);
    void SetOffset(const Vec3f& newOffset);
};

class RenderCuboid : public neko::RenderCuboid, public VertexArrayObject
{
public:
    using neko::RenderCuboid::RenderCuboid;
    void Init() override;

    void Draw() const override;

    void Destroy() override;
};

class RenderWireFrameCuboid final : public neko::RenderWireFrameCuboid, public VertexArrayObject
{
public:
    using neko::RenderWireFrameCuboid::RenderWireFrameCuboid;
    void Init() override;

    void Draw() const override;

    void Destroy() override;
};

class RenderLine3d final : public neko::RenderLine3d, public VertexArrayObject
{
public:
    using neko::RenderLine3d::RenderLine3d;
    void Init() override;

    void Draw() const override;

    void Destroy() override;
};

class RenderSphere : public neko::RenderSphere, public VertexArrayObject
{
public:
	explicit RenderSphere(Vec3f offset, float radius, size_t segment = 64);

    void Init() override;

    void Draw() const override;

    void Destroy() override;
protected:
    size_t segment_ = 0;
	size_t indexCount_ = 0;
};

class RenderWireFrameSphere : public neko::RenderWireFrameSphere, public VertexArrayObject
{
public:
    explicit RenderWireFrameSphere(Vec3f offset, float radius);

    void Init() override;

    void Draw() const override;

    void Destroy() override;
};
}