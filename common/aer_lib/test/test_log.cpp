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

 Author : Dylan von Arx
 Co-Author :
 Date : 03.11.2020
---------------------------------------------------------- */

#include <gtest/gtest.h>

#include "aer/log.h"

namespace neko
{
TEST(Logs, TestLogsBasic)
{
	LogManager logger;

	LogDebug(LogCategory::ENGINE, "Engine is running");
	LogWarning("That feature isn't fully supported yet");
	LogError("Could not retrieve value!");

	logger.Wait();
}

TEST(Logs, TestLogsFromNewThread)
{
	LogManager logger;

	auto task(
		[]()
		{
			LogDebug("Task Begin");
			LogDebug("This is a task");
		});

	auto newThread = std::thread(
		[task]()
		{
			LogDebug(LogCategory::ENGINE, "Creating logs from another thread");
			task();
		});

	newThread.join();

	logger.Wait();
    LogDebug("Test without check");
}
}    // namespace neko
