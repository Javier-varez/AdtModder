#include "adt_modder.h"

#include <sstream>

#include "fmt/core.h"

std::unordered_map<std::string, AdtModder::Op*>& GetOperations() {
  static std::unordered_map<std::string, AdtModder::Op*> ops;
  return ops;
}

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
