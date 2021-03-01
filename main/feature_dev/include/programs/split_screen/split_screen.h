#pragma once
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

 Author : Simon Canas
 Co-Author :
 Date : 16.02.2021
---------------------------------------------------------- */
#include "dev/sample_program.h"

#include "gl/shader.h"
#include "gl/shape.h"
#include "graphics/graphics.h"
#include "programs/split_screen/game_camera.h"

namespace neko::dev
{
constexpr std::uint8_t MaxPlayerNum = 4;

///Test for the split screen implementation
class SplitScreen : public SampleProgram
{
public:
	void Init() override;
	void Update(seconds dt) override;
	void DrawImGui() override;
	void Render() override;
	void Destroy() override;

	void OnEvent(const SDL_Event& event) override;

private:
	void RenderScene() const;

	seconds timeSinceInit_ = seconds(0.0f);

	GameCamera camera_;

	gl::Shader shader_;
	gl::RenderCuboid cube_{Vec3f::zero, Vec3f::one};

	std::uint8_t playerNum_ = MaxPlayerNum;
};
}    // namespace neko::dev
