#include "adt_modder.h"

#include <sstream>

#include "fmt/core.h"

std::unordered_map<std::string, AdtModder::Op *> &GetOperations() {
  static std::unordered_map<std::string, AdtModder::Op *> ops;
  return ops;
}

AdtModder::Result
AdtModder::RunFromJson(AdtModder::Adt adt_data,
                       const nlohmann::json &op_array) noexcept {
  if (!op_array.is_array()) {
    fmt::print("AdtModder: Expected a json array.\n");
    return Error::MalformedJson;
  }

  auto &ops = GetOperations();
  for (auto &element : op_array) {
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

    auto result = op->second->Run(adt_data, element);
    if (result.is_error()) {
      fmt::print("AdtModder: Error running operation \"{}\"\n", name);
      return result.error_value();
    }
  }

  return AdtModder::Result::ok();
}

std::string AdtModder::Help() const noexcept {
  std::stringstream stream;
  stream << "\nSupported Adt Modder commands:\n";
  for (const auto &[name, op] : GetOperations()) {
    stream << fmt::format("Command \"{}\": {}\n", name, op->Help());
  }
  return stream.str();
}

bool AdtModder::RegisterOperation(std::string_view name,
                                  Op &operation) noexcept {
  GetOperations()[std::string{name}] = &operation;
  return true;
}

Ditto::Result<uint32_t, AdtModder::Error>
AdtModder::ParseU32(const std::string &string) {
  int base = 10;
  const char *ptr = string.c_str();
  if (string.starts_with("0x")) {
    base = 16;
    ptr = string.c_str() + 2;
  }

  try {
    return std::strtoul(string.c_str(), nullptr, base);
  } catch (std::out_of_range &e) {
    fmt::print("Invalid u32 in string: {}, {}", string, e.what());
    return AdtModder::Error::InvalidOperation;
  } catch (std::invalid_argument &e) {
    fmt::print("Invalid u32 in string: {}, {}", string, e.what());
    return AdtModder::Error::InvalidOperation;
  }
}

Ditto::Result<uint64_t, AdtModder::Error>
AdtModder::ParseU64(const std::string &string) {
  int base = 10;
  const char *ptr = string.c_str();
  if (string.starts_with("0x")) {
    base = 16;
    ptr = string.c_str() + 2;
  }

  try {
    return std::strtoull(string.c_str(), nullptr, base);
  } catch (std::out_of_range &e) {
    fmt::print("Invalid u64 in string: {}, {}", string, e.what());
    return AdtModder::Error::InvalidOperation;
  } catch (std::invalid_argument &e) {
    fmt::print("Invalid u64 in string: {}, {}", string, e.what());
    return AdtModder::Error::InvalidOperation;
  }
}
