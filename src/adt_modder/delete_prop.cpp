#include <cstring>

#include "adt.h"
#include "adt_modder.h"
#include "fmt/core.h"

class DeletePropertyOp : public AdtModder::Op {
 public:
  static DeletePropertyOp& GetInstance() {
    static DeletePropertyOp op;
    return op;
  }

 private:
  AdtModder::Result Run(AdtModder::Adt adt_data, const nlohmann::json&) noexcept override;

  [[nodiscard]] const char* Help() const noexcept override {
    return "Deletes the given property for the given node in the ADT";
  }

  static bool s_initialized;
};

bool DeletePropertyOp::s_initialized = AdtModder::RegisterOperation(
    "delete_property", DeletePropertyOp::GetInstance());

AdtModder::Result DeletePropertyOp::Run(AdtModder::Adt adt_data,
                                 const nlohmann::json& command) noexcept{
  if (!command.contains("node") || !command["node"].is_string()) {
    fmt::print("Unable to find node in command\n");
    return AdtModder::Error::InvalidOperation;
  }
  if (!command.contains("property") || !command["property"].is_string()) {
    fmt::print("Unable to find property in command\n");
    return AdtModder::Error::InvalidOperation;
  }

  const std::string node_name = command["node"].get<std::string>();
  const std::string prop_name = command["property"].get<std::string>();

  uint8_t* data = adt_data.data();
  int node_offset = adt_path_offset(data, node_name.c_str());
  if (node_offset < 0) {
    fmt::print("Could not find node \"{}\"\n", node_name.c_str());
    return AdtModder::Error::NodeNotFound;
  }
  auto* prop = adt_get_property(data, node_offset, prop_name.c_str());
  if (prop == nullptr) {
    fmt::print("Could not find node \"{}\", prop \"{}\"\n", node_name,
               prop_name);
    return AdtModder::Error::PropertyNotFound;
  }

  int prop_offset =
      adt_get_property_offset(data, node_offset, prop_name.c_str());
  int next_prop_offset = adt_next_property_offset(data, prop_offset);

  uint32_t copy_length = adt_data.size() - next_prop_offset;
  memmove(&adt_data[prop_offset], &adt_data[next_prop_offset], copy_length);

  // The node now has one less property
  auto node = ADT_NODE(&adt_data[0], node_offset);
  node->property_count--;

  return AdtModder::Adt {data, adt_data.size() - (next_prop_offset - prop_offset)};
}
