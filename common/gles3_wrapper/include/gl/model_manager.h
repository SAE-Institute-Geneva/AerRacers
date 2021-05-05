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
 */
#include <queue>

#include "gl/model_loader.h"

namespace neko::gl
{
class IModelManager
{
public:
	[[nodiscard]] virtual Model* GetModelPtr(ModelId) = 0;
	[[nodiscard]] virtual const Model* GetModel(ModelId) const           = 0;
	[[nodiscard]] virtual std::string GetModelName(ModelId modelId)      = 0;
	[[nodiscard]] virtual std::string_view GetModelPath(ModelId modelId) = 0;

	[[nodiscard]] virtual bool IsLoaded(ModelId) const = 0;
	virtual ModelId LoadModel(std::string_view)        = 0;
	/**
	 * \brief Nb of models hat has been loaded
	 */
	[[nodiscard]] virtual int CountModelLoaded() const = 0;
	/**
	 * \brief Nb of models hat has been loaded
	 */
	[[nodiscard]] virtual int CountModelNotLoaded() const = 0;
	/**
	 * \brief Nb of models in total
	 */
	[[nodiscard]] virtual int CountOfAllModel() const = 0;
};

class NullModelManager : public IModelManager
{
public:
	[[nodiscard]] Model* GetModelPtr(ModelId) override { return nullptr; }
	[[nodiscard]] const Model* GetModel(ModelId) const override { return nullptr; }
	[[nodiscard]] std::string GetModelName(ModelId) override { return ""; };
	[[nodiscard]] std::string_view GetModelPath(ModelId) override { return ""; };

	[[nodiscard]] bool IsLoaded(ModelId) const override { return false; }
	ModelId LoadModel(std::string_view) override { return INVALID_MODEL_ID; }
	[[nodiscard]] int CountModelLoaded() const override { return 0; }
	[[nodiscard]] int CountModelNotLoaded() const override { return 0; }
	[[nodiscard]] int CountOfAllModel() const override { return 0; }
};

class ModelManager : public IModelManager, public SystemInterface
{
public:
	ModelManager();

	void Init() override;
	void Update(seconds) override;
	void Destroy() override;
	
	[[nodiscard]] Model* GetModelPtr(ModelId modelId) override;
	[[nodiscard]] const Model* GetModel(ModelId modelId) const override;
	[[nodiscard]] std::string GetModelName(ModelId modelId) override;
	[[nodiscard]] std::string_view GetModelPath(ModelId modelId) override;

	[[nodiscard]] bool IsLoaded(ModelId modelId) const override { return GetModel(modelId); }
	ModelId LoadModel(std::string_view path) override;
	[[nodiscard]] int CountModelLoaded() const override { return modelMap_.size(); }
	[[nodiscard]] int CountModelNotLoaded() const override { return modelLoaders_.size(); }
	[[nodiscard]] int CountOfAllModel() const override { return modelPathMap_.size(); }

	void SetTexture(ModelId modelId,
		const std::string& texturelPath,
		gl::Mesh::Texture::Type textureType);

private:
	std::map<std::string, ModelId> modelPathMap_ {};
	std::map<ModelId, Model> modelMap_ {};
	std::queue<ModelLoader> modelLoaders_ {};
};

using ModelManagerLocator = Locator<IModelManager, NullModelManager>;
}
