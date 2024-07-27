#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP

#include <filesystem>
#include <optional>

#include "exception.hpp"

/**
 * @brief This class manages the software configuration. Since the configuration
 * is meant to be unique it is then wrapped into a singleton
 */
class Conf
{
 private:
    [[nodiscard]] virtual std::optional<std::filesystem::path> homeDirectory() const;
    [[nodiscard]] std::filesystem::path baseDirectory() const;

 public:
    class Excep : public Exception
    {
        using Exception::Exception;
    };

    Conf() = default;
    Conf(const Conf& conf) = delete;
    Conf(Conf&& conf) = delete;

    [[nodiscard]] std::filesystem::path romDirectory() const;
    [[nodiscard]] std::filesystem::path cacheFile() const;

    Conf& operator=(const Conf& conf) = delete;
    Conf& operator=(Conf&& conf) = delete;
    bool operator==(const Conf& conf) = delete;

    virtual ~Conf() = default;
};

class Configuration
{
 public:
    Configuration() = delete;

    [[nodiscard]] inline static Conf& get()
    {
        static Conf conf;
        return conf;
    }
};

#endif // CONFIGURATION_HPP
