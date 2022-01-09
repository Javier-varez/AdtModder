#ifndef ADT_MODDER_H_
#define ADT_MODDER_H_

#include <string_view>
#include <unordered_map>

#include "ditto/span.h"
#include "nlohmann/json.hpp"

class AdtModder {
 public:
  class Op {
   public:
    virtual bool Run(Ditto::span<uint8_t> adt_data, const nlohmann::json&) = 0;
    virtual const char* Help() const {
      return "No help available for this command";
    }

    virtual ~Op() = default;
  };

  bool RunFromJson(Ditto::span<uint8_t> adt_data, const nlohmann::json& json);
  std::string Help() const;

  static bool RegisterOperation(std::string_view name, Op& operation);
};

#endif  // ADT_MODDER_H_
