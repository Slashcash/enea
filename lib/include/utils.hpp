#ifndef UTILS_HPP
#define UTILS_HPP

#include <nlohmann/json.hpp>

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
inline std::optional<T> getValueFromJson(const nlohmann::json& json, const std::string_view& field)
{
    try
    {
        return json.at(field).get<T>();
    }
    catch ([[maybe_unused]] const nlohmann::json::exception& e)
    {
        return std::nullopt;
    }
}
} // namespace utils

#endif // UTILS_HPP
