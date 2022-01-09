#include <stdlib.h>
#include <time.h>

#include "adt.h"
#include "argparse/argparse.hpp"
#include "fileio.h"
#include "fmt/core.h"

uint8_t* dt_data = nullptr;

void zero_out_property(Ditto::span<uint8_t> data, const char* node,
                       const char* prop_name) {
  uint8_t* dt_data = data.data();
  int node_offset = adt_path_offset(dt_data, node);
  if (node_offset < 0) {
    fmt::print("Could not look find node \"%s\"\n", node);
    return;
  }
  auto* prop = adt_get_property(dt_data, node_offset, prop_name);
  if (prop == nullptr) {
    fmt::print("Could not look find node \"{}\", prop \"{}\"\n", node,
               prop_name);
    return;
  }

  memset(&prop->value[0], 0, prop->size);
}

void randomize_property(Ditto::span<uint8_t> data, const char* node,
                        const char* prop_name) {
  uint8_t* dt_data = data.data();
  int node_offset = adt_path_offset(dt_data, node);
  if (node_offset < 0) {
    fmt::print("Could not look find node \"{}\"\n", node);
    return;
  }
  auto* prop = adt_get_property(dt_data, node_offset, prop_name);
  if (prop == nullptr) {
    fmt::print("Could not look find node \"{}\", prop \"{}\"\n", node,
               prop_name);
    return;
  }

  for (size_t i = 0; i < prop->size; i++) {
    prop->value[i] = rand();
  }
}

Ditto::Result<void, File::Error> run(int argc, char* argv[]) {
  argparse::ArgumentParser program("adt_modder");

  program.add_argument("device_tree").help("Input ADT to modify");
  program.add_argument("-o", "--output")
      .default_value(std::string{"modded_adt.bin"});

  try {
    program.parse_args(argc, argv);
  } catch (const std::runtime_error& exc) {
    fmt::print("{}", exc.what());
    std::exit(1);
  }

  // Seed rng
  srand(time(nullptr));

  const std::string original_dt_name = program.get<std::string>("device_tree");
  const std::string dest_dt_name = program.get<std::string>("-o");
  fmt::print("Opening file {}\n", original_dt_name);
  auto original_dt = DITTO_PROPAGATE(File::Open(original_dt_name.c_str()));
  auto dt_data = DITTO_PROPAGATE(original_dt.ReadAll());

  fmt::print("Working on the given dt...\n");
  zero_out_property(dt_data, "/", "serial-number");
  zero_out_property(dt_data, "/", "mlb-serial-number");
  zero_out_property(dt_data, "/", "region-info");
  zero_out_property(dt_data, "/", "time-stamp");
  zero_out_property(dt_data, "/", "model-config");
  randomize_property(dt_data, "/chosen", "apfs-preboot-uuid");
  randomize_property(dt_data, "/chosen", "boot-uuid");
  zero_out_property(dt_data, "/chosen", "boot-objects-path");
  zero_out_property(dt_data, "/chosen", "system-volume-auth-blob");
  randomize_property(dt_data, "/chosen", "mac-address-bluetooth0");
  randomize_property(dt_data, "/chosen", "mac-address-wifi0");
  randomize_property(dt_data, "/chosen/secure-boot-hashes", "ansf");
  randomize_property(dt_data, "/chosen/secure-boot-hashes", "ciof");
  randomize_property(dt_data, "/chosen/secure-boot-hashes", "tmuf");
  randomize_property(dt_data, "/chosen/secure-boot-hashes",
                     "sfr-manifest-hash");
  randomize_property(dt_data, "/chosen/secure-boot-hashes", "aubt");
  randomize_property(dt_data, "/chosen/secure-boot-hashes", "dcpf");
  randomize_property(dt_data, "/chosen/secure-boot-hashes", "logo");
  randomize_property(dt_data, "/chosen/secure-boot-hashes", "lpol");
  randomize_property(dt_data, "/chosen/secure-boot-hashes", "ibot");
  randomize_property(dt_data, "/chosen/secure-boot-hashes", "dcp2");
  randomize_property(dt_data, "/chosen/secure-boot-hashes", "aopf");
  randomize_property(dt_data, "/chosen/secure-boot-hashes", "anef");
  randomize_property(dt_data, "/chosen/secure-boot-hashes", "avef");
  randomize_property(dt_data, "/chosen/secure-boot-hashes", "gfxf");
  randomize_property(dt_data, "/chosen/secure-boot-hashes", "ispf");
  randomize_property(dt_data, "/chosen/secure-boot-hashes", "pmpf");
  randomize_property(dt_data, "/chosen/secure-boot-hashes", "siof");
  randomize_property(dt_data, "/chosen/secure-boot-hashes", "dtre");
  randomize_property(dt_data, "/chosen/secure-boot-hashes", "ibdt");
  randomize_property(dt_data, "/chosen/secure-boot-hashes", "isys");
  randomize_property(dt_data, "/chosen/secure-boot-hashes",
                     "system-volume-auth-blob");
  randomize_property(dt_data, "/chosen/secure-boot-hashes", "csys");
  randomize_property(dt_data, "/chosen/secure-boot-hashes",
                     "base-system-volume-auth-blob");
  randomize_property(dt_data, "/chosen/secure-boot-hashes", "bstc");
  randomize_property(dt_data, "/chosen/secure-boot-hashes", "trst");
  randomize_property(dt_data, "/chosen/secure-boot-hashes", "fuos");
  randomize_property(dt_data, "/chosen/secure-boot-hashes", "state");
  randomize_property(dt_data, "/arm-io/wlan", "local-mac-address");
  randomize_property(dt_data, "/arm-io/wlan", "wifi-calibration-msf");
  randomize_property(dt_data, "/arm-io/bluetooth",
                     "bluetooth-taurus-calibration-bf");
  randomize_property(dt_data, "/arm-io/bluetooth",
                     "bluetooth-taurus-calibration");
  randomize_property(dt_data, "/arm-io/bluetooth", "local-mac-address");
  randomize_property(dt_data, "/arm-io/usb-drd0", "host-mac-address");
  randomize_property(dt_data, "/arm-io/usb-drd0", "device-mac-address");
  randomize_property(dt_data, "/arm-io/usb-drd1", "host-mac-address");
  randomize_property(dt_data, "/arm-io/usb-drd1", "device-mac-address");
  randomize_property(dt_data, "/arm-io/usb-drd2", "host-mac-address");
  randomize_property(dt_data, "/arm-io/usb-drd2", "device-mac-address");

  File dest_dt = DITTO_PROPAGATE(File::Create(dest_dt_name.c_str()));
  auto result = dest_dt.Write(dt_data);
  if (result.is_error()) {
    return result.error_value();
  }

  return Ditto::Result<void, File::Error>::ok();
}

int main(int argc, char* argv[]) {
  auto result = run(argc, argv);
  if (result.is_error()) {
    fmt::print("Error running command {}",
               static_cast<uint32_t>(result.error_value()));
    return -1;
  }
  return 0;
}
