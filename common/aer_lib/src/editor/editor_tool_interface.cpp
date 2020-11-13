#include "editor/editor_tool_interface.h"

namespace neko::aer {
EditorToolInterface::EditorToolInterface(
    ToolType type,
    int id,
    std::string name)
    : kId_(id),
      name_(name),
      type_(type) {}

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
