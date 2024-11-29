#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP

#include <filesystem>
#include <optional>

#include "exception.hpp"
#include "singleton.hpp"

class Conf
{
 private:
    [[nodiscard]] virtual std::optional<std::filesystem::path> homeDirectory() const;
    [[nodiscard]] virtual std::optional<std::filesystem::path> executableDirectory() const;
    [[nodiscard]] std::filesystem::path baseDirectory() const;

 public:
    class Exception : public enea::Exception
    {
        using enea::Exception::Exception;
    };

    Conf() = default;
    Conf(const Conf& conf) = delete;
    Conf(Conf&& conf) = delete;

    [[nodiscard]] std::filesystem::path romDirectory() const;
    [[nodiscard]] std::filesystem::path bundledRomDirectory() const;
    [[nodiscard]] std::filesystem::path cacheDirectory() const;
    [[nodiscard]] std::filesystem::path advMameConfigurationFile() const;

    Conf& operator=(const Conf& conf) = delete;
    Conf& operator=(Conf&& conf) = delete;
    bool operator==(const Conf& conf) = delete;

    virtual ~Conf() = default;
};

using Configuration = Singleton<Conf>;

#endif // CONFIGURATION_HPP
