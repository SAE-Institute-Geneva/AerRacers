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
#include <aer/managers/waypoint_manager.h>
#include <engine/entity.h>

namespace neko::aer
{

    WaypointManager::WaypointManager(AerEngine& engine) : engine_(engine)
    {
        
    }

    void WaypointManager::Init()
    {
        WaypointIndex startWaypointIndex = 0;
        for (uint8_t i = 0; i < engine_.GetComponentManagerContainer().playerManager.GetPlayerCount(); i++)
        {
            playerPositionData_.waypoints[i] = startWaypointIndex;
            playerPositionData_.waypointsCount[i] = 0;
            CalculatePlayerPosition(engine_.GetComponentManagerContainer().playerManager.GetPlayerPosition(i), i);
            CalculatePlayerPlacement();
        }
    }

    void WaypointManager::Update()
    {
        for (uint8_t i = 0; i < engine_.GetComponentManagerContainer().playerManager.GetPlayerCount(); i++)
        {
            CalculatePlayerPosition(engine_.GetComponentManagerContainer().playerManager.GetPlayerPosition(i), i);
            CalculatePlayerPlacement();
        }
    }

    void WaypointManager::AddWaypointFromJson(Entity entity, const json& jsonComponent)
    {
        Waypoint newWaypoint;
        if (CheckJsonParameter(jsonComponent, "position", json::object()))
        {
            newWaypoint.position = GetVector2FromJson(jsonComponent, "position");
        }
        if (CheckJsonParameter(jsonComponent, "normalizedNextVector", json::object()))
        {
            newWaypoint.normalizedNextVector = GetVector2FromJson(jsonComponent, "normalizedNextVector");
        }
        if (CheckJsonParameter(jsonComponent, "previousWaypoint", json::object()))
        {
            if ((IsJsonValueNumeric(jsonComponent["previousWaypoint"])))
            {
                newWaypoint.previousWaypoint = jsonComponent["previousWaypoint"];
            }
        }
        if (CheckJsonParameter(jsonComponent, "nextWaypoint", json::object()))
        {
            if ((IsJsonValueNumeric(jsonComponent["nextWaypoint"])))
            {
                newWaypoint.nextWaypoint = jsonComponent["nextWaypoint"];
            }
        }
        if (CheckJsonParameter(jsonComponent, "previousWaypoint2", json::object()))
        {
            if ((IsJsonValueNumeric(jsonComponent["previousWaypoint2"])))
            {
                newWaypoint.previousWaypoint2 = jsonComponent["previousWaypoint2"];
            }
        }
        if (CheckJsonParameter(jsonComponent, "nextWaypoint2", json::object()))
        {
            if ((IsJsonValueNumeric(jsonComponent["nextWaypoint2"])))
            {
                newWaypoint.previousWaypoint2 = jsonComponent["nextWaypoint2"];
            }
        }
        if (CheckJsonParameter(jsonComponent, "lengthNext", json::object()))
        {
            if ((IsJsonValueNumeric(jsonComponent["lengthNext"])))
            {
                newWaypoint.lengthNext = jsonComponent["lengthNext"];
            }
        }
        if (CheckJsonParameter(jsonComponent, "hasTwoPrevious", json::object()))
        {
            //TODO: Check if it works
            if (jsonComponent["hasTwoPrevious"] == "true")
            {
                newWaypoint.hasTwoNext = true;
            }
            else
            {
                newWaypoint.hasTwoNext = false;
            }
        }
        if (CheckJsonParameter(jsonComponent, "hasTwoNext", json::object()))
        {
            //TODO: Check if it works
            if (jsonComponent["hasTwoNext"] == "true")
            {
                newWaypoint.hasTwoPrevious = true;
            }
            else
            {
                newWaypoint.hasTwoPrevious = false;
            }
        }
    }

    void WaypointManager::CalculatePlayerPosition(Vec3f playerPosition, PlayerId playerId)
    {
        //TODO: If multiple Waypoints
        Vec2f playerPosition2d = Vec2f(playerPosition.x, playerPosition.z);
        float position = Vec2f::Dot(playerPosition2d, waypoints_[playerPositionData_.waypoints[playerId]].normalizedNextVector);
        if (position > waypoints_[playerPositionData_.waypoints[playerId]].lengthNext)
        {
            playerPositionData_.waypoints[playerId] = waypoints_[playerPositionData_.waypoints[playerId]].nextWaypoint;
            playerPositionData_.waypointsCount[playerId]++;
            playerPositionData_.positionInWaypoint[playerId] = position;
        }

        else if (position < 0)
        {
            float previousPosition = Vec2f::Dot(playerPosition2d, waypoints_[waypoints_[playerPositionData_.waypoints[playerId]].previousWaypoint].normalizedNextVector);
            if (waypoints_[waypoints_[playerPositionData_.waypoints[playerId]].previousWaypoint].lengthNext
                > previousPosition)
            {
                playerPositionData_.waypoints[playerId] = waypoints_[playerPositionData_.waypoints[playerId]].previousWaypoint;
                playerPositionData_.waypointsCount[playerId]--;
                playerPositionData_.positionInWaypoint[playerId] = previousPosition;
            }
            else
            {
                playerPositionData_.positionInWaypoint[playerId] = position;
            }
        }
    }

    void WaypointManager::CalculatePlayerPlacement()
    {
        //TODO: Sort
        for (int i = 0; i < engine_.GetComponentManagerContainer().playerManager.GetPlayerCount(); i++)
        {
            if (i > 0)
            {
                for (int j = i; j >= 0; j--)
                {
                    if (playerPositionData_.waypointsCount[i] > playerPositionData_.waypointsCount[j])
                    {
                        playerPositionData_.positionInWaypoint[j]++;
                    }
                    else if (playerPositionData_.waypointsCount[i] == playerPositionData_.waypointsCount[j])
                    {
                        if (playerPositionData_.positionInWaypoint[i] > playerPositionData_.positionInWaypoint[j])
                        {
                            playerPositionData_.positionInWaypoint[j]++;
                        }
                        else
                        {
                            playerPositionData_.positionInWaypoint[i]++;
                        }
                    }
                    else
                    {
                        playerPositionData_.positionInWaypoint[i]++;
                    }
                }
            }
            else
            {
                playerPositionData_.waypointsCount[i] = 1;
            }
        }
    }

    void WaypointManager::Destroy()
    {
        
    }
}
