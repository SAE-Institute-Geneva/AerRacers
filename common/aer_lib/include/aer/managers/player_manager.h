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
#include <sdl_engine/sdl_input.h>

namespace neko::aer
{
class ShipInputManager;
class ShipControllerManager;
class CameraControllerManager;
struct ComponentManagerContainer;

const size_t INIT_PLAYER_NMB = 4;

using PlayerId = uint8_t;

struct PlayerComponent {
    Entity shipEntity      = INVALID_ENTITY;
    Entity shipModelEntity = INVALID_ENTITY;

    PlayerId playerNumber            = 0;
    sdl::ControllerId linkedJoystick = 0;
    Vec3f playerSpawn                = Vec3f(0, 0, 0);

    //TODO texture
};

/**
 * \brief PlayerManager use to store player data
 */
class PlayerManager final : public SystemInterface {
public:
    explicit PlayerManager(ComponentManagerContainer& cContainer);

    virtual ~PlayerManager() = default;

    PlayerId CreatePlayer(Vec3f pos,
        EulerAngles euler = EulerAngles(degree_t(0.0f), degree_t(0.0f), degree_t(0.0f)));

    PlayerComponent GetPlayerComponent(PlayerId playerId);
    Entity GetShipEntity(PlayerId playerId);
    size_t GetPlayerCount() const { return playerCount_; }

    void Init() override;
    void Update(seconds dt) override;
    void Destroy() override;
private:
    size_t playerCount_ = 0;
    std::vector<PlayerComponent> playerComponents_;
    ComponentManagerContainer& cContainer_;
    CameraControllerManager& cameraControllerManager_;
    ShipControllerManager& shipControllerManager_;
    ShipInputManager& shipInputManager_;

    const float kYDespawnPosition_ = -10000.0f;
};
}
