#ifndef UTILSLAZYMOCK_HPP
#define UTILSLAZYMOCK_HPP

#include <gmock/gmock.h>

#include "utils/lazy.hpp"

class ILoadable
{
 public:
    [[nodiscard]] virtual bool isLoaded() const = 0;
    virtual bool load() = 0;
};

class Loadable : public ILoadable
{
 public:
    MOCK_METHOD(bool, isLoaded, (), (override, const));
    MOCK_METHOD(bool, load, (), (override));
};

#endif // UTILSLAZYMOCK_HPP
