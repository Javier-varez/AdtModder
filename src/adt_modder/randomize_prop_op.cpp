#include <cstring>

#include "adt.h"
#include "adt_modder.h"
#include "fmt/core.h"

class RandomizePropertyOp : public AdtModder::Op {
 public:
  static RandomizePropertyOp& GetInstance() {
    static RandomizePropertyOp op;
    return op;
  }

 private:
  bool Run(Ditto::span<uint8_t> adt_data, const nlohmann::json&) override;

  const char* Help() const override {
    return "Randomizes a property value in the given adt";
  }

  static bool s_initialized;
};

bool RandomizePropertyOp::s_initialized = AdtModder::RegisterOperation(
    "randomize_property", RandomizePropertyOp::GetInstance());

bool RandomizePropertyOp::Run(Ditto::span<uint8_t> adt_data,
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
    fmt::print("Could not look find node \"{}\"\n", node.c_str());
    return false;
  }
  auto* prop = adt_get_property(data, node_offset, prop_name.c_str());
  if (prop == nullptr) {
    fmt::print("Could not look find node \"{}\", prop \"{}\"\n", node,
               prop_name);
    return false;
  }

  for (size_t i = 0; i < prop->size; i++) {
    prop->value[i] = rand();
  }
  return true;
}
