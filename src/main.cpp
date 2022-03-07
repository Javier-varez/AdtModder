#include "adt_modder.h"
#include "argparse/argparse.hpp"
#include "fileio.h"
#include "fmt/core.h"
#include "nlohmann/json.hpp"

Ditto::Result<void, File::Error> run(int argc, char* argv[]) {
  AdtModder modder;
  argparse::ArgumentParser program("adt_modder");

  program.add_argument("device_tree").help("Input ADT to modify");
  program.add_argument("operations.json")
      .help("A json file with the operations to perform on the dt");
  program.add_argument("-o", "--output")
      .default_value(std::string{"modded_adt.bin"});
  program.add_epilog(modder.Help());

  try {
    program.parse_args(argc, argv);
  } catch (const std::runtime_error& exc) {
    fmt::print("{}", exc.what());
    std::exit(1);
  }

  const std::string original_dt_name = program.get<std::string>("device_tree");
  const std::string dest_dt_name = program.get<std::string>("-o");
  const std::string op_path = program.get<std::string>("operations.json");

  auto op_file = DITTO_PROPAGATE(File::Open(op_path.c_str()));
  auto op_data = DITTO_PROPAGATE(op_file.ReadAll());
  const auto operations = nlohmann::json::parse(op_data);

  auto original_dt = DITTO_PROPAGATE(File::Open(original_dt_name.c_str()));
  auto dt_data = DITTO_PROPAGATE(original_dt.ReadAll());

  auto mod_result = modder.RunFromJson(dt_data, operations);
  if (mod_result.is_error()) {
    fmt::print("Error running commands: {}", AdtModder::error_to_string(mod_result.error_value()));
    exit(1);
  }

  const auto final_data = mod_result.ok_value();

  File dest_dt = DITTO_PROPAGATE(File::Create(dest_dt_name.c_str()));
  auto result = dest_dt.Write(final_data);
  if (result.is_error()) {
    return result.error_value();
  }

  return Ditto::Result<void, File::Error>::ok();
}

int main(int argc, char* argv[]) {
  srand(time(nullptr));
  auto result = run(argc, argv);
  if (result.is_error()) {
    fmt::print("Error running command {}",
               static_cast<uint32_t>(result.error_value()));
    return -1;
  }
  return 0;
}
