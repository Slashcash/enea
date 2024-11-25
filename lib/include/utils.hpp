#ifndef UTILS_HPP
#define UTILS_HPP

#include <magic_enum.hpp>
#include <nlohmann/json.hpp>

#include "exception.hpp"

template <typename T>
concept OptionalCompatible = requires { typename std::optional<T>; };

template <typename T>
concept JsonConvertible = requires(T value) {
    { nlohmann::json{value} };
};

template <typename T>
concept JsonConstructible = requires(nlohmann::json json) {
    T{json}; // Check if T can be constructed from a nlohmann::json
};

namespace utils {
/**
 * @brief Helper function that adds a value wrapped in an std::optional into a json.
 * If the std::optional contains a value it is added to the json. If the std::optional is empty
 * it insteads does nothing
 *
 * @param json The json in which the value will be added
 * @param field The json label for the value
 * @param value the std::optional supposedly containing the value
 */
template <typename T>
    requires JsonConvertible<T> && OptionalCompatible<T>
inline void addOptionalToJson(nlohmann::json& json, const std::string_view& field, const std::optional<T>& value)
{
    if (value)
    {
        json[field] = *value;
    }
}

/**
 * @brief Helper function that gets the value from a json available at the given label.
 * If there is no value attached to that json label the function will return an empty optional
 *
 * @param json The json in which to look for the value
 * @param field The json label of the value to retrieve
 * @return The value found in the json, if any
 */
template <typename T>
    requires JsonConstructible<T> && OptionalCompatible<T>
inline std::optional<T> getOptionalValueFromJson(const nlohmann::json& json, const std::string_view& field)
{
    try
    {
        return json.at(field).get<T>();
    }
    catch ([[maybe_unused]] const nlohmann::json::out_of_range& e)
    {
        return std::nullopt;
    }
}

template <typename T>
    requires JsonConstructible<T>
inline T getMandatoryValueFromJson(const nlohmann::json& json, const std::string_view& field)
{
    return convert<T>(getMandatoryValueFromJson<nlohmann::json>(json, field));
}

template <>
inline nlohmann::json getMandatoryValueFromJson<nlohmann::json>(const nlohmann::json& json,
                                                                const std::string_view& field)
{
    if (!json.contains(field))
    {
        throw enea::json::MissingMandatoryFieldException(json, field);
    }

    return json.at(field);
}

template <typename T>
    requires JsonConstructible<T>
inline T convert(const nlohmann::json& json)
{
    try
    {
        return json.template get<T>();
    }
    catch ([[maybe_unudsed]] const nlohmann::json::type_error& excep)
    {
        throw enea::json::CannotConvertException<T>(json);
    }
}

} // namespace utils

#endif // UTILS_HPP
