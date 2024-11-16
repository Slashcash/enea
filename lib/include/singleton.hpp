#ifndef SINGLETON_HPP
#define SINGLETON_HPP

#include <memory>
#include <mutex>
#include <utility>

template <typename T, auto... constructorArgs> class Singleton
{
 private:
    static std::unique_ptr<T> instance;
    static std::once_flag instanceFlag;

 public:
    Singleton() = delete;
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

    [[nodiscard]] inline static T& get()
    {
        std::call_once(instanceFlag, []() { instance.reset(new T(constructorArgs...)); });

        return *instance;
    }
};

template <typename T, auto... constructorArgs> std::unique_ptr<T> Singleton<T, constructorArgs...>::instance = nullptr;

template <typename T, auto... constructorArgs> std::once_flag Singleton<T, constructorArgs...>::instanceFlag;

#endif // SINGLETON_HPP
