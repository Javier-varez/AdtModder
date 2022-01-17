#include <cstring>

#include "adt.h"
#include "adt_modder.h"
#include "fmt/core.h"

class ZeroOutPropertyOp : public AdtModder::Op {
 public:
  static ZeroOutPropertyOp& GetInstance() {
    static ZeroOutPropertyOp op;
    return op;
  }

 private:
  bool Run(Ditto::span<uint8_t> adt_data, const nlohmann::json&) override;
  const char* Help() const override;

  static bool s_initialized;
};

bool ZeroOutPropertyOp::s_initialized = AdtModder::RegisterOperation(
    "zero_out_property", ZeroOutPropertyOp::GetInstance());

const char* ZeroOutPropertyOp::Help() const {
  return "Writes 0's to the given property value";
}

bool ZeroOutPropertyOp::Run(Ditto::span<uint8_t> adt_data,
                            const nlohmann::json& command) {
  if (!command.contains("node") || !command["node"].is_string()) {
    fmt::print("Unable to find node in command\n");
    return false;
  }
  if (!command.contains("property") || !command["property"].is_string()) {
    fmt::print("Unable to find property in command\n");
    return false;
  }
  const std::string node = command["node"].get<std::string>();
  const std::string prop_name = command["property"].get<std::string>();

  uint8_t* data = adt_data.data();
  int node_offset = adt_path_offset(data, node.c_str());
  if (node_offset < 0) {
    fmt::print("Could not look find node \"%s\"\n", node.c_str());
    return false;
  }
  auto* prop = adt_get_property(data, node_offset, prop_name.c_str());
  if (prop == nullptr) {
    fmt::print("Could not look find node \"{}\", prop \"{}\"\n", node,
               prop_name);
    return false;
  }

  memset(&prop->value[0], 0, prop->size);
  return true;
}
