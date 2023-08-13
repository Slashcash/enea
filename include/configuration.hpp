#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP

#include <filesystem>
#include <optional>
#include <stdexcept>

class Configuration
{
 public:
    class Exception : public std::runtime_error
    {
        using std::runtime_error::runtime_error;
    };

    [[nodiscard]] std::filesystem::path romDirectory() const;

    [[nodiscard]] inline static Configuration& get()
    {
        static Configuration conf;
        return conf;
    }

 protected:
    Configuration() = default;

    ~Configuration() = default;

 private:
    Configuration(Configuration& configuration) = delete;

    void operator=(const Configuration&) = delete;

    [[nodiscard]] std::filesystem::path baseDirectory() const;
    [[nodiscard]] virtual std::optional<std::filesystem::path> homeDirectory() const;
    [[nodiscard]] virtual std::optional<std::filesystem::path> tempHomeDirectory() const;
};

#endif // CONFIGURATION_HPP