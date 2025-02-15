#ifndef DATABASETABLE_HPP
#define DATABASETABLE_HPP

#include <string>

#include <ChefFun/Either.hh>
#include <ChefFun/Error.hh>
#include <cmrc/cmrc.hpp>
#include <magic_enum.hpp>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include "exception.hpp"
#include "singleton.hpp"
#include "utils/lazy.hpp"

CMRC_DECLARE(resources);

namespace Database {

/**
 * The database key should be:
 * - Default constructible
 * - Copy constructible
 * - Copy assignable
 * - Destructible
 * - Hashable since it will be used as a key for an std::unordered_map
 * - Equally comparable as it will be used in a std::unordered_map
 * - Constructible fom a json since it will be read from a json file
 * - Formattable since its value will be logged
 */
template <typename T>
concept Key = requires {
    requires std::is_default_constructible_v<T>;
    requires std::is_copy_constructible_v<T>;
    requires std::is_copy_assignable_v<T>;
    requires std::is_destructible_v<T>;

    requires requires(T t) {
        { std::hash<T>{}(t) } -> std::same_as<std::size_t>;
    };

    requires requires(T a, T b) {
        { a == b } -> std::convertible_to<bool>;
    };

    requires requires(nlohmann::json json) { T{json}; };

    requires requires(T t) { fmt::format("{}", t); };
};

/**
 * The database value should be:
 * - Copy constructible
 * - Copy assignable
 * - Destructible
 * - Able to be used as an std::optional template parameter
 * - Constructible fom a json since it will be read from a json file
 * - Formattable since its value will be logged
 */
template <typename T>
concept Value = requires {
    requires std::is_copy_constructible_v<T>;
    requires std::is_copy_assignable_v<T>;
    requires std::is_destructible_v<T>;

    requires requires { typename std::optional<T>; };

    requires requires(nlohmann::json json) { T{json}; };

    requires requires(T t) { fmt::format("{}", t); };
};

enum class Result
{
    SUCCESS = 0,
    NOT_LOADED,
    ALREADY_LOADED,
    OPEN_DATABASE_FILE,
    PARSE_JSON,
};

class Exception : public enea::Exception
{
    using enea::Exception::Exception;
};

/**
 * This class abstracts the concept of a database table.
 * It will provide an association between an unique key and a value.
 * Database and table are to be intended in the context of this software
 * and not as typical SQL databases.
 *
 * A table is physically represented as a json file embedded into our
 * executable filesystem that gets loaded at runtime.
 */
using Error = ChefFun::Error<Result>;
template <Key K, Value V, const char* fileName> class VTable
{
 public:
    using ReadResult = ChefFun::Either<Database::Error, std::string>;
    using QueryResult = ChefFun::Either<Database::Error, std::optional<V>>;

 private:
    /*
        This optional variable stores the load operation result.
        It will be empty if the load operation never occurred.
    */
    std::optional<Error> mLoadResult;

    /**
     * This unordered map contains the association between
     * database keys and their actual values.
     */
    std::unordered_map<K, V> mRecords;

    /**
     * This function phisycally reads the json from our executable's embedded filesystem.
     * It either provides the json in a raw string format or an error. It is provided as a separete
     * virtual function so it can be easily gmocked.
     */
    [[nodiscard]] virtual inline ReadResult readFromFile() const
    {
        cmrc::file dbFile;
        try
        {
            dbFile = cmrc::resources::get_filesystem().open(fileName);
        }
        catch (const std::system_error& excep)
        {
            spdlog::error("Load operation on {}. Reading file failed. Underlying library threw: {}", fileName,
                          excep.what());
            return readFailed(Result::OPEN_DATABASE_FILE);
        }

        auto databaseSize = static_cast<std::size_t>(std::distance(dbFile.begin(), dbFile.end()));
        std::string databaseString(dbFile.begin(), databaseSize);
        return readSuccess(databaseString);
    }

    /**
     * This is an helper function that eases the load process. It is just meant to provide
     * some code readability enhancements.
     */
    [[nodiscard]] inline Error loadEffective()
    {
        auto logLine = fmt::format("Load operation on {}.", fileName);

        if (isLoaded())
        {
            spdlog::warn("{} Skipping operation as table was already loaded", logLine);

            if (!mLoadResult.has_value())
            {
                throw Exception(fmt::format("{} Unexpectedly empty table load result", logLine));
            }

            return *mLoadResult;
        }

        // Reading physical file
        auto readResult = readFromFile();
        if (readResult.isLeft())
        {
            return readResult.getLeft();
        }

        // Transforming the string into a json
        nlohmann::json parsed;
        try
        {
            parsed = nlohmann::json::parse(readResult.getRight());
        }
        catch (const nlohmann::json::exception& excep)
        {
            spdlog::error("{} Parsing json failed. Underlying library threw: {}", logLine, excep.what());
            return Error(Result::PARSE_JSON);
        }

        // Remapping the json into our map
        nlohmann::json valuesArray;
        try
        {
            valuesArray = parsed.at(VALUES_JSON_FIELD);
        }
        catch (const nlohmann::json::exception& excep)
        {
            spdlog::error("{} Failed because database misses {} field. Underlying library threw: {}", logLine,
                          VALUES_JSON_FIELD, excep.what());
            return Error(Result::PARSE_JSON);
        }

        if (!valuesArray.is_array())
        {
            spdlog::error("{} Failed because {} field is not an array", logLine, VALUES_JSON_FIELD);
            return Error(Result::PARSE_JSON);
        }

        for (const auto& value : valuesArray)
        {
            try
            {
                auto [insertedElem, inserted] =
                    mRecords.try_emplace(value.at(KEY_JSON_FIELD), value.at(VALUE_JSON_FIELD));
                if (inserted)
                {
                    spdlog::trace("{} Value with key {} added to database: {}", logLine, insertedElem->first,
                                  insertedElem->second);
                }
                else
                {
                    spdlog::warn("{} Double insertion for key {}, already available with value {}", logLine,
                                 insertedElem->first, insertedElem->second);
                }
            }
            catch (const nlohmann::json::exception& excep)
            {
                spdlog::warn(R"({} Entry "{}" could not be parsed, will not be added. Underlying library threw: {})",
                             logLine, value.dump(), excep.what());
            }
        }

        spdlog::debug("{} Succesfully parsed {} records", logLine, mRecords.size());
        return Error(Result::SUCCESS);
    }

 public:
    static constexpr std::string_view VALUES_JSON_FIELD = "values";
    static constexpr std::string_view KEY_JSON_FIELD = "key";
    static constexpr std::string_view VALUE_JSON_FIELD = "info";

    VTable() = default;
    VTable(const VTable&) = delete;
    VTable(VTable&&) = delete;

    /**
     * These are just helper function that are meant to enhance code readability.
     * They are public because they are useful for testing purposes
     */
    [[nodiscard]] inline static ReadResult readSuccess(const std::string& fileContent)
    {
        return ReadResult::Right(fileContent);
    }

    [[nodiscard]] inline static ReadResult readFailed(const Result& result)
    {
        return ReadResult::Left(Database::Error(result));
    }

    [[nodiscard]] inline static QueryResult querySuccess(const std::optional<V>& result)
    {
        return QueryResult::Right(result);
    }

    [[nodiscard]] inline static QueryResult querySuccess(const V& result)
    {
        return QueryResult::Right(result);
    }

    [[nodiscard]] inline static QueryResult queryFailed(const Result& result)
    {
        return QueryResult::Left(Database::Error(result));
    }

    /**
     * This function loads the table from its file embedded into executable's
     * filesystem. This function should be called before attempting any kind of query
     * into the table.
     */
    [[nodiscard]] inline Error load()
    {
        return *(mLoadResult = loadEffective());
    }

    /**
     * This function queries the database for the specified key.
     * It returns either the associated value if present or an error.
     */
    [[nodiscard]] inline QueryResult find(const K& key) const
    {
        auto logLine = fmt::format("Find operation on {} for key {}.", fileName, key);

        if (!mLoadResult.has_value())
        {
            spdlog::error("{} Database was not loaded", logLine);
            return queryFailed(Result::NOT_LOADED);
        }

        if (mLoadResult->isError())
        {
            spdlog::error("{} Database was not loaded successfully ({})", logLine,
                          magic_enum::enum_name(mLoadResult->getCode()));

            return queryFailed(mLoadResult->getCode());
        }

        auto searchResult = mRecords.find(key);
        if (searchResult == mRecords.end())
        {
            spdlog::debug("{} No match found", logLine);
            return querySuccess(std::optional<V>());
        }

        spdlog::debug("{} Found match: {}", logLine, searchResult->second);
        return querySuccess(searchResult->second);
    };
    /**
     * This function returns true if an attempt to load the table from
     * file has already been made
     */
    [[nodiscard]] inline bool isLoaded() const
    {
        return mLoadResult.has_value();
    }

    VTable& operator=(const VTable&) = delete;
    VTable& operator=(VTable&&) = delete;
    bool operator==(const VTable&) = delete;
};

template <Key K, Value V, const char* fileName> using Table = LazySingleton<VTable<K, V, fileName>>;

} // namespace Database

#endif // DATABASETABLE_HPP
