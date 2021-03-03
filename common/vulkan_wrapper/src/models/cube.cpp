#include "vk/models/cube.h"

namespace neko::vk
{
RenderCuboid::RenderCuboid(const Vec3f& offset, const Vec3f& size) : offset_(offset), size_(size){}

void RenderCuboid::Init()
{
    const std::vector<Vertex> vertices =
    {
    	//Front Face
        {Vec3f(0.5f,  -0.5f,  0.5f) * size_ + offset_, Vec3f::forward, {0, 1}}, //Top Left
        {Vec3f(-0.5f, -0.5f,  0.5f) * size_ + offset_, Vec3f::forward, {1, 1}}, //Top Right
        {Vec3f(-0.5f, 0.5f,   0.5f) * size_ + offset_, Vec3f::forward, {1, 0}}, //Bottom Right
        {Vec3f(0.5f,  0.5f,   0.5f) * size_ + offset_, Vec3f::forward, {0, 0}}, //Bottom left
    	//Back Face
        {Vec3f(-0.5f, -0.5f, -0.5f) * size_ + offset_, Vec3f::back, {0, 1}}, //Top Left
        {Vec3f(0.5f,  -0.5f, -0.5f) * size_ + offset_, Vec3f::back, {1, 1}}, //Top right
        {Vec3f(0.5f,  0.5f,  -0.5f) * size_ + offset_, Vec3f::back, {1, 0}}, //Bottom Right
        {Vec3f(-0.5f, 0.5f,  -0.5f) * size_ + offset_, Vec3f::back, {0, 0}}, //Bottom left
    	//Right Face
        {Vec3f(0.5f,  -0.5f, -0.5f) * size_ + offset_, Vec3f::right, {0, 1}}, //Top Left
        {Vec3f(0.5f,  -0.5f,  0.5f) * size_ + offset_, Vec3f::right, {1, 1}}, //Top right
        {Vec3f(0.5f,  0.5f,   0.5f) * size_ + offset_, Vec3f::right, {1, 0}}, //Bottom Right
        {Vec3f(0.5f,  0.5f,  -0.5f) * size_ + offset_, Vec3f::right, {0, 0}}, //Bottom left
    	//Left Face
        {Vec3f(-0.5f, -0.5f,  0.5f) * size_ + offset_, Vec3f::left, {0, 1}}, //Top Left
        {Vec3f(-0.5f, -0.5f, -0.5f) * size_ + offset_, Vec3f::left, {1, 1}}, //Top right
        {Vec3f(-0.5f, 0.5f,  -0.5f) * size_ + offset_, Vec3f::left, {1, 0}}, //Bottom Right
        {Vec3f(-0.5f, 0.5f,   0.5f) * size_ + offset_, Vec3f::left, {0, 0}}, //Bottom left
    	//Top Face
        {Vec3f(0.5f,  0.5f,   0.5f) * size_ + offset_, Vec3f::up, {0, 1}}, //Top Left
        {Vec3f(-0.5f, 0.5f,   0.5f) * size_ + offset_, Vec3f::up, {1, 1}}, //Top Right
        {Vec3f(-0.5f, 0.5f,  -0.5f) * size_ + offset_, Vec3f::up, {1, 0}}, //Bottom Right
        {Vec3f(0.5f,  0.5f,  -0.5f) * size_ + offset_, Vec3f::up, {0, 0}}, //Bottom left
    	//Bottom Face
        {Vec3f(0.5f,  -0.5f, -0.5f) * size_ + offset_, Vec3f::down, {0, 0}}, //Bottom left
        {Vec3f(-0.5f, -0.5f, -0.5f) * size_ + offset_, Vec3f::down, {1, 0}}, //Bottom Right
        {Vec3f(-0.5f, -0.5f,  0.5f) * size_ + offset_, Vec3f::down, {1, 1}}, //Top Right
        {Vec3f(0.5f,  -0.5f,  0.5f) * size_ + offset_, Vec3f::down, {0, 1}}, //Top Left
    };

    const std::vector<std::uint32_t> indices =
    {
		0,  1,  2,  2,  3,  0,  //Front Face
		4,  5,  6,  6,  7,  4,  //Back Face
		8,  9,  10, 10, 11, 8,  //Right Face
		12, 13, 14, 14, 15, 12, //Left Face
		16, 17, 18, 18, 19, 16, //Top Face
		20, 21, 22, 22, 23, 20, //Bottom Face
    };

    InitData(vertices, indices);
}
}