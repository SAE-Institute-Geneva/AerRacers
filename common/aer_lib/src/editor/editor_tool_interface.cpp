#include "editor/editor_tool_interface.h"

#include "aer_engine.h"

namespace neko::aer {
EditorToolInterface::EditorToolInterface(
    AerEngine& engine,
    ToolType type,
    int id,
    std::string name)
    : kId_(id),
      name_(name),
      type_(type),
      engine_(engine)
{}

std::string EditorToolInterface::GetName() const
{
    return name_;
}

int EditorToolInterface::GetId() const
{
    return kId_;
}

EditorToolInterface::ToolType EditorToolInterface::GetType() const
{
    return type_;
}

}
