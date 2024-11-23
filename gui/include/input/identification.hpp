#ifndef INPUTIDENTIFICATION_HPP
#define INPUTIDENTIFICATION_HPP

#include <string>
#include <string_view>

#include <nlohmann/json.hpp>

#include "utils.hpp"

namespace Input {
/**
 * @brief This enum represents all the different types of input devices that this
 * software supports.
 * NOTE: Do not change numerical values of this enum, it is serialized and deserialized
 * from json. By changing numerical values parsing will break.
 */
enum class Type
{
    Joystick = 0,
    Keyboard = 1
};

/**
 * @brief This struct represents an identification mechanism for an input method.
 * Every different input method (eg: every different kind of joystick, joypad or keyboard)
 * is assigned with a (supposedly) unique name, vendor id and product id. We use this triplet
 * to identify an input method. This triplet is returned by SFML (and originally retrieved from
 * the underlying OS driver) and for this reason this struct pretty much mimics
 * the sf::Joystick::Identification.
 */
struct Identification
{
    /**
     * @brief When converting this struct to a json the "type" member variable
     * will be added to the json using this as a field name.
     */
    static constexpr std::string_view TYPE_JSON_FIELD = "type";
    /**
     * @brief When converting this struct to a json the "name" member variable
     * will be added to the json using this as a field name.
     */
    static constexpr std::string_view NAME_JSON_FIELD = "name";
    /**
     * @brief When converting this struct to a json the "vendorId" member variable
     * will be added to the json using this as a field name.
     */
    static constexpr std::string_view VENDORID_JSON_FIELD = "vendorId";
    /**
     * @brief When converting this struct to a json the "productId" member variable
     * will be added to the json using this as a field name.
     */
    static constexpr std::string_view PRODUCTID_JSON_FIELD = "productId";

    /**
     * @brief The type of the input method (eg: if the input method is a
     * keyboard, a joypad, a joystick and so on)
     *
     */
    Input::Type type = Input::Type::Keyboard;
    /**
     * @brief The name of the input method. It is generally retrieved
     * from the OS driver by SFML and is filled with the
     * commercial name of the input method (eg: "PS4 Controller" for the
     * Playstation 4 controller).
     */
    std::string name;
    /**
     * @brief The vendor id of the input method. It is generally retrieved
     * from the OS driver by SFML and is filled with the vendor id
     * of the input method. Every vendor is assigned with an unique id that
     * can be used to identify it.
     */
    unsigned int vendorId = 0;
    /**
     * @brief The product id of the input method. It is generally retrieved
     * from the OS driver by SFML and is filled with the vendor id
     * of the input method. Every vendor is assigned with an unique id that
     * can be used to identify it.
     */
    unsigned int productId = 0;

    [[nodiscard]] inline std::string toString() const
    {
        return fmt::format("{}: {}. Vendor ID: {}, Product ID: {}", magic_enum::enum_name(type), name, vendorId,
                           productId);
    }

    [[nodiscard]] bool operator==(const Identification& id) const
    {
        return id.productId == this->productId && id.vendorId == this->vendorId;
    };
};

/**
 * @brief nlohmann::json helper function that makes a json
 * convertible to an Input::Identification.
 *
 * @param json the input json.
 * @param identification the Input::Identification output.
 */
inline void from_json(const nlohmann::json& json, Input::Identification& identification)
{
    identification.type = utils::getMandatoryValueFromJson<Input::Type>(json, Input::Identification::TYPE_JSON_FIELD);
    identification.name = utils::getMandatoryValueFromJson<std::string>(json, Input::Identification::NAME_JSON_FIELD);
    identification.vendorId =
        utils::getMandatoryValueFromJson<unsigned int>(json, Input::Identification::VENDORID_JSON_FIELD);
    identification.productId =
        utils::getMandatoryValueFromJson<unsigned int>(json, Input::Identification::PRODUCTID_JSON_FIELD);
}

/**
 * @brief nlohmann::json helper function that makes a
 * Input::Identification convertible to an Input::Identification.
 *
 * @param json The converted json.
 * @param identification The input Input::Identification.
 */
inline void to_json(nlohmann::json& json, const Input::Identification& identification)
{
    json.clear();

    json[Input::Identification::TYPE_JSON_FIELD] = identification.type;
    json[Input::Identification::NAME_JSON_FIELD] = identification.name;
    json[Input::Identification::VENDORID_JSON_FIELD] = identification.vendorId;
    json[Input::Identification::PRODUCTID_JSON_FIELD] = identification.productId;
}

} // namespace Input

namespace std {
template <> struct hash<Input::Identification>
{
    /**
     * @brief Custom hash function for Input::Identification.
     * This is needed because Input::Identification is needed to fit
     * into an std::unordered_map.
     *
     * @param identification the Input::Identification on which we want to calculate the hash on.
     * @return The calculated hash.
     */
    std::size_t operator()(const Input::Identification& identification) const noexcept
    {
        return std::hash<std::string>{}(identification.toString());
    }
};
} // namespace std

template <> struct fmt::formatter<Input::Identification> : fmt::formatter<string_view>
{
    /**
     * @brief fmt helper function that makes an Input::Identification
     * formattable with fmt.
     *
     * @param id The input Input::Identification.
     * @param ctx The format context.
     * @return The formatted Input::Identification
     */
    auto format(const Input::Identification& identification, fmt::format_context& ctx) const
        -> fmt::format_context::iterator
    {
        return fmt::formatter<string_view>::format(identification.toString(), ctx);
    }
};

#endif // INPUTIDENTIFICATION_HPP
