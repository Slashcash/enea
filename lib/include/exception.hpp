#ifndef EXCEPTION_HPP
#define EXCEPTION_HPP

#include <stdexcept>

#include <fmt/format.h>
#include <magic_enum.hpp>
#include <nlohmann/json.hpp>

namespace enea {
/**
 * @brief This is the base class for our exceptions.
 * All our exceptions are supposed to inherit from this.
 *
 */
class Exception : public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

namespace json {
class Exception : public enea::Exception
{
 public:
    Exception() = delete;
    explicit Exception(const std::string& msg, const nlohmann::json& json)
        : enea::Exception("null"),
          mMsg(fmt::format("{}. This is the json that caused the exception:\n{}", msg, json.dump())) {};

    inline const char* what() const noexcept override
    {
        return mMsg.c_str();
    }

 private:
    std::string mMsg;
};

class MissingMandatoryFieldException : public enea::json::Exception
{
 public:
    MissingMandatoryFieldException() = delete;
    inline MissingMandatoryFieldException(const nlohmann::json& json, const std::string_view& missingField)
        : enea::json::Exception(fmt::format(R"(Missing mandatory "{}" field in json)", missingField), json) {};
};

template <typename T, typename = void> class CannotConvertException : public enea::json::Exception
{
 public:
    CannotConvertException() = delete;
    explicit inline CannotConvertException(const nlohmann::json& json)
        : enea::json::Exception(
              fmt::format(R"(Unsuitable conversion performed on json element which is of type "{}")", json.type_name()),
              json) {};
};

template <typename T>
class CannotConvertException<T, std::enable_if_t<std::is_enum_v<T>>> : public enea::json::Exception
{
 public:
    CannotConvertException() = delete;
    explicit inline CannotConvertException(const nlohmann::json& json)
        : enea::json::Exception(
              fmt::format(
                  R"(Unsuitable conversion performed on json element which cannot be converted to an element of the {} enum)",
                  magic_enum::enum_type_name<T>()),
              json) {};
};

} // namespace json
} // namespace enea
#endif // EXCEPTION_HPP
