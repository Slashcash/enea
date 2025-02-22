#include "utils/lazy_mock.hpp"

#include <gtest/gtest.h>

/**
 * Get a lazy loadable object which has not been loaded yet.
 *
 * Expectations:
 *  - The load operation gets called once
 */
TEST(Lazy, getNotLoaded)
{
    Lazy<Loadable> loadable;

    ON_CALL(loadable.get(), isLoaded()).WillByDefault(testing::Return(false));
    EXPECT_CALL(loadable.get(), load()).Times(testing::Exactly(1));

    loadable.get();
}

/**
 * Get a lazy loadable object which has already been loaded.
 *
 * Expectations:
 *  - The load operation is never called
 */
TEST(Lazy, getAlreadyLoaded)
{
    Lazy<Loadable> loadable;

    ON_CALL(loadable.get(), isLoaded()).WillByDefault(testing::Return(true));
    EXPECT_CALL(loadable.get(), load()).Times(testing::Exactly(0));

    loadable.get();
}
