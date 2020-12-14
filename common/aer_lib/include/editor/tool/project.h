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
#include <filesystem>

#include "editor/editor_tool_interface.h"
#include "gl/texture.h"

namespace neko::aer {

	struct DataResource
	{
		DataResource(std::string name, std::filesystem::path path); 
		~DataResource();

		
		virtual void Display()
		{
			
		}
		virtual void OpenFile()
		{
			
		}
		virtual DataResource* GetResourceIsSelected() = 0
		{
			
		}

		std::string GetName() const;

		bool GetIsSelect()
		{
			return isSelect_;
		}

		std::filesystem::path GetPath()
		{
			return path_;
		}
		bool isHover_= false;
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

		DataResource* GetResourceIsSelected() override
		{
			if (isSelect_) {
				isSelect_ = false;
				return this;
			}
		}

		Type GetType()
		{
			return type_;
		}

		void Display() override;
		void OpenFile() override;
		
	private:
		Type type_ = Type::None;

	};

	struct DataFolder : public DataResource
	{
		DataFolder(std::string name, std::filesystem::path path);

		void Display() override;

		[[nodiscard]] std::vector<DataResource*> GetDataResources() const;
		int GetFileNumber() const;
		
		std::vector<DataResource*> GetRessources()
		{
			return this->resources_;
		}

		virtual DataResource* GetResourceIsSelected()
		{
			if (isHover_)
			{
				return this;
			}
			else
			{
				for (auto element : resources_)
				{
					DataResource* resource = element->GetResourceIsSelected();
					if(resource != nullptr)
					{
							return resource;
					}
				}
			}

			return nullptr;
		}
	
		
	private:
		std::vector<DataResource*> resources_;
		int fileNumber_ = 0;
		
	};

class Project final : public EditorToolInterface {
public:
    explicit Project(ToolType type, int id, std::string name);
    void Init() override;
    void Update(seconds dt) override;
    void DrawImGui() override;
    void Destroy() override;
    void OnEvent(const SDL_Event& event) override;

	void LoadData(const std::string path);

private:
    int posY_ = 0;
	std::string path_ = "./../../data/";
	DataResource* resources_;
	bool contextMenuIsOpen_ = false;
	DataResource* resourceHovered_ = nullptr;
	gl::TextureManager textureManager_;

	bool mouseButtonRight_ = false;
	bool mouseButtonLeft_ = false;
};

}
