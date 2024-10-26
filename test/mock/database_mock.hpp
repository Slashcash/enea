#ifndef DATABASEMOCK_HPP
#define DATABASEMOCK_HPP

#include "database.hpp"

#include <gmock/gmock.h>

class DatabaseMock : public Database<std::string, std::string>
{
 public:
    using Database::Database;
    MOCK_METHOD(nlohmann::json, loadFromFile, (const std::string&), (override, const));
};

#endif // DATABASEBMOCK_HPP
