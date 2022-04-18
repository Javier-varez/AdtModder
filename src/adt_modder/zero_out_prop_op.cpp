#include <cstring>

#include "adt.h"
#include "adt_modder.h"
#include "fmt/core.h"

class ZeroOutPropertyOp : public AdtModder::Op {
 public:
  static ZeroOutPropertyOp& GetInstance() noexcept {
    static ZeroOutPropertyOp op;
    return op;
  }

 private:
  AdtModder::Result Run(AdtModder::Adt adt_data, const nlohmann::json&) noexcept override;
  [[nodiscard]] const char* Help() const noexcept override;

  static bool s_initialized;
};

bool ZeroOutPropertyOp::s_initialized = AdtModder::RegisterOperation(
    "zero_out_property", ZeroOutPropertyOp::GetInstance());

const char* ZeroOutPropertyOp::Help() const noexcept {
  return "Writes 0's to the given property value";
}

AdtModder::Result ZeroOutPropertyOp::Run(AdtModder::Adt adt_data,
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

  memset(&prop->value[0], 0, prop->size);
  return AdtModder::Result::ok();
}
