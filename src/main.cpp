#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file

#include "locked_data.h"
#include <catch2/catch.hpp>

#include <vector>
#include <thread>

TEST_CASE("locked_data")
{
    {
        akt::LockableData<int> a{1};
        auto pa = a.Lock();
        CHECK(1 == pa.mValue);
    }
    {
        akt::LockableData<int> a{1};
        {
            auto pa = a.Lock();
            pa.mValue = 2;
        }
        {
            auto pa = a.Lock();
            CHECK(2 == pa.mValue);
        }
    }
}

TEST_CASE("locked_data with default ctor")
{
    {
        akt::LockableData<int> a;
        {
            auto pa = a.Lock();
            pa.mValue = 1;
        }
        CHECK(1 == a.Lock().mValue);
    }
}

TEST_CASE("non locked_data races")
{
    { // data races
        int i = 0;
        std::vector<std::thread> thrds;
        for ( int k =0; k < 5; ++k) {
            thrds.emplace_back([&i](const int val, const int num){ for(int j = 0; j < num; ++j) i = i+val; }, 10000, 50000);
            thrds.emplace_back([&i](const int val, const int num){ for(int j = 0; j < num; ++j) i = i-val; }, 10000, 50000);
        }
        for (auto& thethr: thrds) {
            thethr.join();
        }
        CHECK_FALSE(i == 0);
    }
}

TEST_CASE("locked_data - no races")
{
    { // fixed data races
        akt::LockableData<int> i{0};
        std::vector<std::thread> thrds;
        for ( int k =0; k < 5; ++k) {
            thrds.emplace_back([&i](const int val, const int num){ for(int j = 0; j < num; ++j) { auto pi = i.Lock(); pi.mValue = pi.mValue+val; } }, 10000, 50000);
            thrds.emplace_back([&i](const int val, const int num){ for(int j = 0; j < num; ++j) {auto pi = i.Lock(); pi.mValue = pi.mValue-val;} }, 10000, 50000);
        }
        for (auto& thethr: thrds) {
            thethr.join();
        }
        CHECK(i.Lock().mValue == 0);
    }
}


TEST_CASE("locked_data - try lock - no races")
{
    { // fixed data races
        akt::LockableData<int> i{0};
        std::vector<std::thread> thrds;
        for ( int k =0; k < 5; ++k) {
            thrds.emplace_back([&i](const int val, const int num)
            { for(int j = 0; j < num;) { auto pi = i.TryLock(); if(pi) { pi->mValue = pi->mValue+val; ++j; } } }, 10000, 50000);
            thrds.emplace_back([&i](const int val, const int num)
            { for(int j = 0; j < num;) {auto pi = i.TryLock(); if(pi) {pi->mValue = pi->mValue-val; ++j; } } }, 10000, 50000);
        }
        for (auto& thethr: thrds) {
            thethr.join();
        }
        CHECK(i.Lock().mValue == 0);
    }
}
