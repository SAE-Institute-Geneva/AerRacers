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

 Author : Floreau Luca
 Co-Author : 
 Date : 28.02.2021
---------------------------------------------------------- */
#include <engine\entity.h>
#include <mathematics/vector.h>

namespace neko::aer
{
struct ComponentManagerContainer;
const size_t INIT_PLAYER_NMB = 4;
	struct PlayerComponent
	{
		Entity shipEntity = INVALID_ENTITY;
		Entity cameraEntity = INVALID_ENTITY;
		Entity shipModelEntity = INVALID_ENTITY;

		int playerNumber = 0;
		int linkedJoystick = 0;

		//TODO texture
	};

	/**
	 * \brief PlayerManager use to store player data
	 */
	class PlayerManager
	{
	public:
		explicit PlayerManager(ComponentManagerContainer& cContainer) : cContainer_(cContainer)
		{
			playerComponents_.reserve(INIT_PLAYER_NMB);
		}

		virtual ~PlayerManager() = default;

		int CreatePlayer(Vec3f pos);

	protected:
		int playerCount = 0;
		std::vector<PlayerComponent> playerComponents_;
		ComponentManagerContainer& cContainer_;
	};
}
