#pragma once
#include "vk/models/model_loader.h"

namespace neko::vk
{
class IModelManager : public SystemInterface
{
public:
	virtual ModelId LoadModel(const std::string&) = 0;
	[[nodiscard]] virtual const Model* GetModel(ModelId) const = 0;
	[[nodiscard]] virtual bool IsLoaded(ModelId) = 0;
};

class NullModelManager : public IModelManager
{
public:
	void Init() override {}
	void Update(seconds) override {}
	void Destroy() override {}

	ModelId LoadModel(const std::string&) override { return sole::uuid(); }
	[[nodiscard]] const Model* GetModel(ModelId) const override { return nullptr; }
	[[nodiscard]] bool IsLoaded(ModelId) override { return false; }
};

class ModelManager : public IModelManager
{
public:
	explicit ModelManager();

	void Init() override;
	void Update(seconds dt) override;
	void Destroy() override;

	ModelId LoadModel(const std::string& path) override;
	[[nodiscard]] const Model* GetModel(ModelId) const override;
	[[nodiscard]] bool IsLoaded(ModelId) override;

private:
	std::map<std::string, ModelId> modelPathMap_;
	std::map<ModelId, Model> models_;
	std::queue<ModelLoader> modelLoaders_;
	Assimp::Importer importer_;
};

using ModelManagerLocator = Locator<IModelManager, NullModelManager>;
}
