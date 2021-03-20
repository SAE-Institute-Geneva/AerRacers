#pragma once
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

 Author: Canas Simon
 Date:
---------------------------------------------------------- */
#include "vk/models/model_loader.h"

namespace neko::vk
{
class IModelManager : public SystemInterface
{
public:
	virtual ModelId LoadModel(std::string_view)                          = 0;
	[[nodiscard]] virtual const Model* GetModel(ModelId) const           = 0;
	[[nodiscard]] virtual std::string GetModelName(ModelId modelId)      = 0;
	[[nodiscard]] virtual std::string_view GetModelPath(ModelId modelId) = 0;
	[[nodiscard]] virtual bool IsLoaded(ModelId) const                   = 0;
};

class NullModelManager : public IModelManager
{
public:
	void Init() override {}
	void Update(seconds) override {}
	void Destroy() override {}

	ModelId LoadModel(std::string_view) override { return sole::uuid(); }
	[[nodiscard]] const Model* GetModel(ModelId) const override { return nullptr; }
	[[nodiscard]] std::string GetModelName(ModelId) override { return ""; };
	[[nodiscard]] std::string_view GetModelPath(ModelId) override { return ""; };
	[[nodiscard]] bool IsLoaded(ModelId) const override { return false; }
};

class ModelManager : public IModelManager
{
public:
	explicit ModelManager();

	void Init() override;
	void Update(seconds dt) override;
	void Destroy() override;

	ModelId LoadModel(std::string_view path) override;
	[[nodiscard]] const Model* GetModel(ModelId) const override;
	[[nodiscard]] std::string GetModelName(ModelId modelId) override;
	[[nodiscard]] std::string_view GetModelPath(ModelId modelId) override;
	[[nodiscard]] bool IsLoaded(ModelId) const override;

private:
	std::map<std::string, ModelId> modelPathMap_;
	std::map<ModelId, Model> models_;
	std::queue<ModelLoader> modelLoaders_;
};

using ModelManagerLocator = Locator<IModelManager, NullModelManager>;
}    // namespace neko::vk
