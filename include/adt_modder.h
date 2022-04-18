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
    NodeAlreadyExists,
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
    case Error::NodeAlreadyExists:
      return "Node already exists";
    }
  }

  using Adt = std::vector<uint8_t> &;
  using Result = Ditto::Result<void, Error>;

  class Op {
  public:
    virtual Result Run(Adt adt_data, const nlohmann::json &) noexcept = 0;
    [[nodiscard]] virtual const char *Help() const noexcept {
      return "No help available for this command";
    }

    virtual ~Op() = default;
  };

  Result RunFromJson(Adt adt_data, const nlohmann::json &json) noexcept;
  [[nodiscard]] std::string Help() const noexcept;

  static bool RegisterOperation(std::string_view name, Op &operation) noexcept;

  static Ditto::Result<uint32_t, Error> ParseU32(const std::string &string);
  static Ditto::Result<uint64_t, Error> ParseU64(const std::string &string);
};

#endif // ADT_MODDER_H_
