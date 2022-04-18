#include <cstring>

#include "adt.h"
#include "adt_modder.h"
#include "fmt/core.h"

class ReplacePropertyOp : public AdtModder::Op {
 public:
  static ReplacePropertyOp& GetInstance() {
    static ReplacePropertyOp op;
    return op;
  }

 private:
  AdtModder::Result Run(AdtModder::Adt adt_data, const nlohmann::json&) noexcept override;
  [[nodiscard]] const char* Help() const noexcept override {
    return "Replaces the contents of the property by the given value";
  }

  static bool s_initialized;
};

bool ReplacePropertyOp::s_initialized = AdtModder::RegisterOperation(
    "replace_property", ReplacePropertyOp::GetInstance());

AdtModder::Result ReplacePropertyOp::Run(AdtModder::Adt adt_data,
                            const nlohmann::json& command) noexcept {
  if (!command.contains("node") || !command["node"].is_string()) {
    fmt::print("Unable to find node in command\n");
    return AdtModder::Error::InvalidOperation;
  }
  if (!command.contains("property") || !command["property"].is_string()) {
    fmt::print("Unable to find property in command\n");
    return AdtModder::Error::InvalidOperation;
  }
  const std::string node = command["node"].get<std::string>();
  const std::string prop_name = command["property"].get<std::string>();

  if (!command.contains("value")) {
    fmt::print("Unable to find value in command\n");
    return AdtModder::Error::InvalidOperation;
  }

  if (!command["value"].is_string()) {
    fmt::print(
        "Only strings are supported at this time to replace a property\n");
    return AdtModder::Error::InvalidOperation;
  }

  uint8_t* data = adt_data.data();
  int node_offset = adt_path_offset(data, node.c_str());
  if (node_offset < 0) {
    fmt::print("Could not look find node \"%s\"\n", node.c_str());
    return AdtModder::Error::NodeNotFound;
  }
  auto* prop = adt_get_property(data, node_offset, prop_name.c_str());
  if (prop == nullptr) {
    fmt::print("Could not look find node \"{}\", prop \"{}\"\n", node,
               prop_name);
    return AdtModder::Error::PropertyNotFound;
  }

  const std::string new_value = command["value"].get<std::string>();
  if (new_value.length() + 1 > prop->size) {
    fmt::print(
        "The requested string value \"%s\" "
        "for property \"%s\" exceeds the size "
        "of the property (%d).\n",
        new_value.c_str(), prop->name, prop->size);
    return AdtModder::Error::InvalidOperation;
  }

  strncpy(reinterpret_cast<char*>(&prop->value[0]), new_value.c_str(),
          prop->size);
  return AdtModder::Result::ok();
}
