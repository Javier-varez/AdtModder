#include <cstring>

#include "adt.h"
#include "adt_modder.h"
#include "fmt/core.h"

class AddNodeOp : public AdtModder::Op {
public:
  static AddNodeOp &GetInstance() {
    static AddNodeOp op;
    return op;
  }

private:
  AdtModder::Result Run(AdtModder::Adt adt_data,
                        const nlohmann::json &) noexcept override;

  [[nodiscard]] const char *Help() const noexcept override {
    return "Adds the given node to the adt in the specified path. If the "
           "parent node doesn't exist it fails";
  }

  static bool s_initialized;
};

bool AddNodeOp::s_initialized =
    AdtModder::RegisterOperation("add_node", AddNodeOp::GetInstance());

AdtModder::Result AddNodeOp::Run(AdtModder::Adt adt_data,
                                 const nlohmann::json &command) noexcept {
  if (!command.contains("node") || !command["node"].is_string()) {
    fmt::print("Unable to find node in command\n");
    return AdtModder::Error::InvalidOperation;
  }

  const std::string node_name = command["node"].get<std::string>();

  // Check if node already exists
  {
    const auto child_node_offset =
        adt_path_offset(adt_data.data(), node_name.c_str());
    if (child_node_offset > 0) {
      return AdtModder::Error::NodeAlreadyExists;
    }
  }

  const auto last_node_separator_index = node_name.find_last_of('/');
  const std::string parent_node_name =
      node_name.substr(0, last_node_separator_index);
  const std::string child_node_name =
      node_name.substr(last_node_separator_index + 1);

  const auto parent_node_offset =
      adt_path_offset(adt_data.data(), parent_node_name.c_str());
  const auto next_sibling_offset =
      adt_next_sibling_offset(adt_data.data(), parent_node_offset);

  // New node will be empty, just header + name property (prop_value =
  // child_node_name + '\0')
  const size_t new_node_size = sizeof(adt_node_hdr) + sizeof(adt_property) +
                               child_node_name.length() + 1;

  const auto prev_size = adt_data.size();
  const auto new_size = prev_size + new_node_size;
  adt_data.resize(new_size);

  // Move the mem upwards
  memmove(&adt_data[next_sibling_offset + new_node_size],
          &adt_data[next_sibling_offset], prev_size - next_sibling_offset);
  auto parent_node = ADT_NODE(adt_data.data(), parent_node_offset);
  // Increase the child count
  parent_node->child_count++;

  // And now write the child!
  const auto child_offset = next_sibling_offset;
  auto new_node = ADT_NODE(adt_data.data(), child_offset);
  new_node->property_count = 1; // Just the name
  new_node->child_count = 0;    // No children

  // Filling name property
  auto name_prop =
      ADT_PROP(adt_data.data(),
               adt_first_property_offset(adt_data.data(), child_offset));
  name_prop->size = child_node_name.length() + 1;
  memcpy(&name_prop->name[0], "name", 5);
  memcpy(&name_prop->value[0], child_node_name.c_str(), name_prop->size);

  return AdtModder::Result::ok();
}
