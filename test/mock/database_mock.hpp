#ifndef DATABASEMOCK_HPP
#define DATABASEMOCK_HPP

#include "database.hpp"

#include <gmock/gmock.h>

class DatabaseMock : public Database<std::string, std::string>
{
 public:
    using Database::Database;
    MOCK_METHOD(std::optional<nlohmann::json>, load, (), (override, const));
};

#endif // DATABASEBMOCK_HPP
