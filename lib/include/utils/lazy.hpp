#ifndef UTILSLAZY_HPP
#define UTILSLAZY_HPP

#include <singleton.hpp>

template <typename T> class Lazy
{
 private:
    T mMember;

 public:
    [[nodiscard]] inline T& get()
    {
        if (!mMember.isLoaded())
        {
            std::ignore = mMember.load();
        }

        return mMember;
    }
};

template <typename T> class LazySingleton
{
 public:
    [[nodiscard]] static inline T& get()
    {
        return Singleton<Lazy<T>>::get().get();
    }
};

#endif // UTILSLAZY_HPP
