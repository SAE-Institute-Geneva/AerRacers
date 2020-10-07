#pragma once
#include <sdl_engine/sdl_engine.h>
#include <draw_system.h>
#include <Editor/Tool/editor.h>

namespace neko::aer
{
	class AerEngine final : public sdl::SdlEngine
	{
	public:
		explicit AerEngine(Configuration* config = nullptr);

		void Init() override;

		void Destroy() override;

		void ManageEvent() override;

		void GenerateUiFrame() override;

	private:
		DrawSystem drawSystem_;
		Editor editorSystem_;
		//Initialization data
		const size_t kInitEntityNmb_ = 10;
	};
}
