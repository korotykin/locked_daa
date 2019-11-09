#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file

#include "locked_data.h"
#include <catch2/catch.hpp>

TEST_CASE("locked_data")
{
    akt::LockableData<int> a{1};
    {
        auto pa = a.Lock();
        CHECK(1 == *pa);
        *pa = 2;
    }
    {
        auto pa = a.Lock();
        CHECK(2 == *pa);
    }
}
