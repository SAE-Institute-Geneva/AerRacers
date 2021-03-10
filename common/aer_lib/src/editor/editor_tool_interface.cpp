#include "aer/editor/editor_tool_interface.h"

#include "aer/aer_engine.h"

namespace neko::aer
{
EditorToolInterface::EditorToolInterface(
	AerEngine& engine, ToolType type, int id, std::string_view name)
   : kId_(id), name_(name), type_(type), engine_(engine)
{}

std::string_view EditorToolInterface::GetName() const { return name_; }

int EditorToolInterface::GetId() const { return kId_; }

EditorToolInterface::ToolType EditorToolInterface::GetType() const { return type_; }

}    // namespace neko::aer
