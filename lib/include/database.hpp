#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <string>

#include <cmrc/cmrc.hpp>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include "exception.hpp"
#include "rominfo.hpp"

// NOLINTBEGIN
namespace cmrc {
namespace detail {
struct dummy;
}
} // namespace cmrc
// NOLINTEND

CMRC_DECLARE(resources);

template <typename Key>
concept DatabaseKey = requires(nlohmann::json json) {
    { Key{json} };                                                                 // Convertible from a json
    { std::hash<Key>{}(std::declval<Key>()) } -> std::convertible_to<std::size_t>; // Hashable
    { std::declval<Key>() == std::declval<Key>() } -> std::convertible_to<bool>;   // Equality comparable
};

template <typename Value>
concept DatabaseValue = std::copyable<Value> && // Must be copyable for use in unordered_map
                        requires(nlohmann::json json) {
                            Value{json};                   // Constructible from nlohmann::json
                            typename std::optional<Value>; // Usable in an optional
                        };

template <DatabaseKey Key, DatabaseValue Value> class Database
{
 private:
    /**
     * The label for the values in the json file
     */
    static constexpr std::string_view VALUES_JSON_FIELD = "values";
    /**
     * The label for the key in the json file
     */
    static constexpr std::string_view KEY_JSON_FIELD = "key";
    /**
     * The label for the info in the json file
     */
    static constexpr std::string_view INFO_JSON_FIELD = "info";

    /**
     * @brief The path from which we will load the database. Please note that
     * this is not an std::filesystem::path because this is not expected to be
     * a filesystem path. It is a path in the internal executable filesystem that
     * cannot always be mapped to a std::filesystem::path easily
     *
     */
    std::string mPath;
    /**
     * @brief This variable stores whether or not we already
     * tried to phsysically load the database from the internal filesystem.
     * First time we try to retrieve information from the database we will
     * check if a load operation was already attempted and if not we will act accordingly
     *
     */
    bool mLoadAttempted = false;
    /**
     * @brief This variable stores whether the load operation was successful.
     * Theoretically an user may want to ignore the fact that the load operation throw
     * an exception and try to attempt a query anyway. A query should be attempted only if the
     * load operation was successful;
     *
     */
    bool mSuccesfullyLoaded = false;
    /**
     * @brief This variable effectively stores the database. For every key we have an
     * associated value that contains the information
     *
     */
    std::unordered_map<Key, Value> mRecords;

    /**
     * @brief Physically load the database from the internal filesystem. This function
     * will be called as soon as we request some information for the first time if the database
     * was not already previously loaded
     *
     */
    inline void load()
    {
        // If we already loaded successfully there is no need to do anything
        if (mSuccesfullyLoaded)
        {
            return;
        }

        auto json = loadFromFile(mPath);

        // If the db does not contain the "values" or it is not an array element we deem it invalid
        if (!json.contains(VALUES_JSON_FIELD) || !json[VALUES_JSON_FIELD].is_array())
        {
            throw Database::Exception("Invalid database (does not contain values)");
        }

        for (auto values = json[VALUES_JSON_FIELD]; const auto& value : values)
        {
            // If not enough information we deem the database invalid
            if (!value.contains(KEY_JSON_FIELD) || !value.contains(INFO_JSON_FIELD))
            {
                throw Database::Exception("Invalid database (contains invalid value)");
            }

            // If we throw when converting the value from a json we deem the database invalid
            try
            {
                mRecords.try_emplace(value[KEY_JSON_FIELD], value[INFO_JSON_FIELD]);
            }
            catch (const std::exception&)
            {
                throw Database::Exception("Invalid database (contains invalid value)");
            }
        }

        mSuccesfullyLoaded = true;
    }

    /**
     * @brief This is an helper function that helps testing this class. It hides the
     * interaction with the internal filesystem behind a mockable layer. It reads
     * the database from the disk as a string and converts it to a json. This way we could test
     * our class without relying on interaction with an internal filesystem which may be tricky
     *
     * @param path The internal filesystem path from which to load the database (note that it will
     * probably always be equal to the value of the mPath variable
     * @return nlohmann::json The json loaded from disk which represents our database
     */
    [[nodiscard]] virtual inline nlohmann::json loadFromFile(const std::string& path) const
    {
        // We are not really loading rom database from filesystem, it's really embedded into our executable's filesystem
        auto embeddedFS = cmrc::resources::get_filesystem();
        cmrc::file dbFile;
        try
        {
            dbFile = embeddedFS.open(mPath);
        }
        catch (...)
        {
            throw Database::Exception("Unable to load database");
        }

        nlohmann::json json;
        try
        {
            json = nlohmann::json::parse(
                std::string{dbFile.begin(), static_cast<std::size_t>(std::distance(dbFile.begin(), dbFile.end()))});
        }
        catch (...)
        {
            throw Database::Exception("Unable to parse database from json");
        }

        return json;
    }

 public:
    class Exception : public enea::Exception
    {
        using enea::Exception::Exception;
    };

    /**
     * @brief Construct a new Database object
     *
     * @param interalPath The path (in the internal executable filesystem) which
     * to use to physically load the rom database
     */
    inline explicit Database(const std::string& internalPath) : mPath(internalPath) {};
    Database() = delete;
    Database(const Database& romdb) = delete;
    Database(Database&& romdb) = delete;

    Database& operator=(const Database& db) = delete;
    Database& operator=(Database&& db) = delete;

    [[nodiscard]] inline std::optional<Value> find(const Key& key)
    {
        // If we never loaded the database we should do it
        if (!mLoadAttempted)
        {
            mLoadAttempted = true;
            spdlog::debug("Loading database at: {}", mPath);
            load();
        }

        // If for some reason the load operation fails we immediately return an empty value
        if (!mSuccesfullyLoaded)
        {
            spdlog::warn("Attempting to retrieve information from {} but load operation failed, returning empty info",
                         mPath);
            return std::nullopt;
        }

        // Search for the actual value
        auto result = mRecords.find(key);
        return result == mRecords.end() ? std::nullopt : std::optional<Value>(result->second);
    }
};

template <DatabaseKey Key, DatabaseValue Value> class DB
{
 public:
    DB() = delete;

    [[nodiscard]] inline static Database<Key, Value>& get(const std::string& internalPath)
    {
        static Database<Key, Value> db{internalPath};

        return db;
    }
};

#endif // DATABASE_HPP
