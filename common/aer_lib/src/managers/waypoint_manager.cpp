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
#include <aer/aer_engine.h>
#include <utils/imgui_utility.h>

namespace neko::aer
{

    WaypointManager::WaypointManager(AerEngine& engine) : engine_(engine)
    {
        
    }

    void WaypointManager::Init()
    {
        engine_.RegisterOnDrawUi(*this);
    }

    void WaypointManager::Update(seconds dt)
    {
        if (hasPlayersSpawned)
        {
            for (uint8_t i = 0; i < engine_.GetComponentManagerContainer().playerManager.GetPlayerCount(); i++)
            {
                CalculatePlayerPosition(engine_.GetComponentManagerContainer().playerManager.GetPlayerPosition(i), i);
                CalculatePlayerPlacement();
            }
        }
    }

    void WaypointManager::DrawImGui()
    {
        // if (hasPlayersSpawned)
        // {
        //     for (int i = 0; i < engine_.GetComponentManagerContainer().playerManager.GetPlayerCount(); i++)
        //     {
        //         ImGui::Begin("Player" + std::to_string(i));
        //         std::string playerText = "Position: " + std::to_string(playerPositionData_.racePlacement[i]) +
        //             "\n Waypoint: " + std::to_string(playerPositionData_.waypoints[i]) +
        //             "\n Waypoint Count: " + std::to_string(playerPositionData_.waypointsCount[i]) +
        //             "\n Position in Waypoint: " + std::to_string(playerPositionData_.positionInWaypoint[i]) +
        //             "\n Placement: " + std::to_string(playerPositionData_.racePlacement[i]) +
        //             "\n Distance From Next WP: " + std::to_string(
        //                 sqrt(
        //
        //                     pow(engine_.GetComponentManagerContainer().playerManager.GetPlayerPosition(i).x
        //                         - waypoints_[waypoints_[playerPositionData_.waypoints[i]].nextWaypoint].position.x, 2) +
        //                     pow(engine_.GetComponentManagerContainer().playerManager.GetPlayerPosition(i).z
        //                         - waypoints_[waypoints_[playerPositionData_.waypoints[i]].nextWaypoint].position.y, 2))) +
        //             "\n Distance From Previous WP: " + std::to_string(sqrt(
        //                 pow(engine_.GetComponentManagerContainer().playerManager.GetPlayerPosition(i).x
        //                     - waypoints_[waypoints_[playerPositionData_.waypoints[i]].previousWaypoint].position.x, 2) +
        //                 pow(engine_.GetComponentManagerContainer().playerManager.GetPlayerPosition(i).z
        //                     - waypoints_[waypoints_[playerPositionData_.waypoints[i]].previousWaypoint].position.y, 2))) +
        //             "\n Current WP Position: " +
        //             "x:" + std::to_string(waypoints_[playerPositionData_.waypoints[i]].position.x) +
        //             " y:" + std::to_string(waypoints_[playerPositionData_.waypoints[i]].position.y) +
        //             "\n Last WP Position: " +
        //             "x:" + std::to_string(waypoints_[waypoints_[playerPositionData_.waypoints[i]].previousWaypoint].position.x) +
        //             " y:" + std::to_string(waypoints_[waypoints_[playerPositionData_.waypoints[i]].previousWaypoint].position.y) +
        //             "\n Next WP Position: " +
        //             "x:" + std::to_string(waypoints_[waypoints_[playerPositionData_.waypoints[i]].nextWaypoint].position.x) +
        //             " y:" + std::to_string(waypoints_[waypoints_[playerPositionData_.waypoints[i]].nextWaypoint].position.y) +
        //             "\n Player Position: " +
        //             "x:" + std::to_string(engine_.GetComponentManagerContainer().playerManager.GetPlayerPosition(i).x) +
        //             " y:" + std::to_string(engine_.GetComponentManagerContainer().playerManager.GetPlayerPosition(i).z) +
        //             "\n Next Normalized: " +
        //             "x:" + std::to_string(waypoints_[playerPositionData_.waypoints[i]].normalizedNextVector.x) +
        //             " y:" + std::to_string(waypoints_[playerPositionData_.waypoints[i]].normalizedNextVector.y);
        //
        //         ImGui::Text(playerText.c_str());
        //         ImGui::End();
        //     }
        // }
    }

    void WaypointManager::AddWaypointFromJson(Entity entity, const json& jsonComponent)
    {
        Waypoint newWaypoint;
        if (CheckJsonParameter(jsonComponent, "position", json::object()))
        {
            newWaypoint.position = GetVector2FromJson(jsonComponent, "position");
            newWaypoint.position.x *= -1;
        }
        if (CheckJsonParameter(jsonComponent, "normalizedNextVector", json::object()))
        {
            newWaypoint.normalizedNextVector = GetVector2FromJson(jsonComponent, "normalizedNextVector");
            newWaypoint.normalizedNextVector.x *= -1;
        }
        if (CheckJsonParameter(jsonComponent, "normalizedNextVector2", json::object()))
        {
            newWaypoint.normalizedNextVector2 = GetVector2FromJson(jsonComponent, "normalizedNextVector2");
            newWaypoint.normalizedNextVector2.x *= -1;
        }
        if (CheckJsonParameter(jsonComponent, "nextWaypoint", json::value_t::number_unsigned))
        {
            if ((IsJsonValueNumeric(jsonComponent["nextWaypoint"])))
            {
                newWaypoint.nextWaypoint = jsonComponent["nextWaypoint"];
            }
        }
        if (CheckJsonParameter(jsonComponent, "previousWaypoint", json::value_t::number_unsigned))
        {
            if ((IsJsonValueNumeric(jsonComponent["previousWaypoint"])))
            {
                newWaypoint.previousWaypoint = jsonComponent["previousWaypoint"];
            }
        }
        if (CheckJsonParameter(jsonComponent, "nextWaypoint2", json::value_t::number_unsigned))
        {
            if ((IsJsonValueNumeric(jsonComponent["nextWaypoint2"])))
            {
                newWaypoint.previousWaypoint2 = jsonComponent["nextWaypoint2"];
            }
        }
        if (CheckJsonParameter(jsonComponent, "previousWaypoint2", json::value_t::number_unsigned))
        {
            if ((IsJsonValueNumeric(jsonComponent["previousWaypoint2"])))
            {
                newWaypoint.previousWaypoint2 = jsonComponent["previousWaypoint2"];
            }
        }
        if (CheckJsonParameter(jsonComponent, "index", json::value_t::number_unsigned))
        {
            if ((IsJsonValueNumeric(jsonComponent["index"])))
            {
                newWaypoint.index = jsonComponent["index"];
            }
        }
        if (CheckJsonParameter(jsonComponent, "lengthNext", json::value_t::number_float))
        {
            if ((IsJsonValueNumeric(jsonComponent["lengthNext"])))
            {
                newWaypoint.lengthNext = jsonComponent["lengthNext"];
            }
        }
        if (CheckJsonParameter(jsonComponent, "lengthNext2", json::value_t::number_float))
        {
            if ((IsJsonValueNumeric(jsonComponent["lengthNext2"])))
            {
                newWaypoint.lengthNext2 = jsonComponent["lengthNext2"];
            }
        }
        if (CheckJsonParameter(jsonComponent, "hasTwoPrevious", json::value_t::boolean))
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
        if (CheckJsonParameter(jsonComponent, "hasTwoNext", json::value_t::boolean))
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
        waypoints_.push_back(newWaypoint);
    }

    void WaypointManager::CalculatePlayerPosition(Vec3f playerPosition, PlayerId playerId)
    {
        Vec2f playerPosition2d = Vec2f(playerPosition.x, playerPosition.z);
        Vec2f vecWaypointPlayer = playerPosition2d - waypoints_[playerPositionData_.waypoints[playerId]].position;
        float positionInWP = Vec2f::Dot(waypoints_[playerPositionData_.waypoints[playerId]].normalizedNextVector, vecWaypointPlayer);
        playerPositionData_.positionInWaypoint[playerId] = positionInWP;
        if (waypoints_[playerPositionData_.waypoints[playerId]].hasTwoNext)
        {
            Vec2f vecWaypointPlayer2 = playerPosition2d - waypoints_[playerPositionData_.waypoints[playerId]].position;
            float position2 = Vec2f::Dot(waypoints_[playerPositionData_.waypoints[playerId]].normalizedNextVector2, vecWaypointPlayer2);
            
            if (positionInWP > waypoints_[playerPositionData_.waypoints[playerId]].lengthNext)
            {
                float distWP1 = sqrt(
                        pow(waypoints_[waypoints_[playerPositionData_.waypoints[playerId]].nextWaypoint].position.x - playerPosition2d.x, 2) +
                        pow(waypoints_[waypoints_[playerPositionData_.waypoints[playerId]].nextWaypoint].position.y - playerPosition2d.y, 2));
                float distWP2 = sqrt(
                    pow(waypoints_[waypoints_[playerPositionData_.waypoints[playerId]].nextWaypoint2].position.x - playerPosition2d.x, 2) +
                    pow(waypoints_[waypoints_[playerPositionData_.waypoints[playerId]].nextWaypoint2].position.y - playerPosition2d.y, 2));
                if (distWP1 < distWP2)
                {
                    playerPositionData_.waypoints[playerId] = waypoints_[playerPositionData_.waypoints[playerId]].nextWaypoint;
                    playerPositionData_.waypointsCount[playerId]++;
                    playerPositionData_.positionInWaypoint[playerId] = Vec2f::Dot(waypoints_[playerPositionData_.waypoints[playerId]].normalizedNextVector, vecWaypointPlayer);
                }
                else
                {
                    
                }
            }
            else if (position2 > waypoints_[playerPositionData_.waypoints[playerId]].lengthNext2)
            {
                float distWP1 = sqrt(
                    pow(waypoints_[waypoints_[playerPositionData_.waypoints[playerId]].nextWaypoint].position.x - playerPosition2d.x, 2) +
                    pow(waypoints_[waypoints_[playerPositionData_.waypoints[playerId]].nextWaypoint].position.y - playerPosition2d.y, 2));
                float distWP2 = sqrt(
                    pow(waypoints_[waypoints_[playerPositionData_.waypoints[playerId]].nextWaypoint2].position.x - playerPosition2d.x, 2) +
                    pow(waypoints_[waypoints_[playerPositionData_.waypoints[playerId]].nextWaypoint2].position.y - playerPosition2d.y, 2));
                if (distWP1 < distWP2)
                {
                    
                }
                else
                {
                    playerPositionData_.waypoints[playerId] = waypoints_[playerPositionData_.waypoints[playerId]].nextWaypoint2;
                    playerPositionData_.waypointsCount[playerId]++;
                    playerPositionData_.positionInWaypoint[playerId] = Vec2f::Dot(waypoints_[playerPositionData_.waypoints[playerId]].normalizedNextVector2, vecWaypointPlayer2);
                }
            }
            else if (positionInWP < 0)
            {

                Vec2f previousVecWaypointPlayer = playerPosition2d - waypoints_[waypoints_[playerPositionData_.waypoints[playerId]].previousWaypoint].position;
                float previousPosition = Vec2f::Dot(waypoints_[waypoints_[playerPositionData_.waypoints[playerId]].previousWaypoint].normalizedNextVector, previousVecWaypointPlayer);
                
                if (waypoints_[waypoints_[playerPositionData_.waypoints[playerId]].previousWaypoint].lengthNext > previousPosition)
                {
                    playerPositionData_.waypoints[playerId] = waypoints_[playerPositionData_.waypoints[playerId]].previousWaypoint;
                    playerPositionData_.waypointsCount[playerId]--;
                    playerPositionData_.positionInWaypoint[playerId] = previousPosition;
                }
                else
                {
                    playerPositionData_.positionInWaypoint[playerId] = positionInWP;
                }
            }
        }
        else if (waypoints_[waypoints_[playerPositionData_.waypoints[playerId]].nextWaypoint].hasTwoPrevious)
        {
            float position2 = Vec2f::Dot(waypoints_[waypoints_[waypoints_[playerPositionData_.waypoints[playerId]].nextWaypoint].previousWaypoint2].normalizedNextVector, vecWaypointPlayer);
            if (positionInWP > waypoints_[playerPositionData_.waypoints[playerId]].lengthNext)
            {
                playerPositionData_.waypoints[playerId] = waypoints_[playerPositionData_.waypoints[playerId]].nextWaypoint;
                playerPositionData_.waypointsCount[playerId]++;
                playerPositionData_.positionInWaypoint[playerId] = positionInWP;
            }
            else if (positionInWP < 0)
            {
                float distWP1 = sqrt(
                    pow(waypoints_[playerPositionData_.waypoints[playerId]].position.x - playerPosition2d.x, 2) +
                    pow(waypoints_[playerPositionData_.waypoints[playerId]].position.y - playerPosition2d.y, 2));
                float distWP2 = sqrt(
                    pow(waypoints_[waypoints_[waypoints_[playerPositionData_.waypoints[playerId]].nextWaypoint2].previousWaypoint2].position.x - playerPosition2d.x, 2) +
                    pow(waypoints_[waypoints_[waypoints_[playerPositionData_.waypoints[playerId]].nextWaypoint2].previousWaypoint2].position.y - playerPosition2d.y, 2));
                if (distWP1 < distWP2)
                {
                    Vec2f previousVecWaypointPlayer = playerPosition2d - waypoints_[waypoints_[playerPositionData_.waypoints[playerId]].previousWaypoint].position;
                    float previousPosition = Vec2f::Dot(waypoints_[waypoints_[playerPositionData_.waypoints[playerId]].previousWaypoint].normalizedNextVector, previousVecWaypointPlayer);

                    if (waypoints_[waypoints_[playerPositionData_.waypoints[playerId]].previousWaypoint].lengthNext
                > previousPosition)
                    {
                        playerPositionData_.waypoints[playerId] = waypoints_[playerPositionData_.waypoints[playerId]].previousWaypoint;
                        playerPositionData_.waypointsCount[playerId]--;
                        playerPositionData_.positionInWaypoint[playerId] = previousPosition;
                    }
                    else
                    {
                        playerPositionData_.positionInWaypoint[playerId] = positionInWP;
                    }
                }
                else
                {
                    
                }
            }
            else if (position2 < 0)
            {
                float distWP1 = sqrt(
                    pow(waypoints_[playerPositionData_.waypoints[playerId]].position.x - playerPosition2d.x, 2) +
                    pow(waypoints_[playerPositionData_.waypoints[playerId]].position.y - playerPosition2d.y, 2));
                float distWP2 = sqrt(
                    pow(waypoints_[waypoints_[waypoints_[playerPositionData_.waypoints[playerId]].nextWaypoint2].previousWaypoint2].position.x - playerPosition2d.x, 2) +
                    pow(waypoints_[waypoints_[waypoints_[playerPositionData_.waypoints[playerId]].nextWaypoint2].previousWaypoint2].position.y - playerPosition2d.y, 2));
                if (distWP1 < distWP2)
                {
                    
                }
                else
                {
                    Vec2f previousVecWaypointPlayer = playerPosition2d - waypoints_[waypoints_[waypoints_[waypoints_[playerPositionData_.waypoints[playerId]].nextWaypoint].previousWaypoint2].previousWaypoint].position;
                    float previousPosition = Vec2f::Dot(waypoints_[waypoints_[waypoints_[waypoints_[playerPositionData_.waypoints[playerId]].nextWaypoint].previousWaypoint2].previousWaypoint].normalizedNextVector, previousVecWaypointPlayer);

                    if (waypoints_[waypoints_[waypoints_[waypoints_[playerPositionData_.waypoints[playerId]].nextWaypoint].previousWaypoint2].previousWaypoint].lengthNext2
                > previousPosition)
                    {
                        playerPositionData_.waypoints[playerId] = waypoints_[waypoints_[waypoints_[playerPositionData_.waypoints[playerId]].nextWaypoint].previousWaypoint2].previousWaypoint;
                        playerPositionData_.waypointsCount[playerId]--;
                        playerPositionData_.positionInWaypoint[playerId] = previousPosition;
                    }
                    else
                    {
                        playerPositionData_.positionInWaypoint[playerId] = positionInWP;
                    }
                }
            }
        }
        else
        {
            
            if (positionInWP > waypoints_[playerPositionData_.waypoints[playerId]].lengthNext)
            {
                playerPositionData_.waypoints[playerId] = waypoints_[playerPositionData_.waypoints[playerId]].nextWaypoint;
                playerPositionData_.waypointsCount[playerId]++;
                Vec2f nextVecWaypointPlayer = playerPosition2d - waypoints_[playerPositionData_.waypoints[playerId]].position;
                float nextPosition = Vec2f::Dot(waypoints_[playerPositionData_.waypoints[playerId]].normalizedNextVector, nextVecWaypointPlayer);
                playerPositionData_.positionInWaypoint[playerId] = nextPosition;
            }
            else if (positionInWP < 0)
            {
                Vec2f previousVecWaypointPlayer = playerPosition2d - waypoints_[waypoints_[playerPositionData_.waypoints[playerId]].previousWaypoint].position;
                float previousPosition = Vec2f::Dot(waypoints_[waypoints_[playerPositionData_.waypoints[playerId]].previousWaypoint].normalizedNextVector, previousVecWaypointPlayer);

                if (waypoints_[waypoints_[playerPositionData_.waypoints[playerId]].previousWaypoint].lengthNext
            > previousPosition)
                {
                    playerPositionData_.waypoints[playerId] = waypoints_[playerPositionData_.waypoints[playerId]].previousWaypoint;
                    playerPositionData_.waypointsCount[playerId]--;
                    playerPositionData_.positionInWaypoint[playerId] = previousPosition;
                }
                else
                {
                    playerPositionData_.positionInWaypoint[playerId] = positionInWP;
                }
            }
        }
    }

    void WaypointManager::CalculatePlayerPlacement()
    {
        for (int i = 0; i < engine_.GetComponentManagerContainer().playerManager.GetPlayerCount(); i++)
        {
            playerPositionData_.racePlacement[i] = i + 1;
        }
        for (int i = 0; i < engine_.GetComponentManagerContainer().playerManager.GetPlayerCount(); i++)
        {
            for (int j = i; j < engine_.GetComponentManagerContainer().playerManager.GetPlayerCount(); j++)
            {
                if (playerPositionData_.waypointsCount[i] > playerPositionData_.waypointsCount[j])
                {
                    if (playerPositionData_.racePlacement[i] < playerPositionData_.racePlacement[j])
                    {
                        RacePlacement racePlacementI = playerPositionData_.racePlacement[i];
                        playerPositionData_.racePlacement[i] = playerPositionData_.racePlacement[j];
                        playerPositionData_.racePlacement[j] = racePlacementI;
                    }
                }
                else if (playerPositionData_.waypointsCount[i] == playerPositionData_.waypointsCount[j])
                {
                    if (playerPositionData_.positionInWaypoint[i] > playerPositionData_.positionInWaypoint[j])
                    {
                        if (playerPositionData_.racePlacement[i] >  playerPositionData_.racePlacement[j])
                        {
                            RacePlacement racePlacementI = playerPositionData_.racePlacement[i];
                            playerPositionData_.racePlacement[i] = playerPositionData_.racePlacement[j];
                            playerPositionData_.racePlacement[j] = racePlacementI;
                        }
                    }
                    else
                    {
                        if (playerPositionData_.racePlacement[i] < playerPositionData_.racePlacement[j])
                        {
                            RacePlacement racePlacementI = playerPositionData_.racePlacement[i];
                            playerPositionData_.racePlacement[i] = playerPositionData_.racePlacement[j];
                            playerPositionData_.racePlacement[j] = racePlacementI;
                        }
                    }
                }
                else
                {
                    if (playerPositionData_.racePlacement[i] < playerPositionData_.racePlacement[j])
                    {
                        RacePlacement racePlacementI = playerPositionData_.racePlacement[i];
                        playerPositionData_.racePlacement[i] = playerPositionData_.racePlacement[j];
                        playerPositionData_.racePlacement[j] = racePlacementI;
                    }
                }
            }
            // if (i > 0)
            // {
            //     for (int j = i-1; j >= 0; j--)
            //     {
            //         if (playerPositionData_.waypointsCount[i] > playerPositionData_.waypointsCount[j])
            //         {
            //             playerPositionData_.racePlacement[j]++;
            //         }
            //         else if (playerPositionData_.waypointsCount[i] == playerPositionData_.waypointsCount[j])
            //         {
            //             if (playerPositionData_.positionInWaypoint[i] > playerPositionData_.positionInWaypoint[j])
            //             {
            //                 playerPositionData_.racePlacement[j]++;
            //             }
            //             else
            //             {
            //                 playerPositionData_.racePlacement[i]++;
            //             }
            //         }
            //         else
            //         {
            //             playerPositionData_.racePlacement[i]++;
            //         }
            //     }
            // }
            // else
            // {
            //     playerPositionData_.racePlacement[i] = 1;
            // }
        }
    }

    void WaypointManager::StartDetection()
    {
        WaypointIndex startWaypointIndex = 0;
        for (uint8_t i = 0; i < engine_.GetComponentManagerContainer().playerManager.GetPlayerCount(); i++)
        {
            playerPositionData_.waypoints[i] = startWaypointIndex;
            playerPositionData_.waypointsCount[i] = 0;
            CalculatePlayerPosition(engine_.GetComponentManagerContainer().playerManager.GetPlayerPosition(i), i);
        }
        CalculatePlayerPlacement();
        hasPlayersSpawned = true;
    }


    PlayerPositionData* WaypointManager::GetPlayerPositionData()
    {
        return &playerPositionData_;
    }

    
    void WaypointManager::Destroy()
    {
        
    }
}
