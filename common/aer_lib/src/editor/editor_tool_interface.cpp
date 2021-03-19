#include "aer/aer_engine.h"

namespace neko::aer
{
EditorToolInterface::EditorToolInterface(AerEngine& engine) : engine_(engine)
{
	engine.RegisterSystem(*this);
	engine.RegisterOnEvent(*this);
}
}    // namespace neko::aer
