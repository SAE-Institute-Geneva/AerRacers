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

 Author : Feser Sebastien
 Co-Author :
 Date : 17.03.2021
---------------------------------------------------------- */
#include <engine/entity.h>
#include <mathematics/vector.h>
#include <array>
#include <utils/json_utility.h>
//#include <aer/managers/player_manager.h>


namespace neko::aer
{
    using WaypointIndex = uint8_t;
    using RacePlacement = uint8_t;
    using PlayerId = uint8_t;
    using WaypointsCount = int;

    class AerEngine;

    struct Waypoint
    {
        WaypointIndex index;

        Vec2f position;
        Vec2f normalizedNextVector;
        Vec2f normalizedNextVector2;

        WaypointIndex previousWaypoint;
        WaypointIndex nextWaypoint;

        WaypointIndex previousWaypoint2;
        WaypointIndex nextWaypoint2;

        float lengthNext;
        float lengthNext2;

        bool hasTwoPrevious = false;
        bool hasTwoNext = false;
    };

    struct PlayerPositionData
    {
        std::array<RacePlacement, 4> racePlacement;
        std::array<float, 4> positionInWaypoint;
        std::array<WaypointIndex, 4> waypoints;
        std::array<WaypointsCount, 4> waypointsCount;
    };

    //class AerEngine;

    class WaypointManager : public SystemInterface, public DrawImGuiInterface
    {
    public:
        WaypointManager(AerEngine& engine);
        void Init() override;
        void Update(seconds dt) override;
        void DrawImGui();
        void AddWaypointFromJson(Entity entity, const json& jsonComponent);
        void CalculatePlayerPosition(Vec3f playerPosition, PlayerId playerId);
        void CalculatePlayerPlacement();
        void StartDetection();
        void RestartWaypointManager();
        PlayerPositionData* GetPlayerPositionData();
        void Destroy() override;
    private:
        bool hasPlayersSpawned = false;
        std::vector<Waypoint> waypoints_;
        AerEngine& engine_;
        PlayerPositionData playerPositionData_;
    };
}



