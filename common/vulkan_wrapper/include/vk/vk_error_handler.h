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

 Author: Canas Simon
 Date: 23/02/2021
---------------------------------------------------------- */
#include "vk/vk_include.h"

#ifdef NEKO_ASSERT
namespace neko::vk
{
void CheckVkError(VkResult err, const char* msg, const char* file, int line);
#ifdef NEKO_KTX
void PrintKtxError(ktx_error_code_e result, const char* file, int line);
#endif
}
#define vkCheckError(err, msg) CheckVkError(err, msg, __FILE__, __LINE__)
#ifdef NEKO_KTX
#define ktxCheckError(err) PrintKtxError(err, __FILE__, __LINE__)
#endif
#else
#define vkCheckError(err, msg) void(0);
#ifdef NEKO_KTX
#define ktxCheckError(err) void(0);
#endif
#endif
