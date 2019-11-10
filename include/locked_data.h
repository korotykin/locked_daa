#pragma once

#include <memory>
#include <mutex>
#include <type_traits>

namespace akt
{
// predeclaration
template <typename T>
class LockedData;


template <typename T>
class LockableData
{
public:
    LockedData<T> Lock() { return LockedData<T>(mMtx, mValue); }
    std::unique_ptr<LockedData<T>> TryLock()
    { std::unique_lock<std::mutex> guard{mMtx, std::defer_lock}; if(!guard.try_lock()) return nullptr; return std::unique_ptr<LockedData<T>>(new LockedData<T>(std::move(guard), mValue)); }
public:
    template <typename... Args, typename std::enable_if<std::is_constructible<T, Args...>::value, int>::type = 0>
    LockableData(Args&&... args): mValue(std::forward<Args>(args)...) {}
/*
    template <typename std::enable_if<std::is_default_constructible<T>::value && std::is_convertable<U, T>::value, int>::type = 0>
    LockableData() {}
*/
private:
    T mValue;
    std::mutex mMtx;
};

template <typename T>
class LockedData
{
private:
friend LockableData<T>;
    LockedData(std::unique_lock<std::mutex>&& guard, T& value): mGuard(std::move(guard)), mValue(value) {}
public:
    LockedData(std::mutex& mtx, T& value): mGuard(mtx), mValue(value) {}
private:
    std::unique_lock<std::mutex> mGuard;
public:
    T& mValue;

public:
    LockedData() = delete;
    LockedData(const LockedData<T>&) = delete;
    LockedData(LockedData<T>&& rhv): mGuard(std::move(rhv.mGuard)), mValue(rhv.mValue) {}
    LockedData& operator=(const LockedData<T>&) = delete;
    LockedData& operator=(LockedData<T>&& rhv) { mGuard = std::move(rhv.mGuard); mValue = rhv.mValue; }
};

} // namespace akt
