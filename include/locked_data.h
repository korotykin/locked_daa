#pragma once

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
public:
    template <typename... Args, typename std::enable_if<std::is_constructible<T, Args...>::value, int>::type = 0>
    LockableData(Args&&... args): mValue(std::forward<Args>(args)...) {}
/*
    template <typename std::enable_if<std::is_default_constructible<T>::value && std::is_convertable<U, T>::value, int>::type = 0>
    LockableData() {}
    template <typename U = T, typename std::enable_if<std::is_copy_constructible<T>::value && std::is_convertable<U, T>::value, int>::type = 0>
    LockableData(const U& rhv): mValue(rhv) {}
    template <typename std::enable_if<std::is_move_constructible<T>::value, int>::type = 0>
    LockableData(T&& rhv): mValue(std::forward<T>(rhv)) {}
*/
private:
    T mValue;
    std::mutex mMtx;
};

template <typename T>
class LockedData
{
public:
    LockedData(std::mutex& mtx, T& value): mGuard(mtx), mValue(value) {}

public:
    T& operator*() { return mValue; }
    const T& operator*() const { return mValue; }
    T* operator->() { return &mValue; }
    const T* operator->() const { return &mValue; }
public:
    LockedData() = delete;
    LockedData(const LockedData<T>&) = delete;
    LockedData(LockedData<T>&& rhv): mGuard(std::move(rhv.mGuard)), mValue(rhv.mValue) {}
    LockedData& operator=(const LockedData<T>&) = delete;
    LockedData& operator=(LockedData<T>&& rhv) { mGuard = std::move(rhv.mGuard); mValue = rhv.mValue; }
private:
    std::unique_lock<std::mutex> mGuard;
    T& mValue;
};

} // namespace akt
