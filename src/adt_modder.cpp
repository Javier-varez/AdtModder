#include "adt_modder.h"

#include <sstream>

#include "fmt/core.h"

std::unordered_map<std::string, AdtModder::Op*>& GetOperations() {
  static std::unordered_map<std::string, AdtModder::Op*> ops;
  return ops;
}

AdtModder::Result AdtModder::RunFromJson(AdtModder::Adt adt_data,
                            const nlohmann::json& op_array) {
  if (!op_array.is_array()) {
    fmt::print("AdtModder: Expected a json array.\n");
    return Error::MalformedJson;
  }

  auto& ops = GetOperations();
  for (auto& element : op_array) {
    if (!element.is_object()) {
      fmt::print("AdtModder: Expected a json object.\n");
      return Error::MalformedJson;
    }

    if (!element.contains("name")) {
      fmt::print("All operation objects should have a \"name\" property\n");
      return Error::MalformedJson;
    }

    std::string name = element["name"].get<std::string>();
    fmt::print("AdtModder: Running op with name: {}\n", name);

    // Find operation
    const auto op = ops.find(name);
    if (op == ops.cend()) {
      fmt::print("AdtModder: Unknown operation with name: \"{}\"\n", name);
      return Error::InvalidOperation;
    }

    adt_data = DITTO_PROPAGATE(op->second->Run(adt_data, element));
  }

  return adt_data;
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
