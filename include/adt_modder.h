#ifndef ADT_MODDER_H_
#define ADT_MODDER_H_

#include <string_view>
#include <unordered_map>

#include "ditto/result.h"
#include "ditto/span.h"

#include "nlohmann/json.hpp"

class AdtModder {
 public:
  enum class Error {
      MalformedJson,
      InvalidOperation,
      NodeNotFound,
      PropertyNotFound,

  };

  static std::string_view error_to_string(Error err) {
    switch (err) {
      case Error::MalformedJson:
        return "Malformed Json";
      case Error::InvalidOperation:
        return "Invalid operation";
      case Error::NodeNotFound:
        return "Node not found";
      case Error::PropertyNotFound:
        return "Property not found";
    }
  }

  using Adt = std::vector<uint8_t>&;
  using Result = Ditto::Result<void, Error>;

  class Op {
   public:
    virtual Result Run(Adt adt_data, const nlohmann::json&) noexcept = 0;
    [[nodiscard]] virtual const char* Help() const noexcept {
      return "No help available for this command";
    }

    virtual ~Op() = default;
  };

  Result RunFromJson(Adt adt_data, const nlohmann::json& json) noexcept;
  [[nodiscard]] std::string Help() const noexcept;

  static bool RegisterOperation(std::string_view name, Op& operation) noexcept;
};

#endif  // ADT_MODDER_H_
