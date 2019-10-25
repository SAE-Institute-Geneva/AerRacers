#include <tools/inspector.h>
#include <tools/neko_editor.h>
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>
#include <utilities/file_utility.h>
#include <engine/log.h>

namespace neko::editor
{

void Inspector::ShowEntityInfo(neko::Entity entity) const
{
	if (entity == neko::INVALID_ENTITY)
		return;


	ImGui::InputText("Entity Name: ", &sceneManager_.GetCurrentScene().entitiesNames[entity]);
	ImGui::LabelText("Entity Value: ", "%u", entity);
	{
		const auto parentEntity = transformManager_.GetParentEntity(entity);
		if (parentEntity != neko::INVALID_ENTITY)
		{
			ImGui::LabelText("Entity Parent", "%u", parentEntity);
		}
	}
	if (entityManager_.HasComponent(entity, neko::EntityMask(neko::NekoComponentType::PREFAB)))
	{
		const auto index = sceneManager_.GetComponent(entity);
		if (index != neko::INVALID_INDEX)
		{
			ImGui::LabelText("Prefab Index", "%u", index);
		}
	}
	if (entityManager_.HasComponent(entity, neko::EntityMask(neko::NekoComponentType::POSITION2D)))
	{

		bool keepComponent = true;

		if (ImGui::CollapsingHeader("Position2d Component", &keepComponent, ImGuiTreeNodeFlags_DefaultOpen))
		{
			auto& pos = positionManager_.GetComponent(entity);
			float posRaw[2] = { pos.x, pos.y };
			if (ImGui::InputFloat2("Position", posRaw))
			{
				positionManager_.SetComponent(entity, sf::Vector2f(posRaw[0], posRaw[1]));
			}
		}


		if (!keepComponent)
		{
			positionManager_.DestroyComponent(entityManager_, entity);
		}
	}

	if (entityManager_.HasComponent(entity, neko::EntityMask(neko::NekoComponentType::SCALE2D)))
	{

		bool keepComponent = true;
		if (ImGui::CollapsingHeader("Scale2d Component", &keepComponent, ImGuiTreeNodeFlags_DefaultOpen))
		{
			auto& scale = scaleManager_.GetComponent(entity);
			float scaleRaw[2] = { scale.x, scale.y };
			if (ImGui::InputFloat2("Scale", scaleRaw))
			{
				scaleManager_.SetComponent(entity, sf::Vector2f(scaleRaw[0], scaleRaw[1]));
			}
		}

		if (!keepComponent)
		{
			scaleManager_.DestroyComponent(entityManager_, entity);
		}
	}

	if (entityManager_.HasComponent(entity, neko::EntityMask(neko::NekoComponentType::ROTATION2D)))
	{

		bool keepComponent = true;
		if (ImGui::CollapsingHeader("Angle2d Component", &keepComponent, ImGuiTreeNodeFlags_DefaultOpen))
		{
			const auto& angle = rotationManager_.GetComponent(entity);
			float tmpAngle = angle;
			if (ImGui::InputFloat("Angle", &tmpAngle))
			{
				rotationManager_.SetComponent(entity, tmpAngle);
			}
		}

		if (!keepComponent)
		{
			rotationManager_.DestroyComponent(entityManager_, entity);
		}

	}
	if (entityManager_.HasComponent(entity, neko::EntityMask(neko::NekoComponentType::SPRITE2D)))
	{
		bool keepComponent = true;
		if (ImGui::CollapsingHeader("Sprite2d Component", &keepComponent, ImGuiTreeNodeFlags_DefaultOpen))
		{

			auto& sprite = spriteManager_.GetComponent(entity);
			const auto& textureName = textureManager_.GetTexturePath(sprite.textureId);
			if (ImGui::Button(textureName.data()))
			{
				ImGui::OpenPopup("Texture Popup");

			}
			ImGui::SameLine();
			ImGui::Text("Texture");

			if (ImGui::BeginPopup("Texture Popup"))
			{
				ImGui::Text("Texture Browser");
				ImGui::Separator();
				if (ImGui::Selectable("No Texture"))
				{
					spriteManager_.CopyTexture(neko::INVALID_INDEX, entity, 1);
				}
				for (neko::Index i = 0; i < textureManager_.GetTextureCount(); i++)
				{
					if (ImGui::Selectable(textureManager_.GetTexturePath(i).data()))
					{
						spriteManager_.CopyTexture(i, entity, 1);
					}
				}
				ImGui::EndPopup();
			}
			auto& origin = sprite.origin;

			float originRaw[2] = { origin.x, origin.y };

			if (ImGui::InputFloat2("Origin", originRaw))
			{
				origin.x = originRaw[0];
				origin.y = originRaw[1];
			}
			ImGui::PushID("Sprite Layer");
			ImGui::InputInt("Layer", &sprite.layer, 1);
			ImGui::PopID();
		}

		if (!keepComponent)
		{
			spriteManager_.DestroyComponent(entityManager_, entity);
		}
	}

	if (entityManager_.HasComponent(entity, neko::EntityMask(neko::NekoComponentType::SPINE_ANIMATION)))
	{

		auto& spineDrawable = spineManager_.GetComponent(entity);
		auto& spineDrawableInfo = spineManager_.GetInfo(entity);
		bool keepComponent = true;
		if (ImGui::CollapsingHeader("Spine2d Component", &keepComponent, ImGuiTreeNodeFlags_DefaultOpen))
		{
			std::string spineButtonName = spineDrawableInfo.spinePath;
			if (spineButtonName.empty())
			{
				spineButtonName = "None";
			}
			if (ImGui::Button(spineButtonName.c_str()))
			{
				ImGui::OpenPopup("Spine Popup");
			}
			ImGui::SameLine();
			ImGui::Text("Spine");
			ImGui::LabelText("Atlas", "%s", spineDrawableInfo.atlasPath.c_str());
			ImGui::LabelText("Skeleton", "%s", spineDrawableInfo.skeletonDataPath.c_str());

			static std::vector<std::string> spineFileList;
			if (ImGui::BeginPopup("Spine Popup"))
			{
				if (spineFileList.empty())
				{
					neko::IterateDirectory(nekoEditor_.config.dataRootPath, [](const std::string_view filename)
						{
							if (filename.find(".spine") != std::string_view::npos)
							{
								spineFileList.push_back(filename.data());
							}
						}, true);
				}
				ImGui::Selectable("No Spine File");

				for (auto& spineFilename : spineFileList)
				{
					if (ImGui::Selectable(spineFilename.c_str()))
					{
						auto spineJson = neko::LoadJson(spineFilename);
						spineDrawableInfo.atlasPath = spineJson["atlas"];
						spineDrawableInfo.skeletonDataPath = spineJson["skeleton"];
						spineFileList.clear();
					}

				}
				ImGui::EndPopup();
			}
			else
			{
				spineFileList.clear();
			}


			ImGui::PushID("Load Spine Component");
			if (ImGui::Button("Load..."))
			{
				if (spineManager_.AddSpineDrawable(entity, spineDrawableInfo.atlasPath,
					spineDrawableInfo.skeletonDataPath))
				{
					logDebug("Successfully load spine drawable");
				}
				else
				{
					logDebug("[Error] Could not load spine drawable");
				}
			}
			ImGui::PopID();
			ImGui::SameLine();
			ImGui::PushID("Preview Spine Component");
			if (ImGui::Button("Preview..."))
			{
				if (spineDrawable.skeletonDrawable != nullptr)
				{
					auto& previewer = nekoEditor_.GetPreviewer();
					previewer.SetSpineAnimation(&spineDrawable);
				}
				else
				{
					logDebug("[Error] Load the spine component first");
				}
			}
			ImGui::PopID();
		}
		if (!keepComponent)
		{
			spineManager_.DestroyComponent(entityManager_, entity);
		}
		ImGui::Separator();
	}

	if (entityManager_.HasComponent(entity, neko::EntityMask(neko::NekoComponentType::BODY2D)))
	{
		bool keepComponent = true;
		if (ImGui::CollapsingHeader("Body2d Component", &keepComponent, ImGuiTreeNodeFlags_DefaultOpen))
		{
			auto& bodyDef = bodyDefManager_.GetComponent(entity);
			const char* bodyTypeMap[3] =
			{
					"Static",
					"Kinematic",
					"Dynamic"
			};
			ImGui::Combo("Body Type", (int*)(&bodyDef.type), bodyTypeMap, 3);
			ImGui::InputFloat("Gravity Scale", &bodyDef.gravityScale);
			ImGui::Checkbox("Fixed Rotation", &bodyDef.fixedRotation);
			if (!bodyDef.fixedRotation &&
				!entityManager_.HasComponent(entity, neko::EntityMask(neko::NekoComponentType::ROTATION2D)))
			{
				ImGui::SameLine();
				ImGui::TextColored(ImColor(255, 0, 0), "Warning!");
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::TextColored(ImColor(255, 0, 0), "[Warning] Rotation without Angle component");
					ImGui::EndTooltip();
				}
			}
		}

		if (!keepComponent)
		{
			bodyDefManager_.DestroyComponent(entityManager_, entity);
		}
	}
	if (entityManager_.HasComponent(entity, neko::EntityMask(neko::NekoComponentType::BOX_COLLIDER2D)))
	{
		bool keepComponent = true;
		if (ImGui::CollapsingHeader("Box Collider 2D Component", &keepComponent, ImGuiTreeNodeFlags_DefaultOpen))
		{
			auto& boxColliderDef = boxColliderDefManager_.GetComponent(entity);

			auto& isSensor = boxColliderDef.fixtureDef.isSensor;
			ImGui::Checkbox("Is Sensor", &isSensor);

			float size[2] = { boxColliderDef.size.x, boxColliderDef.size.y };
			if (ImGui::InputFloat2("Box Size", size))
			{
				boxColliderDef.size = sf::Vector2f(size[0], size[1]);
			}
			float offset[2] = { boxColliderDef.offset.x, boxColliderDef.offset.y };
			if (ImGui::InputFloat2("Box Offset", offset))
			{
				boxColliderDef.offset = sf::Vector2f(offset[0], offset[1]);
			}

		}
	}
	if (entityManager_.HasComponent(entity, neko::EntityMask(neko::NekoComponentType::CIRCLE_COLLIDER2D)))
	{
		bool keepComponent = true;
		if (ImGui::CollapsingHeader("Circle Collider 2D Component", &keepComponent, ImGuiTreeNodeFlags_DefaultOpen))
		{
			auto& circleColliderDef = circleColliderDefManager_.GetComponent(entity);

			auto& isSensor = circleColliderDef.fixtureDef.isSensor;
			ImGui::Checkbox("Is Sensor", &isSensor);

			ImGui::InputFloat("Circle Radius", &circleColliderDef.shapeDef.m_radius);

			float offset[2] = { circleColliderDef.shapeDef.m_p.x, circleColliderDef.shapeDef.m_p.y };
			if (ImGui::InputFloat2("Circle Offset", offset))
			{
				circleColliderDef.shapeDef.m_p = b2Vec2(offset[0], offset[1]);
			}
		}
	}
	if (entityManager_.HasComponent(entity, neko::EntityMask(neko::NekoComponentType::POLYGON_COLLIDER2D)))
	{
		bool keepComponent = true;
		if (ImGui::CollapsingHeader("Polygon Collider 2D Component", &keepComponent, ImGuiTreeNodeFlags_DefaultOpen))
		{

		}
	}


	if (ImGui::Button("Add Component"))
		ImGui::OpenPopup("Component Popup");
	const static std::map<neko::NekoComponentType, std::string> componentNameMap =
	{
			{neko::NekoComponentType::TRANSFORM2D,       "Transform 2D"},
			{neko::NekoComponentType::POSITION2D,        "Position 2D"},
			{neko::NekoComponentType::SCALE2D,            "Scale 2D"},
			{neko::NekoComponentType::ROTATION2D,         "Angle 2D"},
			{neko::NekoComponentType::SPRITE2D,           "Sprite 2D"},
			{neko::NekoComponentType::BODY2D,             "Body 2D"},
			{neko::NekoComponentType::BOX_COLLIDER2D,     "Box Collider 2D"},
			{neko::NekoComponentType::CIRCLE_COLLIDER2D,  "Circle Collider 2D"},
			{neko::NekoComponentType::POLYGON_COLLIDER2D, "Polygon Collider 2D"},
			{neko::NekoComponentType::SPINE_ANIMATION,    "Spine Animation 2D"},
	};

	ImGui::SameLine();
	if (ImGui::BeginPopup("Component Popup"))
	{
		ImGui::Text("Component");
		ImGui::Separator();
		for (auto& component : componentNameMap)
		{
			if (entityManager_.HasComponent(entity, neko::EntityMask(component.first)))
				continue;
			if (ImGui::Selectable(component.second.c_str(), false))
			{
				entityManager_.AddComponentType(entity, neko::EntityMask(component.first));
				switch (component.first)
				{
				case neko::NekoComponentType::SCALE2D:
				{
					scaleManager_.AddComponent(entityManager_, entity);
					break;
				}
				case neko::NekoComponentType::POSITION2D:
				{
					positionManager_.AddComponent(entityManager_, entity);
					break;
				}
				case neko::NekoComponentType::ROTATION2D:
				{
					rotationManager_.AddComponent(entityManager_, entity);
					break;
				}
				case neko::NekoComponentType::TRANSFORM2D:
				{

					positionManager_.AddComponent(entityManager_, entity);
					scaleManager_.AddComponent(entityManager_, entity);
					rotationManager_.AddComponent(entityManager_, entity);
					break;
				}
				case neko::NekoComponentType::SPRITE2D:
				{
					spriteManager_.AddComponent(entityManager_, entity);
					break;
				}
				case neko::NekoComponentType::SPINE_ANIMATION:
				{
					spineManager_.AddComponent(entityManager_, entity);
					break;
				}
				case neko::NekoComponentType::BODY2D:
				{

					bodyDefManager_.AddComponent(entityManager_, entity);
					if (!entityManager_.HasComponent(entity, neko::EntityMask(neko::NekoComponentType::POSITION2D)))
					{
						positionManager_.AddComponent(entityManager_, entity);
					}
					break;
				}
				case neko::NekoComponentType::BOX_COLLIDER2D:
				{
					boxColliderDefManager_.AddComponent(entityManager_, entity);
					break;
				}
				case neko::NekoComponentType::CIRCLE_COLLIDER2D:
				{
					circleColliderDefManager_.AddComponent(entityManager_, entity);
					break;
				}
				case neko::NekoComponentType::POLYGON_COLLIDER2D:
				{
					polygonColliderDefManager_.AddComponent(entityManager_, entity);
					break;
				}
				case neko::NekoComponentType::CONVEX_SHAPE2D:
					break;
				default:
					break;
				}
				ImGui::CloseCurrentPopup();
				break;
			}
		}
		ImGui::EndPopup();
	}
}

Inspector::Inspector(NekoEditorExport& nekoEditorExport) :
	transformManager_(nekoEditorExport.transform2dManager),
	sceneManager_(nekoEditorExport.sceneManager),
	entityManager_(nekoEditorExport.entityManager),
	positionManager_(nekoEditorExport.position2dManager),
	scaleManager_(nekoEditorExport.scale2dManager),
	rotationManager_(nekoEditorExport.rotation2dManager),
	spriteManager_(nekoEditorExport.spriteManager),
	textureManager_(nekoEditorExport.textureManager),
	spineManager_(nekoEditorExport.spineManager),
	nekoEditor_(nekoEditorExport.editor),
	bodyDefManager_(nekoEditorExport.bodyDef2dManager),
	boxColliderDefManager_(nekoEditorExport.boxColliderDefManager_),
	circleColliderDefManager_(nekoEditorExport.circleColliderDefManager_),
	polygonColliderDefManager_(nekoEditorExport.polygonColldierDefManager_)
{
}

void Inspector::BeginWindow()
{
	ImGui::Begin("Inspector", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
}

void Inspector::EndWindow()
{
	ImGui::End();
}
}