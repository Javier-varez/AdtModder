#include "adt_modder.h"

#include <sstream>

#include "adt.h"
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

class RandomizePropertyOp : public AdtModder::Op {
 public:
  static RandomizePropertyOp& GetInstance() {
    static RandomizePropertyOp op;
    return op;
  }

 private:
  bool Run(Ditto::span<uint8_t> adt_data, const nlohmann::json&) override;
  const char* Help() const override;

  static bool s_initialized;
};

std::unordered_map<std::string, AdtModder::Op*>& GetOperations() {
  static std::unordered_map<std::string, AdtModder::Op*> ops;
  return ops;
}

bool ZeroOutPropertyOp::s_initialized = AdtModder::RegisterOperation(
    "zero_out_property", ZeroOutPropertyOp::GetInstance());
bool RandomizePropertyOp::s_initialized = AdtModder::RegisterOperation(
    "randomize_property", RandomizePropertyOp::GetInstance());

bool AdtModder::RunFromJson(Ditto::span<uint8_t> adt_data,
                            const nlohmann::json& op_array) {
  if (!op_array.is_array()) {
    fmt::print("AdtModder: Expected a json array.\n");
    return false;
  }

  auto& ops = GetOperations();
  for (auto& element : op_array) {
    if (!element.is_object()) {
      fmt::print("AdtModder: Expected a json object.\n");
      return false;
    }

    if (!element.contains("name")) {
      fmt::print("All operation objects should have a \"name\" property\n");
      return false;
    }

    std::string name = element["name"].get<std::string>();
    fmt::print("AdtModder: Running op with name: {}\n", name);

    // Find operation
    const auto op = ops.find(name);
    if (op == ops.cend()) {
      fmt::print("AdtModder: Unknown operation with name: \"{}\"\n", name);
      return false;
    }

    if (!op->second->Run(adt_data, element)) {
      fmt::print("AdtModder: Error running operation with name: \"{}\"\n",
                 name);
      return false;
    }
  }

  return true;
}

std::string AdtModder::Help() const {
  std::stringstream stream;
  stream << "\nSupported Adt Modder commands:\n";
  for (const auto& [name, op] : GetOperations()) {
    stream << fmt::format("Command \"{}\": {}\n", name, op->Help());
  }
  return stream.str();
}

bool AdtModder::RegisterOperation(std::string_view name, Op& operation) {
  GetOperations()[std::string{name}] = &operation;
  return true;
}

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

const char* RandomizePropertyOp::Help() const {
  return "Randomizes a property value in the given adt";
}
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
