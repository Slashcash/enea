#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <ChefFun/Either.hh>
#include <cmrc/cmrc.hpp>
#include <magic_enum.hpp>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include "exception.hpp"
#include "singleton.hpp"

CMRC_DECLARE(resources);

template <typename T>
concept DatabaseKey = requires {
    requires std::is_default_constructible_v<T>;
    requires std::is_copy_constructible_v<T>;
    requires std::is_copy_assignable_v<T>;
    requires std::is_destructible_v<T>;

    // The database key should be hashable as it will be used in a std::unordered_map
    requires requires(T t) {
        { std::hash<T>{}(t) } -> std::same_as<std::size_t>;
    };

    // The database key should be equally comparable as it will be used in a std::unordered_map
    requires requires(T a, T b) {
        { a == b } -> std::convertible_to<bool>;
    };

    // The database key should be constructible from a json
    requires requires(nlohmann::json json) { T{json}; };

    // The database key should be formattable
    requires requires(T t) { fmt::format("{}", t); };
};

template <typename T>
concept DatabaseValue = requires {
    requires std::is_copy_constructible_v<T>;
    requires std::is_copy_assignable_v<T>;
    requires std::is_destructible_v<T>;

    // The database value should be able to be put in an std::optional
    requires requires { typename std::optional<T>; };

    // The database value should be constructible from a json
    requires requires(nlohmann::json json) { T{json}; };

    // The database value should be formattable
    requires requires(T t) { fmt::format("{}", t); };
};

template <DatabaseKey Key, DatabaseValue Value> class Database
{
 public:
    enum class Error
    {
        SUCCESS = 0,
        OPEN_DATABASE_FILE,
        PARSE_JSON,
        VALUES_MISSING,
        VALUES_NOT_ARRAY,
    };

    class Exception : public enea::Exception
    {
        using enea::Exception::Exception;
    };

 private:
    using DatabaseRecords = std::unordered_map<Key, Value>;

    std::string mName;
    mutable DatabaseRecords mRecords;
    mutable std::once_flag mLoaded;

    const std::string mLoadOperationLog;
    const std::string mQueryOperationLog;

    // These functions could actually be static, it's just that they are easily gmocked by being virtual
    [[nodiscard]] virtual ChefFun::Either<Error, std::string> read(const std::string& name) const;
    [[nodiscard]] ChefFun::Either<Error, nlohmann::json> parseJson(const std::string& jsonString) const;
    [[nodiscard]] ChefFun::Either<Error, DatabaseRecords> parseRecords(const nlohmann::json& json) const;
    [[nodiscard]] ChefFun::Either<Error, DatabaseRecords> load(const std::string& name) const;
    void loadIfNecessary() const;

 public:
    static constexpr std::string_view VALUES_JSON_FIELD = "values";
    static constexpr std::string_view KEY_JSON_FIELD = "key";
    static constexpr std::string_view INFO_JSON_FIELD = "info";

    Database() = delete;
    Database(const Database& romdb) = delete;
    Database(Database&& romdb) = delete;

    Database& operator=(const Database&) = delete;
    Database& operator=(Database&&) = delete;
    bool operator==(const Database&) const = delete;

    inline explicit Database(const std::string& name)
        : mName(name), mLoadOperationLog(fmt::format(R"(Load operation on database "{}".)", name)),
          mQueryOperationLog(fmt::format(R"(Query operation on database "{}".)", name))
    {}

    [[nodiscard]] std::optional<Value> find(const Key& key) const;
    [[nodiscard]] std::size_t entries() const;
};

template <DatabaseKey Key, DatabaseValue Value> std::size_t Database<Key, Value>::entries() const
{
    loadIfNecessary();
    return mRecords.size();
}

template <DatabaseKey Key, DatabaseValue Value> std::optional<Value> Database<Key, Value>::find(const Key& key) const
{
    loadIfNecessary();

    auto result = mRecords.find(key);
    if (result == mRecords.end())
    {
        spdlog::trace(R"({} Key: "{}" has no match)", mQueryOperationLog, key);
        return std::nullopt;
    }

    spdlog::trace(R"({} Key: "{}" has a match: "{}")", mQueryOperationLog, key, result->second);
    return result->second;
}

template <DatabaseKey Key, DatabaseValue Value>
ChefFun::Either<typename Database<Key, Value>::Error, std::string>
Database<Key, Value>::read(const std::string& name) const
{
    cmrc::file dbFile;
    try
    {
        dbFile = cmrc::resources::get_filesystem().open(name);
    }
    catch (const std::system_error& excep)
    {
        spdlog::error(
            R"({} Reading from file failed because embedded file could not be opened, underlying resource library threw "{}")",
            mLoadOperationLog, excep.what());

        return ChefFun::Either<Error, std::string>::Left(Error::OPEN_DATABASE_FILE);
    }

    auto databaseSize = static_cast<std::size_t>(std::distance(dbFile.begin(), dbFile.end()));
    std::string databaseString(dbFile.begin(), databaseSize);
    spdlog::debug("{} Successfully read {} bytes from file", mLoadOperationLog, databaseSize, name);
    spdlog::trace(R"({} Read database: "{}")", mLoadOperationLog, databaseString);
    return ChefFun::Either<Error, std::string>::Right(databaseString);
}

template <DatabaseKey Key, DatabaseValue Value>
ChefFun::Either<typename Database<Key, Value>::Error, nlohmann::json>
Database<Key, Value>::parseJson(const std::string& json) const
{
    nlohmann::json parsed;
    try
    {
        parsed = nlohmann::json::parse(json);
    }
    catch (const nlohmann::json::exception& excep)
    {
        spdlog::error(R"({} Failed because json could not be parsed, underlying json library threw "{}")",
                      mLoadOperationLog, excep.what());

        return ChefFun::Either<Error, nlohmann::json>::Left(Error::PARSE_JSON);
    }

    spdlog::trace(R"({} Parsed database json: "{}")", mLoadOperationLog, parsed.dump());
    return ChefFun::Either<Error, nlohmann::json>::Right(parsed);
}

template <DatabaseKey Key, DatabaseValue Value>
ChefFun::Either<typename Database<Key, Value>::Error, typename Database<Key, Value>::DatabaseRecords>
Database<Key, Value>::parseRecords(const nlohmann::json& json) const
{
    DatabaseRecords result;

    // Parsing the database values
    nlohmann::json valuesArray;
    try
    {
        valuesArray = json.at(VALUES_JSON_FIELD);
    }
    catch (const nlohmann::json::exception& excep)
    {
        spdlog::error(
            R"({} Failed because database does not look like it's well-formed: missing "{}" field. Underlying json parser threw "{}")",
            mLoadOperationLog, VALUES_JSON_FIELD, excep.what());

        return ChefFun::Either<Error, DatabaseRecords>::Left(Error::VALUES_MISSING);
    }

    // Checking if the database values are contained in an array
    if (!valuesArray.is_array())
    {
        spdlog::error(R"({} Failed because database does not look like it's well formed: "{}" field is not an array")",
                      mLoadOperationLog, VALUES_JSON_FIELD);

        return ChefFun::Either<Error, DatabaseRecords>::Left(Error::VALUES_NOT_ARRAY);
    }

    // Filling the database entries
    for (const auto& value : valuesArray)
    {
        try
        {
            auto [insertedElem, inserted] = result.try_emplace(value.at(KEY_JSON_FIELD), value.at(INFO_JSON_FIELD));
            spdlog::trace(R"({} Value added to database "{}: {}")", mLoadOperationLog, insertedElem->first,
                          insertedElem->second);
        }
        catch (const nlohmann::json::exception& excep)
        {
            spdlog::warn(
                R"({} Entry "{}" could not be parsed, will not be added to the database. Underlying json parser threw "{}")",
                mLoadOperationLog, value.dump(), excep.what());
        }
    }

    spdlog::debug("{} Successfully parsed {} database entries", mLoadOperationLog, result.size());
    return ChefFun::Either<Error, DatabaseRecords>::Right(result);
}

template <DatabaseKey Key, DatabaseValue Value>
ChefFun::Either<typename Database<Key, Value>::Error, typename Database<Key, Value>::DatabaseRecords>
Database<Key, Value>::load(const std::string& name) const
{
    // Loading the database from file
    std::string file;
    if (auto readResult = read(name); readResult.isRight())
    {
        file = readResult.getRight();
    }
    else
    {
        return ChefFun::Either<Error, DatabaseRecords>::Left(readResult.getLeft());
    }

    // Parsing the json from the read file
    nlohmann::json json;
    if (auto parseResult = parseJson(file); parseResult.isRight())
    {
        json = parseResult.getRight();
    }
    else
    {
        return ChefFun::Either<Error, DatabaseRecords>::Left(parseResult.getLeft());
    }

    // Constructing the records from the json
    return parseRecords(json);
}

template <DatabaseKey Key, DatabaseValue Value> void Database<Key, Value>::loadIfNecessary() const
{
    std::call_once(mLoaded, [this]() {
        if (auto loadResult = load(mName); loadResult.isRight())
        {
            mRecords = loadResult.getRight();
        }
        else
        {
            throw Exception(fmt::format(R"(Failed to load database "{}", error: "{}")", mName,
                                        magic_enum::enum_name(loadResult.getLeft())));
        }
    });
}

template <typename Key, typename Value, const char* dbPath>
using DatabaseSingleton = Singleton<Database<Key, Value>, dbPath>;

#endif // DATABASE_HPP
