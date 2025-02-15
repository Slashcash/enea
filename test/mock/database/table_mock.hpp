#ifndef DATABASETABLEMOCK_HPP
#define DATABASETABLEMOCK_HPP

#include "database/table.hpp"

#include <gmock/gmock.h>

namespace Database {

using namespace ChefFun;
inline constexpr char fileName[] = "test.json";
template <Key K, Value V> class VTableMock : public VTable<K, V, fileName>
{
 public:
    using VTable<K, V, fileName>::VTable;
    MOCK_METHOD((Either<Error, std::string>), readFromFile, (), (override, const));
};

} // namespace Database
#endif // DATABASETABLEBMOCK_HPP
