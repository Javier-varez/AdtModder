#include <errno.h>
#include <fcntl.h>
#include <fmt/core.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "adt.h"

uint8_t* dt_data = nullptr;

void zero_out_property(const char* node, const char* prop_name) {
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

void randomize_property(const char* node, const char* prop_name) {
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

int main() {
  // Seed rng
  srand(time(nullptr));

  int fd = open("original_dt.bin", O_RDONLY);
  if (fd < 0) {
    fmt::print("Error opening original dt {}\n", strerror(errno));
    return 0;
  }

  constexpr static uint32_t LEN = 1024 * 1024;
  dt_data = reinterpret_cast<uint8_t*>(malloc(LEN));
  const uint32_t size = read(fd, dt_data, LEN);
  if (size == 0) {
    close(fd);
    fmt::print("DT size should not be 0...\n");
    return 0;
  }

  zero_out_property("/", "serial-number");
  zero_out_property("/", "mlb-serial-number");
  zero_out_property("/", "region-info");
  zero_out_property("/", "time-stamp");
  zero_out_property("/", "model-config");
  randomize_property("/chosen", "apfs-preboot-uuid");
  randomize_property("/chosen", "boot-uuid");
  zero_out_property("/chosen", "boot-objects-path");
  zero_out_property("/chosen", "system-volume-auth-blob");
  randomize_property("/chosen", "mac-address-bluetooth0");
  randomize_property("/chosen", "mac-address-wifi0");
  randomize_property("/chosen/secure-boot-hashes", "ansf");
  randomize_property("/chosen/secure-boot-hashes", "ciof");
  randomize_property("/chosen/secure-boot-hashes", "tmuf");
  randomize_property("/chosen/secure-boot-hashes", "sfr-manifest-hash");
  randomize_property("/chosen/secure-boot-hashes", "aubt");
  randomize_property("/chosen/secure-boot-hashes", "dcpf");
  randomize_property("/chosen/secure-boot-hashes", "logo");
  randomize_property("/chosen/secure-boot-hashes", "lpol");
  randomize_property("/chosen/secure-boot-hashes", "ibot");
  randomize_property("/chosen/secure-boot-hashes", "dcp2");
  randomize_property("/chosen/secure-boot-hashes", "aopf");
  randomize_property("/chosen/secure-boot-hashes", "anef");
  randomize_property("/chosen/secure-boot-hashes", "avef");
  randomize_property("/chosen/secure-boot-hashes", "gfxf");
  randomize_property("/chosen/secure-boot-hashes", "ispf");
  randomize_property("/chosen/secure-boot-hashes", "pmpf");
  randomize_property("/chosen/secure-boot-hashes", "siof");
  randomize_property("/chosen/secure-boot-hashes", "dtre");
  randomize_property("/chosen/secure-boot-hashes", "ibdt");
  randomize_property("/chosen/secure-boot-hashes", "isys");
  randomize_property("/chosen/secure-boot-hashes", "system-volume-auth-blob");
  randomize_property("/chosen/secure-boot-hashes", "csys");
  randomize_property("/chosen/secure-boot-hashes",
                     "base-system-volume-auth-blob");
  randomize_property("/chosen/secure-boot-hashes", "bstc");
  randomize_property("/chosen/secure-boot-hashes", "trst");
  randomize_property("/chosen/secure-boot-hashes", "fuos");
  randomize_property("/chosen/secure-boot-hashes", "state");
  randomize_property("/arm-io/wlan", "local-mac-address");
  randomize_property("/arm-io/wlan", "wifi-calibration-msf");
  randomize_property("/arm-io/bluetooth", "bluetooth-taurus-calibration-bf");
  randomize_property("/arm-io/bluetooth", "bluetooth-taurus-calibration");
  randomize_property("/arm-io/bluetooth", "local-mac-address");
  randomize_property("/arm-io/usb-drd0", "host-mac-address");
  randomize_property("/arm-io/usb-drd0", "device-mac-address");
  randomize_property("/arm-io/usb-drd1", "host-mac-address");
  randomize_property("/arm-io/usb-drd1", "device-mac-address");
  randomize_property("/arm-io/usb-drd2", "host-mac-address");
  randomize_property("/arm-io/usb-drd2", "device-mac-address");

  int destfd = open("dt.bin", O_RDWR | O_CREAT, S_IRWXU | S_IRWXG | S_IROTH);
  if (destfd < 0) {
    fmt::print("Error opening dest dt {}\n", strerror(errno));
    close(fd);
    return 0;
  }
  const uint32_t write_size = write(destfd, dt_data, size);
  if (size != write_size) {
    fmt::print("Could not write new dt: ({}) {}\n", write_size,
               strerror(errno));
  }

  close(fd);
  close(destfd);

  return 0;
}
