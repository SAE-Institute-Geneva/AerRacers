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

namespace neko
{
using Color3 = Vec3f;
using Color4 = Vec4f;

namespace Color
{
	const static Color4 white = Color4::one;
	const static Color4 grey = Color4(0.5f, 0.5f, 0.5f, 1.0f);
	const static Color4 gray = grey;
	const static Color4 black = Color4(0.0f, 0.0f, 0.0f, 1.0f);
					  
	const static Color4 red = Color4(1.0f, 0.0f, 0.0f, 1.0f);
	const static Color4 green = Color4(0.0f, 1.0f, 0.0f, 1.0f);
	const static Color4 blue = Color4(0.0f, 0.0f, 1.0f, 1.0f);
					  
	const static Color4 orange = Color4(1.0f, 0.62f, 0.13f, 1.0f);
	const static Color4 yellow = Color4(1.0f, 0.92f, 0.016f, 1.0f);
	const static Color4 cyan = Color4(0.0f, 1.0f, 1.0f, 1.0f);
	const static Color4 magenta = Color4(1.0f, 0.0f, 1.0f, 1.0f);
	
	const static Color4 clear = Color4::zero;
}
}
