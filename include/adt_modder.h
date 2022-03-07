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

  using Adt = Ditto::span<uint8_t>;
  using Result = Ditto::Result<Adt, Error>;

  class Op {
   public:
    virtual Result Run(Adt adt_data, const nlohmann::json&) = 0;
    virtual const char* Help() const {
      return "No help available for this command";
    }

    virtual ~Op() = default;
  };

  Result RunFromJson(Adt adt_data, const nlohmann::json& json);
  std::string Help() const;

  static bool RegisterOperation(std::string_view name, Op& operation);
};

#endif  // ADT_MODDER_H_
