#include <cstring>

#include "adt.h"
#include "adt_modder.h"
#include "fmt/core.h"

class AddPropertyOp : public AdtModder::Op {
public:
  static AddPropertyOp &GetInstance() {
    static AddPropertyOp op;
    return op;
  }

private:
  AdtModder::Result Run(AdtModder::Adt adt_data,
                        const nlohmann::json &) noexcept override;

  [[nodiscard]] const char *Help() const noexcept override {
    return "Adds a new property to the provided node in the ADT";
  }

  using Command = std::tuple<std::string, std::string, std::vector<uint8_t>>;

  static Ditto::Result<Command, AdtModder::Error>
  ParseCommand(const nlohmann::json &command) noexcept;

  static bool s_initialized;
};

bool AddPropertyOp::s_initialized =
    AdtModder::RegisterOperation("add_property", AddPropertyOp::GetInstance());

AdtModder::Result AddPropertyOp::Run(AdtModder::Adt adt_data,
                                     const nlohmann::json &command) noexcept {
  const auto [node_name, property_name, value] =
      DITTO_PROPAGATE(AddPropertyOp::ParseCommand(command));

  const auto node_offset = adt_path_offset(adt_data.data(), node_name.c_str());
  if (node_offset < 0) {
    fmt::print("Node not found {}", node_name);
    return AdtModder::Error::NodeNotFound;
  }

  const auto node = ADT_NODE(adt_data.data(), node_offset);

  const auto insertion_offset =
      adt_first_child_offset(adt_data.data(), node_offset);

  // Calculate size of the property
  const size_t property_size = sizeof(adt_property) + value.size();

  // Resize the vector
  const auto old_size = adt_data.size();
  const auto new_size = old_size + property_size;
  adt_data.resize(new_size);

  // Shift the data, making space for the property
  memmove(&adt_data[insertion_offset + property_size],
          &adt_data[insertion_offset], old_size - insertion_offset);

  // Increase property count
  node->property_count++;

  const auto property = ADT_PROP(adt_data.data(), insertion_offset);
  memcpy(property->name, property_name.c_str(), property_name.length());
  property->name[property_name.length()] = '\0';

  property->size = value.size();
  memcpy(&property->value[0], value.data(), value.size());

  return AdtModder::Result::ok();
}

Ditto::Result<AddPropertyOp::Command, AdtModder::Error>
AddPropertyOp::ParseCommand(const nlohmann::json &command) noexcept {
  if (!command.contains("node") || !command["node"].is_string()) {
    fmt::print("Unable to find node in command\n");
    return AdtModder::Error::InvalidOperation;
  }

  if (!command.contains("property") || !command["property"].is_string()) {
    fmt::print("Unable to find node in command\n");
    return AdtModder::Error::InvalidOperation;
  }

  const std::string node_name = command["node"].get<std::string>();
  const std::string property_name = command["property"].get<std::string>();
  if (property_name.length() > MAX_PROPERTY_NAME_LENGTH) {
    fmt::print("Property name is too long `{}`", property_name);
    return AdtModder::Error::InvalidOperation;
  }

  std::vector<uint8_t> value;

  if (!command.contains("value")) {
    fmt::print("Unable to find node in command\n");
    return AdtModder::Error::InvalidOperation;
  }

  if (command["value"].is_string()) {
    const std::string value_string = command["value"].get<std::string>();
    value.resize(value_string.length() + 1);
    memcpy(value.data(), value_string.c_str(), value_string.length());
    value[value_string.length()] = '\0';
  } else if (command["value"].is_object()) {
    const auto value_object = command["value"].get<nlohmann::json>();

    if (!value_object.contains("type") || !value_object["type"].is_string()) {
      fmt::print("Unknown value type in command\n");
      return AdtModder::Error::InvalidOperation;
    }

    if (!value_object.contains("contents")) {
      fmt::print("Unknown value contents in command\n");
      return AdtModder::Error::InvalidOperation;
    }

    const auto type = value_object["type"].get<std::string>();

    if (type == "u64") {
      if (!value_object["contents"].is_string()) {
        fmt::print("Type u64 should be a string in json\n");
        return AdtModder::Error::InvalidOperation;
      }
      const auto contents = value_object["contents"].get<std::string>();
      uint64_t parsed = DITTO_PROPAGATE(AdtModder::ParseU64(contents));
      for (size_t i = 0; i < sizeof(uint64_t); i++) {
        value.push_back(parsed & 0xFF);
        parsed >>= 8;
      }
    } else if (type == "u32") {
      if (!value_object["contents"].is_string()) {
        fmt::print("Type u32 should be a string in json\n");
        return AdtModder::Error::InvalidOperation;
      }
      const auto contents = value_object["contents"].get<std::string>();
      uint64_t parsed = DITTO_PROPAGATE(AdtModder::ParseU32(contents));
      for (size_t i = 0; i < sizeof(uint64_t); i++) {
        value.push_back(parsed & 0xFF);
        parsed >>= 8;
      }
    } else if (type == "u64[]") {
      if (!value_object["contents"].is_array()) {
        fmt::print("Type u32 should be a string in json\n");
        return AdtModder::Error::InvalidOperation;
      }

      for (nlohmann::json &content_element :
           value_object["contents"].get<std::vector<nlohmann::json>>()) {
        if (!content_element.is_string()) {
          fmt::print("Expected string in u64[]\n");
          return AdtModder::Error::InvalidOperation;
        }

        uint64_t parsed = DITTO_PROPAGATE(
            AdtModder::ParseU32(content_element.get<std::string>()));
        for (size_t i = 0; i < sizeof(uint64_t); i++) {
          value.push_back(parsed & 0xFF);
          parsed >>= 8;
        }
      }
    } else {
      fmt::print("Unknown value type in command");
      return AdtModder::Error::InvalidOperation;
    }
  } else {
    fmt::print("Unknown value type in command");
    return AdtModder::Error::InvalidOperation;
  }

  return std::tuple{node_name, property_name, value};
}
