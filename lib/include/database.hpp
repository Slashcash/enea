#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <cmrc/cmrc.hpp>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include "singleton.hpp"

CMRC_DECLARE(resources);

template <typename Key>
concept DatabaseKey = requires(const Key key, nlohmann::json json) {
    { Key(json) };
    { fmt::format("{}", key) } -> std::same_as<std::string>;
    { std::hash<Key>{}(key) } -> std::convertible_to<std::size_t>;

    { key == key } -> std::convertible_to<bool>;
};

template <typename Value>
concept DatabaseValue = std::copyable<Value> && requires(const Value value, nlohmann::json json) {
    { Value(json) };
    { fmt::format("{}", value) };
    { std::optional<Value>(value) };
};

template <DatabaseKey Key, DatabaseValue Value> class Database
{
 private:
    std::string mPath;
    bool mParseAttempted = false;
    bool mSuccesfullyParsed = false;
    std::unordered_map<Key, Value> mRecords;

    [[nodiscard]] virtual inline std::optional<nlohmann::json> load() const
    {
        auto embeddedFS = cmrc::resources::get_filesystem();
        cmrc::file dbFile;
        std::string loadLog(fmt::format(R"(Load operation on database "{}".)", mPath));
        try
        {
            dbFile = embeddedFS.open(mPath);
        }
        catch (const std::system_error& excep)
        {
            spdlog::error(
                R"({} Failed because embedded file could not be opened, underlying resource library threw "{}")",
                loadLog, excep.what());

            return std::nullopt;
        }

        nlohmann::json json;
        try
        {
            json = nlohmann::json::parse(
                std::string{dbFile.begin(), static_cast<std::size_t>(std::distance(dbFile.begin(), dbFile.end()))});

            spdlog::debug("{} Operation successful", loadLog);
        }
        catch (const nlohmann::json::exception& excep)
        {
            spdlog::error(R"({} Failed because json could not be parsed, underlying json library threw "{}")", loadLog,
                          excep.what());

            return std::nullopt;
        }

        return json;
    }

    [[nodiscard]] virtual inline bool parse()
    {
        auto json = load();

        if (!json)
        {
            return false;
        }

        std::string parseLog(fmt::format(R"(Parse operation on database "{}".)", mPath));
        nlohmann::json valuesArray;
        try
        {
            valuesArray = json->at(VALUES_JSON_FIELD);
        }
        catch (const nlohmann::json::exception& excep)
        {
            spdlog::error(
                R"({} Failed because "{}" does not look like a well-formed database. Underlying json parser threw "{}")",
                parseLog, json->dump(), excep.what());

            return false;
        }

        if (!valuesArray.is_array())
        {
            spdlog::error(
                R"({} Failed because "{}" does not look like a well-formed database. "{}" field is not an array")",
                parseLog, json->dump(), VALUES_JSON_FIELD);

            return false;
        }

        for (const auto& value : valuesArray)
        {
            try
            {
                auto [insertedElem, inserted] =
                    mRecords.try_emplace(value.at(KEY_JSON_FIELD), value.at(INFO_JSON_FIELD));
                spdlog::trace(R"({} Value added to database "{}: {}")", parseLog, insertedElem->first,
                              insertedElem->second);
            }
            catch (const nlohmann::json::exception& excep)
            {
                spdlog::warn(
                    R"({} Entry "{}" could not be parsed, will not be added to the database. Underlying json parser threw "{}")",
                    parseLog, value.dump(), excep.what());
            }
        }

        spdlog::debug("{} Operation successful", parseLog);
        return true;
    }

 public:
    // these constexpr are public just to ease testing
    static constexpr std::string_view VALUES_JSON_FIELD = "values";
    static constexpr std::string_view KEY_JSON_FIELD = "key";
    static constexpr std::string_view INFO_JSON_FIELD = "info";

    inline explicit Database(const std::string& path) : mPath(path) {}
    Database() = delete;
    Database(const Database& romdb) = delete;
    Database(Database&& romdb) = delete;

    Database& operator=(const Database& db) = delete;
    Database& operator=(Database&& db) = delete;

    [[nodiscard]] inline std::optional<Value> find(const Key& key)
    {
        if (!mParseAttempted)
        {
            mSuccesfullyParsed = parse();
            mParseAttempted = true;
        }

        std::string queryLog(fmt::format(R"(Query operation on database "{}" for key "{}".)", mPath, key));
        if (!mSuccesfullyParsed)
        {
            spdlog::warn("{} Query result will be empty since database parse operation was unsuccesful", queryLog);
            return std::nullopt;
        }

        // Search for the actual value
        auto result = mRecords.find(key);
        if (result == mRecords.end())
        {
            spdlog::debug("{} Query result will be empty since there is no match in database");
            return std::nullopt;
        }

        spdlog::debug(R"({} Found match "{}")", queryLog, result->second);
        return std::optional<Value>(result->second);
    };
};

template <typename Key, typename Value, const char* dbPath> using DB = Singleton<Database<Key, Value>, dbPath>;

#endif // DATABASE_HPP
