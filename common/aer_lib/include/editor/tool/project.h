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
 
 Author : Dylan von Arx
 Co-Author : 
 Date : 25.11.2020
---------------------------------------------------------- */
#include <SDL_events.h>

#include "editor/editor_tool_interface.h"

namespace neko::aer
{
class Project;

struct DataResource
{
	DataResource(std::string name, std::filesystem::path path);
	~DataResource();

	// Displays the resource in an interactive menu with imgui
	virtual void Display(Project& projectManager) = 0;

	virtual DataResource* GetResourceIsSelected() = 0;
	std::string GetName() const;
	bool GetIsSelect();

	std::filesystem::path GetPath();
	bool isHover_ = false;
protected:
	std::filesystem::path path_;
	std::string name_;

	bool isSelect_ = false;
};

struct DataFile : public DataResource
{
	enum class Type
	{
		None,
		Data,
		Text,
		Model,
		Prefab,
		Scene,
		Material,
		Texture,
		Sound,
		Font,
		Shader
	};

	DataFile(std::string name, std::filesystem::path path, Type type);

	DataResource* GetResourceIsSelected() override;

	Type GetType();

	void Display(Project& projectManager) override;

private:
	Type type_ = Type::None;
};

struct DataFolder : public DataResource
{
	DataFolder(std::string name, std::filesystem::path path);

	void Display(Project& projectManager) override;

	int GetSize();

	// Retrieves the resource that has been selected
	virtual DataResource* GetResourceIsSelected();
	[[nodiscard]] std::vector<DataResource*> GetResources() const;
private:
	std::vector<DataResource*> resources_;
};

class Project final : public EditorToolInterface
{
public:
	explicit Project(ToolType type, int id, std::string name);
	void Init() override;
	void Update(seconds dt) override;
	void DrawImGui() override;
	void Destroy() override;
	void OnEvent(const SDL_Event& event) override;

	void LoadData(const std::string path);
	bool OpenFile(const std::filesystem::path& path) override;
	void SetEditorToolManager(EditorToolManager* editorToolManager);


	void Open() const;

	bool contextMenuIsOpen_ = false;
	bool openFile_ = false;
	DataResource* resourceHovered_ = nullptr;
private:
	std::string path_ = "./../../data/";
	DataResource* resources_;

	EditorToolManager* editorToolManager_;
};
}
