#include "editor/editor_tool_interface.h"

namespace neko::aer {
EditorToolInterface::EditorToolInterface(ToolType type, int id)
  : type_(type), id_(id) {
  name_ = toolNames_[(int)type];
}

std::string EditorToolInterface::GetName() const {
  return name_;
}

int EditorToolInterface::GetId() const {
  return id_;
}

EditorToolInterface::ToolType EditorToolInterface::GetType() const {
  return type_;
}

}
