#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP

#include <filesystem>
#include <optional>

#include "exception.hpp"

/**
 * @brief This class manages the software configuration.
 *
 */
class Configuration
{
 private:
    [[nodiscard]] virtual std::optional<std::filesystem::path> homeDirectory() const;
    [[nodiscard]] std::filesystem::path baseDirectory() const;

 public:
    class Excep : public Exception
    {
        using Exception::Exception;
    };

    Configuration() = default;
    Configuration(const Configuration& conf) = delete;
    Configuration(Configuration&& conf) = delete;

    [[nodiscard]] std::filesystem::path romDirectory() const;
    [[nodiscard]] std::filesystem::path cacheFile() const;

    Configuration& operator=(const Configuration& conf) = delete;
    Configuration& operator=(Configuration&& conf) = delete;
    bool operator==(const Configuration& conf) = delete;

    virtual ~Configuration() = default;
};

#endif // CONFIGURATION_HPP
