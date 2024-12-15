#ifndef DATABASEMOCK_HPP
#define DATABASEMOCK_HPP

#include "database.hpp"

#include <gmock/gmock.h>

class DatabaseMock : public Database<std::string, std::string>
{
 public:
    using Database::Database;
    MOCK_METHOD((ChefFun::Either<Error, std::string>), read, (const std::string& name), (override, const));
};

#endif // DATABASEBMOCK_HPP
